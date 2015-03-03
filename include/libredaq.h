/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#pragma once

#include <string>

namespace libredaq
{
	/** @name Main LibreDAQ API
	  * @{ */
	
	/** The interface to any LibreDAQ physical device.
	  */
	class Device
	{
	public:
		/** Constructor of an unitilized device. Must be connected with \ref connect() */
		Device();

		/** Destructor. It automatically stop all tasks and disconnects from the device. */
		~Device();

		/** Connects to a device in a given serial port
		  * \return  true on success, false on any error
		  */
		bool connect_serial_port( const std::string &serialPortName );

		/** Disconnect from the device. Note that this does not stop running tasks on the firmware. 
		  * Does nothing if already disconnected.
		  * \sa stop_all_tasks() */
		void disconnect();

		/** Instruct the firmware to stop all running tasks */
		void stop_all_tasks();

	private:
		void *m_ptr_serial_port;  // Opaque ptr to CSerialPort

	}; // end class
	
	/** @} */
}
