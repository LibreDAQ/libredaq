/**********************************************************
 *  LibreDAQ PC interface library
 *   (C) 2015 
 *   License GNU GPL 3
 **********************************************************/

#include <libredaq.h>

int main(int argc, char **argv)
{
	// Declare the object that represents one LibreDAQ physical device
	libredaq::Device  daq;

	// Establish communications:
	const std::string sSerialPort = "COM25";
	daq.connect_serial_port(sSerialPort);
	

	
	return 0;
}
