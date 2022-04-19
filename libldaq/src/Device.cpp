/*+-------------------------------------------------------------------------+
|                             LibreDAQ                                    |
|                                                                         |
| Copyright (C) 2015  Jose Luis Blanco Claraco                            |
| Distributed under GNU General Public License version 3                  |
|   See <http://www.gnu.org/licenses/>                                    |
+-------------------------------------------------------------------------+  */

#include <cstdio>  // stderr,...
#include <cstring>  // memcpy
#include <thread>

#include "ldaq_frames.h"  // common header between PC SDK and firmware
#include "libredaq.h"
#include "utils/CSerialPort.h"
#include "utils/circular_buffer.h"

using namespace libredaq;

constexpr size_t RX_BUFFER_SIZE = 0x4000;

/** Sleep for the given number of milliseconds */
void libredaq::sleep_ms(unsigned int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

Device::Device()
{
    m_ptr_serial_port = std::make_unique<libredaq::internal::CSerialPort>();
    m_rx_buf =
        std::make_unique<internal::circular_buffer<uint8_t>>(RX_BUFFER_SIZE);
    m_rx_thread_handle = std::thread(&Device::thread_rx, this);
}

Device::~Device()
{
    this->stop_all_tasks();
    this->disconnect();

    m_all_threads_must_exit = true;
    sleep_ms(100);
}

bool Device::connect_serial_port(const std::string& serialPortName)
{
    try
    {
        // Open serial port:
        m_ptr_serial_port->open(serialPortName);

        // Timeouts (ms)
        const int ReadTotalTimeoutConstant  = 3;
        const int WriteTotalTimeoutConstant = 10;
        m_ptr_serial_port->setTimeouts(
            0, 0, ReadTotalTimeoutConstant, 0,
            WriteTotalTimeoutConstant);  // See function docs for arguments

        // Start ...
        // ........
        // TODO: Send ID command and wait to detect the board model and features

        // TODO: Auto-detect m_device_tick_period

        return true;  // ok
    }
    catch (std::exception& e)
    {
        fprintf(stderr, "%s", e.what());
        return false;  // error
    }
}

void Device::disconnect() { m_ptr_serial_port->close(); }

bool Device::stop_all_tasks()
{
    TFrame_LDAQCMD_StopAllTasks cmd;
    return internal_send_cmd(&cmd, sizeof(cmd), "end_all_tasks");
}

bool Device::start_task_adc(unsigned int sampling_rate_hz)
{
    TFrame_LDAQCMD_ADC_Start cmd;
    // TODO: Check sanity of required rate for this board!
    cmd.sampling_rate_hz = sampling_rate_hz;
    return internal_send_cmd(&cmd, sizeof(cmd), "start_task_adc");
}

bool Device::start_task_pga_adc(
    unsigned int sampling_rate_hz, unsigned int PGA_gain)
{
    TFrame_LDAQCMD_ADC_AMP_Start cmd;
    // TODO: Check sanity of required rate for this board!
    cmd.sampling_rate_hz = sampling_rate_hz;
    cmd.desired_gain     = PGA_gain;
    m_pga_value          = PGA_gain;
    return internal_send_cmd(&cmd, sizeof(cmd), "start_task_pga_adc");
}

bool Device::start_task_encoders(unsigned int sampling_rate_hz)
{
    TFrame_LDAQCMD_ENC_Start cmd;

    // TODO: Check sanity of required rate for this board!
    cmd.sampling_rate_hz = sampling_rate_hz;

    return internal_send_cmd(&cmd, sizeof(cmd), "start_task_enc");
}

bool Device::dac_set_values(uint16_t* vals)
{
    TFrame_LDAQCMD_DAC_SetValues cmd;
    // TODO: Check sanity of required rate for this board!
    for (int i = 0; i < 4; i++) cmd.dac_values[i] = vals[i];
    return internal_send_cmd(&cmd, sizeof(cmd), "dac_set_values");
}

bool Device::switch_firmware_mode(uint8_t mode)
{
    TFrame_LDAQCMD_SwitchFirmwareMode cmd;
    cmd.new_firmware_mode = mode;
    return internal_send_cmd(&cmd, sizeof(cmd), "switch_firmware_mode");
}

void Device::thread_rx()
{
    auto& rx_buf = *m_rx_buf;

    // These vars are declared here to avoid wasting time reallocating the mem
    // buffers
    TCallbackData_ADC adc16b_x8_data, adc24b_x4_data;
    TCallbackData_ENC enc32b_x4_data;

    // Bandwidth stats:
    size_t  num_bytes_rx = 0, num_skipped_bytes_rx = 0, num_frames_rx = 0;
    CTicTac num_bytes_rx_timer;

    // Main thrad loop:
    while (!m_all_threads_must_exit)
    {
        // Skip if we don't have an open serial link:
        if (!m_ptr_serial_port->isOpen())
        {
            sleep_ms(100);
            continue;
        }

        // Try to read data and parse it as frames:
        try
        {
            unsigned char buf[0x2000];
            const size_t  nActualRead =
                m_ptr_serial_port->Read(buf, sizeof(buf));
            if (nActualRead) rx_buf.push_many(buf, nActualRead);

            // Bandwidth stats:
            {
                num_bytes_rx += nActualRead;
                const double At = num_bytes_rx_timer.Tac();
                if (At > 1.0)
                {
                    const double RX_kBytesPerSec = 1e-3 * num_bytes_rx / At;
                    const double RX_kBytesSkipPerSec =
                        1e-3 * num_skipped_bytes_rx / At;
                    const double RX_FramesPerSec = num_frames_rx / At;
                    num_bytes_rx_timer.Tic();
                    num_bytes_rx  = 0;
                    num_frames_rx = 0;
                    printf(
                        "[libredaq::thread_rx] RX=%7.3f KB/s | %7.3f Fr/s | "
                        "Ign=%6.3f KB/s\n",
                        RX_kBytesPerSec, RX_FramesPerSec, RX_kBytesSkipPerSec);
                }
            }
        }
        catch (std::exception& e)
        {
            // Comms error:
            fprintf(
                stderr,
                "[libredaq::Device::thread_rx] Communication error reading "
                "from device, now closing serial port link.\nError details: "
                "%s\n",
                e.what());
            // m_ptr_serial_port->close();
            sleep_ms(100);
            continue;
        }

        // -----------------------------------
        //   Process pending input queue
        // -----------------------------------
        while (rx_buf.size() > 0)
        {
            if (rx_buf.peek(0) != LDAQ_FRAME_START)
            {
                // It's not a valid frame start, ignore this byte:
                rx_buf.pop();
                num_skipped_bytes_rx++;
                continue;
            }

            // Ok, we have are aligned to a valid frame start:
            // HEADER | OPCODE | LEN | DATA | TAIL
            //   1        1        1    LEN    1   = 4+LEN
            if (rx_buf.size() < 4)
                break;  // We are sure there is not a complete frame, wait to it
                        // to be rx....

            const uint8_t payload_len     = rx_buf.peek(2);
            const size_t  total_frame_len = 4 + payload_len;
            if (rx_buf.size() < total_frame_len)
                break;  // We are sure there is not a complete frame, wait to it
                        // to be rx....

            // Yes: we have a COMPLETE FRAME
            // --------------------------------
            unsigned char frame_buf[0x400];
            rx_buf.pop_many(frame_buf, total_frame_len);
            num_frames_rx++;

            // Decode depending on OPCODE:
            const uint8_t opcode = frame_buf[1];

            // -----------------------------------
            //   Dispatch RX frames
            // -----------------------------------
            switch (opcode)
            {
                case FRAME_ADC_16bx8:
                {
                    TFrame_LDAQDATA_ADC_16bx8 adc16b_x8;
                    ::memcpy(&adc16b_x8, frame_buf, sizeof(adc16b_x8));
                    // onReceive_ADC16b_x8(adc16b_x8);

                    // TODO: Handle depending on slot_idx and its known
                    // features!
                    if (m_callback_adc)
                    {
                        adc16b_x8_data.device_timestamp =
                            adc16b_x8.time * m_device_tick_period;
                        adc16b_x8_data.num_channels = 8;
                        adc16b_x8_data.adc_data_volts.resize(
                            1 * adc16b_x8_data.num_channels);

                        for (unsigned int i = 0;
                             i < adc16b_x8_data.adc_data_volts.size(); i++)
                        {
                            adc16b_x8_data.adc_data_volts[i] =
                                adc16b_x8.adcs[i] * 10.0 / (0x7FFF);
                        }

                        m_callback_adc(adc16b_x8_data);
                    }
                }
                break;

                case FRAME_ENC_32bx4:
                {
                    TFrame_LDAQDATA_ENC_32bx4 enc;
                    ::memcpy(&enc, frame_buf, sizeof(enc));
                    // onReceive_ENC32b_x4(enc);

                    if (m_callback_enc)
                    {
                        enc32b_x4_data.device_timestamp =
                            enc.time * m_device_tick_period;
                        enc32b_x4_data.num_channels = 4;
                        enc32b_x4_data.enc_ticks.resize(
                            1 * enc32b_x4_data.num_channels);

                        for (unsigned int i = 0;
                             i < enc32b_x4_data.enc_ticks.size(); i++)
                            enc32b_x4_data.enc_ticks[i] = enc.tickpos[i];

                        m_callback_enc(enc32b_x4_data);
                    }
                }
                break;

                case FRAME_ADC_24bx4:
                {
                    TFrame_LDAQDATA_ADC_24bx4 adc24b_x4;
                    ::memcpy(&adc24b_x4, frame_buf, sizeof(adc24b_x4));

                    // TODO: Handle depending on slot_idx and its known
                    // features!
                    if (m_callback_adc)
                    {
                        adc24b_x4_data.device_timestamp =
                            adc24b_x4.time * m_device_tick_period;
                        adc24b_x4_data.num_channels = 4;
                        adc24b_x4_data.adc_data_volts.resize(
                            1 * adc24b_x4_data.num_channels);

                        const double k_res =
                            (2.048 / ((2 << 23) - 1)) / m_pga_value;

                        for (unsigned int i = 0;
                             i < adc24b_x4_data.adc_data_volts.size(); i++)
                        {  // Convert from 24bit -> double
                            const uint8_t* ptr = &adc24b_x4.adcs[i * 3];
                            int32_t        val = (((uint32_t)ptr[2]) << 16) |
                                          (((uint32_t)ptr[1]) << 8) |
                                          (uint32_t)(ptr[0]);
                            adc24b_x4_data.adc_data_volts[i] = val * k_res;
                        }

                        m_callback_adc(adc24b_x4_data);
                    }
                }
                break;

                default:
                    printf(
                        "[libredaq] WARNING: Ignoring received frame with "
                        "unknown OPCODE 0x%02X\n",
                        opcode);
            };
        }

        // Loop:
        sleep_ms(1);
    }
}

bool Device::internal_send_cmd(void* buf, size_t len, const char* error_msg_cmd)
{
    try
    {
        const size_t nActualWrite = m_ptr_serial_port->Write(buf, len);
        if (nActualWrite != len)
            throw std::runtime_error("Could not write all bytes.");
        return true;
    }
    catch (std::exception& e)
    {
        // Comms error:
        fprintf(
            stderr,
            "[libredaq::Device::%s] Communication error writing to device, now "
            "closing serial port link.\nError details: %s\n",
            error_msg_cmd, e.what());
        m_ptr_serial_port->close();
        return false;
    }
}
