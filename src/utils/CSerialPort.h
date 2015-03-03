/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  |                                                                         |
  | NOTE: This class is derived from CSerialPort from the MRPT project:     |
  |                     Mobile Robot Programming Toolkit (MRPT)             |
  |                          http://www.mrpt.org/                           |
  |                                                                         |
  | Copyright (c) 2005-2015, Individual contributors, see AUTHORS file      |
  | See: http://www.mrpt.org/Authors - All rights reserved.                 |
  | Released under BSD License. See details in http://www.mrpt.org/License  |
  +-------------------------------------------------------------------------+  */

#pragma once

#include <string>
#include <stdexcept>

#include "CTicTac.h"

/** A communication serial port.
	* On communication errors (eg. the given port number does not exist, timeouts,...), most of the methods will
	* raise an exception of the class "std::exception"
	*
	*  The serial port to open is passed in the constructor in the form of a string description,
	*   which is platform dependent.
	*
	*  In windows they are numbered "COM1"-"COM4" and "\\.\COMXXX" for numbers above.
	*    It is recomended to always use the prefix "\\.\" despite the actual port number.
	*
	*  In Linux the name must refer to the device, for example: "ttyUSB0","ttyS0". If the name string does not
	*   start with "/" (an absolute path), the constructor will assume the prefix "/dev/".
	*
	*  History:
	*    - 1/DEC/2005:  (JLBC) First version
	*    - 20/DEC/2006: (JLBC) Integration into the MRPT framework
	*    - 12/DEC/2007: (JLBC) Added support for Linux.
	*    - 03/MAR/2015: (JLBC) Ported to libldaq
	*/
class CSerialPort
{
public:
	/** Constructor
		* \param portName The serial port to open. See comments at the begining of this page.
		* \param openNow Whether to try to open the port now. If not selected, the port should be open later with "open()".
		*/
	CSerialPort( const std::string &portName, bool openNow = true );

	/** Default constructor: it does not open any port - later you must call "setSerialPortName" and then "open" */
	CSerialPort();

	/** Destructor */
	virtual ~CSerialPort();

	/** Sets the serial port to open (it is an error to try to change this while open yet).
		* \sa open, close
		*/
	void setSerialPortName( const std::string &COM_name );

	/** Open the port. If is already open results in no action.
	* \exception std::exception On communication errors
	*/
	void  open();

	/** Open the given serial port. If it is already open and the name does not match, an exception is raised.
	* \exception std::exception On communication errors or a different serial port already open.
	*/
	void  open(const std::string &COM_name);

	/** Close the port. If is already closed, results in no action. */
	void  close();

	/** Returns if port has been correctly open. */
	bool  isOpen() const;

	/** Purge tx and rx buffers.
		* \exception std::exception On communication errors
		*/
	void  purgeBuffers();

	/** Changes the configuration of the port.
	*  \param parity  0:No parity, 1:Odd, 2:Even (WINDOWS ONLY: 3:Mark, 4:Space)
	*  \param baudRate The desired baud rate Accepted values: 50 - 230400
	*  \param bits Bits per word (typ. 8) Accepted values: 5,6,7,8.
	*  \param nStopBits Stop bits (typ. 1) Accepted values: 1,2
	*  \param enableFlowControl Whether to enable the hardware flow control (RTS/CTS) (default=no)
	* \exception std::exception On communication errors
	*/
	void  setConfig(
		int baudRate,
		int parity = 0,
		int bits = 8,
		int nStopBits = 1,
		bool enableFlowControl=false);

	/** Changes the timeouts of the port, in milliseconds.
	* \exception std::exception On communication errors
	*/
	void  setTimeouts(
		int		ReadIntervalTimeout,
		int		ReadTotalTimeoutMultiplier,
		int		ReadTotalTimeoutConstant,
		int		WriteTotalTimeoutMultiplier,
		int		WriteTotalTimeoutConstant );


	/** Read incoming data. This method will not raise an exception on zero bytes read, as long as there is not any fatal error in the communications.
		* \exception std::exception On communication errors
		*/
	size_t  Read(void *Buffer, size_t Count);

	/** Reads one text line from the serial port in POSIX "canonical mode".
		*  This method reads from the serial port until one of the characters in \a eol are found.
		* \param eol_chars A line reception is finished when one of these characters is found. Default: LF (10), CR (13).
		* \param total_timeout_ms If >0, the maximum number of milliseconds to wait.
		* \param out_timeout If provided, will hold true on return if a timeout ocurred, false on a valid read.
		* \return The read string, without the final
		* \exception std::exception On communication errors
		*/
	std::string ReadString(const int total_timeout_ms=-1, bool *out_timeout =NULL, const char *eol_chars = "\r\n");

	/** Implements the virtual method responsible for writing to the stream.
		*  Write attempts to write up to Count bytes to Buffer, and returns the number of bytes actually written.
	* \exception std::exception On communication errors
		*/
	size_t  Write(const void *Buffer, size_t Count);

protected:
	std::string    m_serialName;  //!< The complete name of the serial port device (i.e. "\\.\COM10","/dev/ttyS2",...)
	int            m_baudRate;
	int            m_totalTimeout_ms,m_interBytesTimeout_ms;
	CTicTac        m_timer; //!< Used only in \a ReadString

#ifdef _WIN32
	// WINDOWS
	void		*hCOM;
#else
	// LINUX
	int 		hCOM; //!< The file handle (-1: Not open)
#endif

}; // end of class

