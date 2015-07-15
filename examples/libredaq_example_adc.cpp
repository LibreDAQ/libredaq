/**********************************************************
 *  LibreDAQ PC interface library
 *   (C) 2015 
 *   License GNU GPL 3
 **********************************************************/

#include <libredaq.h>
#include <cstdio>

// Callback for ADC data
void my_callback_ADC(const libredaq::TCallbackData_ADC &data)
{
	static int i=0;
	if (++i==10000)
	{
		i=0;
		printf("TIME: %15.7f ADC DATA: 0=%5.03f 1=%5.03f 2=%5.03f 3=%5.03f 4=%5.03f\n", data.device_timestamp, data.adc_data_volts[0],data.adc_data_volts[1],data.adc_data_volts[2],data.adc_data_volts[3],data.adc_data_volts[4] );
	}

#if 1
	static FILE* f=fopen("adc.txt","wt");
	fprintf(f,"%15.7f", data.device_timestamp);
	for (int k=0;k<data.adc_data_volts.size();k++)
		fprintf(f," %5.05f",data.adc_data_volts[k] );
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

	daq.set_callback_ADC(&my_callback_ADC);

#if 0
	daq.switch_firmware_mode(1);
#else
	daq.switch_firmware_mode(0);
	printf("Starting ADC task...\n");
	daq.start_task_adc(5000);  // 20000
#endif
	libredaq::sleep_ms(15000);
	
	printf("Stopping ADC task...\n");
	daq.stop_all_tasks();  // This is not required as it will be done anyway in the destructor, but it is good practice.

	return 0;
}
