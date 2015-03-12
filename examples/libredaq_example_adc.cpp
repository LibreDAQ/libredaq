/**********************************************************
 *  LibreDAQ PC interface library
 *   (C) 2015 
 *   License GNU GPL 3
 **********************************************************/

#include <libredaq.h>

// Callback for ADC data
void my_callback_ADC(const libredaq::TCallbackData_ADC &data)
{
	static int i=0;
	if (++i==100) 
	{
		i=0;
		printf("TIME: %10u ADC DATA: %5.04f\n", data.device_timestamp, data.adc_data_volts[0] );
	}
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

	libredaq::sleep_ms(10000);
	printf("Stopping ADC task...\n");
	daq.stop_all_tasks();

	return 0;
}
