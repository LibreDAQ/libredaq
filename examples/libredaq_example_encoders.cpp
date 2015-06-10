/**********************************************************
 *  LibreDAQ PC interface library
 *   (C) 2015
 *   License GNU GPL 3
 **********************************************************/

#include <libredaq.h>
#include <cstdio>

// Callback for ENC data
void my_callback_ENC(const libredaq::TCallbackData_ENC &data)
{
	static int i=0;
	if (++i==5000)
	{
		i=0;
		printf("TIME: %10u ENCODERS: %8li %8li %8li %8li\n", (unsigned long)data.device_timestamp, data.enc_ticks[0],data.enc_ticks[1],data.enc_ticks[2],data.enc_ticks[3]);
	}

#if 0
	static FILE* f=fopen("enc.txt","wt");
	fprintf(f,"%10u", (unsigned long)data.device_timestamp);
	for (int k=0;k<data.enc_ticks.size();k++)
		fprintf(f," %li",data.enc_ticks[k] );
	fprintf(f,"\n");
#endif
}


int main(int argc, char **argv)
{
	// Declare the object that represents one LibreDAQ physical device
	libredaq::Device  daq;

	// Establish communications:
	const std::string sSerialPort = "COM9";
	daq.connect_serial_port(sSerialPort);

	daq.set_callback_ENC(&my_callback_ENC);

	printf("Starting ENCODERS task...\n");
	daq.start_task_encoders(10000);

	libredaq::sleep_ms(10000);

	printf("Stopping ENCODERS task...\n");
	daq.stop_all_tasks();  // This is not required as it will be done anyway in the destructor, but it is good practice.

	return 0;
}
