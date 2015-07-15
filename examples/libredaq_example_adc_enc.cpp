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
		printf("TIME: %15.7f ENCODERS: %8f %8f %8f %8f\n", data.device_timestamp, (double)data.enc_ticks[0], (double)data.enc_ticks[1], (double)data.enc_ticks[2], (double)data.enc_ticks[3]);
	}

#if 1
	static FILE* f=fopen("enc.txt","wt");
	fprintf(f,"%15.7f", data.device_timestamp);
	for (int k=0;k<data.enc_ticks.size();k++)
		fprintf(f," %f",(double)data.enc_ticks[k] );
	fprintf(f,"\n");
#endif
}

// Callback for ADC data
void my_callback_ADC(const libredaq::TCallbackData_ADC &data)
{
	static int i = 0;
	if (++i == 10000)
	{
		i = 0;
		printf("TIME: %15.7f ADC DATA: 0=%5.03f 1=%5.03f 2=%5.03f 3=%5.03f 4=%5.03f\n", data.device_timestamp, data.adc_data_volts[0], data.adc_data_volts[1], data.adc_data_volts[2], data.adc_data_volts[3], data.adc_data_volts[4]);
	}

#if 1
	static FILE* f = fopen("adc.txt", "wt");
	fprintf(f, "%15.7f", data.device_timestamp);
	for (int k = 0; k<data.adc_data_volts.size(); k++)
		fprintf(f, " %5.05f", data.adc_data_volts[k]);
	fprintf(f, "\n");
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
	daq.set_callback_ADC(&my_callback_ADC);

	printf("Starting ADC and ENCODERS tasks...\n");
	daq.start_task_encoders(25000);
	daq.start_task_adc(25000);

	libredaq::sleep_ms(10000);

	printf("Stopping tasks...\n");
	daq.stop_all_tasks();  // This is not required as it will be done anyway in the destructor, but it is good practice.

	return 0;
}
