/**********************************************************
 *  LibreDAQ PC interface library
 *   (C) 2015
 *   License GNU GPL 3
 **********************************************************/

#include <libredaq.h>

#include <cstdio>

int main(int argc, char** argv)
{
    // Declare the object that represents one LibreDAQ physical device
    libredaq::Device daq;

    // Establish communications:
    const std::string sSerialPort = "COM9";
    daq.connect_serial_port(sSerialPort);

    printf("Starting sending DAC values...\n");

    while (1)
    {
        uint16_t vals[4];
        for (int i = 0; i < 10; i++)
        {
            vals[0] = i * 100;
            vals[1] = 100 + i * 100;
            vals[2] = 200 + i * 100;
            vals[3] = 300 + i * 100;
            daq.dac_set_values(vals);

            libredaq::sleep_ms(5);
        }
    }

    printf("Done.\n");
    daq.stop_all_tasks();  // This is not required as it will be done anyway in
                           // the destructor, but it is good practice.

    return 0;
}
