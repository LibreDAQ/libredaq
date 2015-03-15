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
	if (++i==100)
	{
		i=0;
		printf("TIME: %10u ADC DATA: 0=%5.03f 1=%5.03f 2=%5.03f 3=%5.03f 4=%5.03f\n", (unsigned int)data.device_timestamp, data.adc_data_volts[0],data.adc_data_volts[1],data.adc_data_volts[2],data.adc_data_volts[3],data.adc_data_volts[4] );
	}

	static FILE* f=fopen("adc.txt","wt");
	fprintf(f,"%10u", (unsigned int)data.device_timestamp);
	for (int k=0;k<data.adc_data_volts.size();k++)
		fprintf(f," %5.03f",data.adc_data_volts[k] );
	fprintf(f,"\n");
}

int main(int argc, char **argv)
{
	// Declare the object that represents one LibreDAQ physical device
	libredaq::Device  daq;

	// Establish communications:
	const std::string sSerialPort = "COM25";
	daq.connect_serial_port(sSerialPort);

	daq.set_callback_ADC(&my_callback_ADC);

	printf("Starting ADC task...\n");
	daq.start_task_adc(1000);

	libredaq::sleep_ms(5000);
	printf("Stopping ADC task...\n");
	daq.stop_all_tasks();

	return 0;
}
