/*+-------------------------------------------------------------------------+
|                             LibreDAQ                                    |
|                                                                         |
| Copyright (C) 2015  Jose Luis Blanco Claraco                            |
| Distributed under GNU General Public License version 3                  |
|   See <http://www.gnu.org/licenses/>                                    |
+-------------------------------------------------------------------------+  */

#include "libredaq.h"

#include "utils/CSerialPort.h"

#define PTR_SERIALPORT (reinterpret_cast<CSerialPort*>(m_ptr_serial_port))

using namespace libredaq;

Device::Device() :
	m_ptr_serial_port( new CSerialPort() )
{

}

Device::~Device()
{
	this->stop_all_tasks();
	this->disconnect();

	// Free all objects alloc'd via opaque pointers:
	if (m_ptr_serial_port)
	{
		delete PTR_SERIALPORT;
		m_ptr_serial_port=NULL;
	}
}

bool Device::connect_serial_port( const std::string &serialPortName )
{
	try
	{
		// Open serial port:
		PTR_SERIALPORT->open(serialPortName);

		// Start ...
		// ........

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

