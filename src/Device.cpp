/*+-------------------------------------------------------------------------+
|                             LibreDAQ                                    |
|                                                                         |
| Copyright (C) 2015  Jose Luis Blanco Claraco                            |
| Distributed under GNU General Public License version 3                  |
|   See <http://www.gnu.org/licenses/>                                    |
+-------------------------------------------------------------------------+  */

#include "libredaq.h"

#include "utils/CSerialPort.h"
#include "utils/threads.h"

using namespace libredaq;
using libredaq::system::TThreadHandle;

#define PTR_SERIALPORT (reinterpret_cast<CSerialPort*>(m_ptr_serial_port))
#define PTR_HANDLE_RX_THREAD (reinterpret_cast<libredaq::system::TThreadHandle*>(m_rx_thread_handle))


Device::Device() :
	m_ptr_serial_port( new CSerialPort() ),
	m_rx_thread_handle( new TThreadHandle),
	m_all_threads_must_exit(false)
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
	delete m_rx_thread_handle;
}

bool Device::connect_serial_port( const std::string &serialPortName )
{
	try
	{
		// Open serial port:
		PTR_SERIALPORT->open(serialPortName);

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

	return true;
}


void Device::thread_rx()
{
	while (!m_all_threads_must_exit)
	{
		if (!PTR_SERIALPORT->isOpen())
		{
			libredaq::system::sleep(100);
			continue;
		}



		libredaq::system::sleep(1);
	}
}

