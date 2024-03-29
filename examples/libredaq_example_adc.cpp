/**********************************************************
 *  LibreDAQ PC interface library
 *   (C) 2015
 *   License GNU GPL 3
 **********************************************************/

#include <libredaq.h>
#include <format.h>

#include <cstdio>
#include <iostream>
#include <fstream>

std::ofstream fOut;

// Callback for ADC data
void my_callback_ADC(const libredaq::TCallbackData_ADC& data)
{
    static int i = 0;
    if (++i == 10000)
    {
        i = 0;
        printf(
            "TIME: %15.7f ADC DATA: 0=%5.03f 1=%5.03f 2=%5.03f 3=%5.03f "
            "4=%5.03f\n",
            data.device_timestamp, data.adc_data_volts[0],
            data.adc_data_volts[1], data.adc_data_volts[2],
            data.adc_data_volts[3], data.adc_data_volts[4]);
    }

#if 1
    if (!fOut.is_open()) fOut.open("adc.txt");

    fOut << libredaq::format("%15.7f", data.device_timestamp);
    for (size_t k = 0; k < data.adc_data_volts.size(); k++)
        fOut << libredaq::format(" %5.05f", data.adc_data_volts[k]);
    fOut << "\n";
#endif
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <COM_PORT>\n";
        return 1;
    }

    // Declare the object that represents one LibreDAQ physical device
    libredaq::Device daq;

    // Establish communications:
    const std::string sSerialPort = argv[1];
    daq.connect_serial_port(sSerialPort);

    daq.set_callback_ADC(&my_callback_ADC);

#if 0
	// Use special firmware mode: high-speed ADC (100 KSPS)
	daq.switch_firmware_mode(1);
#else
    daq.switch_firmware_mode(0);
    printf("Starting ADC task...\n");
    daq.start_task_adc(10000);
#endif
    libredaq::sleep_ms(15000);

    printf("Stopping ADC task...\n");
    daq.stop_all_tasks();  // This is not required as it will be done anyway in
                           // the destructor, but it is good practice.

    libredaq::sleep_ms(1000);
    fOut.close();
    return 0;
}
