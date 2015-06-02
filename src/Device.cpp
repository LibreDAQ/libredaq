/*+-------------------------------------------------------------------------+
|                             LibreDAQ                                    |
|                                                                         |
| Copyright (C) 2015  Jose Luis Blanco Claraco                            |
| Distributed under GNU General Public License version 3                  |
|   See <http://www.gnu.org/licenses/>                                    |
+-------------------------------------------------------------------------+  */

#include "libredaq.h"

#include "frames.h"  // common header between PC SDK and firmware

#include "utils/CSerialPort.h"
#include "utils/threads.h"
#include "utils/circular_buffer.h"

#include <cstdio>  // stderr,...
#include <cstring> // memcpy

using namespace libredaq;
using libredaq::system::TThreadHandle;
using libredaq::utils::circular_buffer;

// All these complications around opaque pointers are done to minimize 
// as much as possible the #include space of the user of <libredaq.h>
#define PTR_SERIALPORT (reinterpret_cast<CSerialPort*>(m_ptr_serial_port))
#define PTR_HANDLE_RX_THREAD (reinterpret_cast<libredaq::system::TThreadHandle*>(m_rx_thread_handle))
#define PTR_RX_BUFFER (reinterpret_cast<libredaq::utils::circular_buffer<unsigned char>*>(m_rx_buf))

const size_t RX_BUFFER_SIZE = 0x1000;

/** Sleep for the given number of milliseconds */
void libredaq::sleep_ms(unsigned int ms)
{
	libredaq::system::sleep(ms);
}


Device::Device() :
	m_ptr_serial_port( new CSerialPort() ),
	m_rx_thread_handle( new TThreadHandle),
	m_rx_buf(new circular_buffer<unsigned char>(RX_BUFFER_SIZE) ),
	m_all_threads_must_exit(false),
	// Callbacks:
	m_callback_adc(NULL)
{
	*PTR_HANDLE_RX_THREAD  = libredaq::system::createThreadFromObjectMethod(this, &Device::thread_rx );
}

Device::~Device()
{
	this->stop_all_tasks();
	this->disconnect();

	m_all_threads_must_exit = true;
	libredaq::system::sleep(100);

	// Free all objects alloc'd via opaque pointers: (these will never be NULL)
	delete PTR_SERIALPORT;
	delete PTR_HANDLE_RX_THREAD;
	delete PTR_RX_BUFFER;
}

bool Device::connect_serial_port( const std::string &serialPortName )
{
	try
	{
		// Open serial port:
		PTR_SERIALPORT->open(serialPortName);

		// Timeouts (ms)
		const int ReadTotalTimeoutConstant = 3;
		const int WriteTotalTimeoutConstant = 10;
		PTR_SERIALPORT->setTimeouts(0,0,ReadTotalTimeoutConstant,0,WriteTotalTimeoutConstant);  // See function docs for arguments

		// Start ...
		// ........
		//TODO: Send ID command and wait to detect the board model and features

		return true; // ok
	}
	catch (std::exception &e)
	{
		fprintf(stderr,"%s",e.what());
		return false; // error
	}
}

void Device::disconnect()
{
	PTR_SERIALPORT->close();
}

void Device::stop_all_tasks()
{
}


bool Device::start_task_adc( double sampling_rate_hz )
{
	TFrameDAQ_ADC_Start cmd;

	// TODO: Check sanity of required rate for this board!
	cmd.sampling_rate_khz = uint8_t(sampling_rate_hz / 1000);

	return internal_send_cmd(&cmd,sizeof(cmd),"start_task_adc");
}

