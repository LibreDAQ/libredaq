/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+ */

#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

namespace libredaq
{
// forward decls:
namespace internal
{
class CSerialPort;
template <typename T>
class circular_buffer;
}  // namespace internal

/** @name Callback types
 * @{ */

/** Data for callbacks of type ADC */
struct TCallbackData_ADC
{
    double       device_timestamp;  //!< Timestamp in seconds.
    unsigned int num_channels;  //!< Number of ADC channels
    /** Interlaced ADC data, in volts: [A0 ... A7](for t=0), [A0 ... A7](for
     * t=1), etc. */
    std::vector<double> adc_data_volts;
};

/** Callback for ADC data */
using callback_adc_t = std::function<void(const TCallbackData_ADC& data)>;

/** Data for callbacks of type ENCODER */
struct TCallbackData_ENC
{
    double       device_timestamp;  //!< Timestamp in seconds
    unsigned int num_channels;  //!< Number of ADC channels
    /** Interlaced ENCODER data, in ticks: [ENC0 ... ENC3](for t=0), [ENC0 ...
     * ENC7](for t=1), etc. */
    std::vector<int32_t> enc_ticks;
};

/** Callback for ENCODER data */
using callback_enc_t = std::function<void(const TCallbackData_ENC& data)>;

/** @} */

/** @name Main LibreDAQ API
 * @{ */

/** The interface to a LibreDAQ board.
 */
class Device
{
   public:
    /** Constructor of an unitilized device. Must be connected with \ref
     * connect() */
    Device();

    /** Destructor. It automatically stop all tasks and disconnects from the
     * device. */
    virtual ~Device();

    /** Connects to a device in a given serial port
     * \return  true on success, false on any error
     */
    bool connect_serial_port(const std::string& serialPortName);

    /** Disconnect from the device. Note that this does not stop running tasks
     * on the firmware. Does nothing if already disconnected. \sa
     * stop_all_tasks() */
    void disconnect();

    /** Instruct the firmware to stop all running tasks
     * \return  true on success, false on any error */
    bool stop_all_tasks();

    /** Start capturing Analog-to-Digital (ADC) channels
     * \return false on any error (and dumps details to stderr)
     */
    bool start_task_adc(unsigned int sampling_rate_hz);

    /** Start capturing Analog-to-Digital (ADC) channels in a module with a
     * Programmable Gain Amplifier (PGA) \return false on any error (and dumps
     * details to stderr)
     */
    bool start_task_pga_adc(
        unsigned int sampling_rate_hz, unsigned int PGA_gain);

    void set_callback_ADC(const callback_adc_t& user_function)
    {
        m_callback_adc = user_function;
    }

    /** Start capturing Quadrature Encoders channels
     * \return false on any error (and dumps details to stderr)
     */
    bool start_task_encoders(unsigned int sampling_rate_hz);

    void set_callback_ENC(const callback_enc_t& user_function)
    {
        m_callback_enc = user_function;
    }

    bool dac_set_values(uint16_t* vals);  //!< 4 values

    /// Types are declared in firmware_mode_t
    bool switch_firmware_mode(uint8_t mode);

   private:
    std::unique_ptr<internal::CSerialPort>              m_ptr_serial_port;
    std::thread                                         m_rx_thread_handle;
    std::unique_ptr<internal::circular_buffer<uint8_t>> m_rx_buf;

    std::atomic_bool m_all_threads_must_exit{false};

    /// The running thread for
    void thread_rx();

    /// Returns false on any comms error
    bool internal_send_cmd(void* buf, size_t len, const char* error_msg_cmd);

    // Functors for callbacks:
    callback_adc_t m_callback_adc;
    callback_enc_t m_callback_enc;

    double m_pga_value = 1.0;

    /// 1 / frequency of the systick (to convert timestamps to seconds)
    double m_device_tick_period = 1.0 / 50e3;

};  // end class

/** Sleep for the given number of milliseconds */
void sleep_ms(unsigned int ms);

/** @} */
}  // namespace libredaq