void Device::thread_rx()
{
	libredaq::utils::circular_buffer<unsigned char> &rx_buf = *PTR_RX_BUFFER;

	// ----------- These vars are declared here to avoid wasting time reallocating the mem buffers -----------
	TCallbackData_ADC adc16b_x8_data;
	// ------------------------------------------------------------------------------------------

	// Bandwidth stats:
	size_t  num_bytes_rx = 0, num_frames_rx = 0;
	CTicTac num_bytes_rx_timer;

	// Main thrad loop:
	while (!m_all_threads_must_exit)
	{
		// Skip if we don't have an open serial link:
		if (!PTR_SERIALPORT->isOpen())
		{
			libredaq::system::sleep(100);
			continue;
		}

		// Try to read data and parse it as frames:
		try
		{
			unsigned char buf[0x800];
			const size_t nActualRead = PTR_SERIALPORT->Read(buf,sizeof(buf));
			if (nActualRead)
				rx_buf.push_many(buf,nActualRead);

			// Bandwidth stats:
			{
				num_bytes_rx+=nActualRead;
				const double At = num_bytes_rx_timer.Tac();
				if (At>1.0)
				{
					const double RX_kBytesPerSec = 1e-3*num_bytes_rx/At;
					const double RX_FramesPerSec = num_frames_rx/At;
					num_bytes_rx_timer.Tic();
					num_bytes_rx=0;
					num_frames_rx=0;
					printf("[libredaq::Device::thread_rx] RX Bandwidth=%7.3f KB/s | %7.3f Frames/s\n",RX_kBytesPerSec,RX_FramesPerSec);
				}
			}
		}
		catch(std::exception &e)
		{
			// Comms error:
			fprintf(stderr,"[libredaq::Device::thread_rx] Communication error reading from device, now closing serial port link.\nError details: %s\n",e.what());
			//PTR_SERIALPORT->close();
			libredaq::system::sleep(100);
			continue;
		}

		// -----------------------------------
		//   Process pending input queue
		// -----------------------------------
		while (rx_buf.size()>0)
		{
			if (rx_buf.peek(0)!=LDAQ_FRAME_START)
			{
				// It's not a valid frame start, ignore this byte:
				rx_buf.pop();
				continue;
			}

			// Ok, we have are aligned to a valid frame start:
			// HEADER | OPCODE | LEN | DATA | TAIL 
			//   1        1        1    LEN    1   = 4+LEN
			if (rx_buf.size()<4) 
				break; // We are sure there is not a complete frame, wait to it to be rx....

			const uint8_t payload_len = rx_buf.peek(2);
			const size_t total_frame_len = 4+payload_len;
			if (rx_buf.size()<total_frame_len)
				break;  // We are sure there is not a complete frame, wait to it to be rx....
			
			// Yes: we have a COMPLETE FRAME
			// --------------------------------
			unsigned char frame_buf[0x400];
			rx_buf.pop_many(frame_buf,total_frame_len);
			num_frames_rx++;

			// Decode depending on OPCODE:
			const uint8_t opcode = frame_buf[1];

			// -----------------------------------
			//   Dispatch RX frames
			// -----------------------------------
			switch(opcode)
			{
			case FRAME_ADC16b_x8:
				{
					TFrameDAQ_ADC<8,int16_t>  adc16b_x8(0);
					::memcpy(&adc16b_x8,frame_buf,sizeof(adc16b_x8));
					//onReceive_ADC16b_x8(adc16b_x8);

					if (m_callback_adc) {
						adc16b_x8_data.device_timestamp = adc16b_x8.time;
						adc16b_x8_data.num_channels = 8;
						adc16b_x8_data.adc_data_volts.resize(1*adc16b_x8_data.num_channels);

						for (unsigned int i=0;i<adc16b_x8_data.adc_data_volts.size();i++)
						{
							adc16b_x8_data.adc_data_volts[i] = adc16b_x8.adcs[i] * 10.0/(0x7FFF);
						}

						(*m_callback_adc)(adc16b_x8_data);
					}
				}
				break;

			default:
				printf("[libredaq] WARNING: Ignoring received frame with unknown OPCODE 0x%02X\n",opcode);
			};
		}

		// Loop:
		libredaq::system::sleep(1);
	}
}

bool Device::internal_send_cmd(void *buf, size_t len, const char *error_msg_cmd)
{
	try
	{
		const size_t nActualWrite = PTR_SERIALPORT->Write(buf,len);
		if (nActualWrite!=len)
			throw std::runtime_error("Could not write all bytes.");
		return true;
	}
	catch(std::exception &e)
	{
		// Comms error:
		fprintf(stderr,"[libredaq::Device::%s] Communication error writing to device, now closing serial port link.\nError details: %s\n",error_msg_cmd, e.what());
		PTR_SERIALPORT->close();
		return false;
	}
}

