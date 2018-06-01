/**********************************************************
 *  LibreDAQ PC interface library
 *   (C) 2015
 *   License GNU GPL 3
 **********************************************************/

#include <cstdio>
#include <libredaq.h>

// Callback for ENC data
void my_callback_ENC(const libredaq::TCallbackData_ENC &data) {
  static int i = 0;
  if (++i == 5000) {
    i = 0;
    printf("TIME: %15.7f ENCODERS: %8f %8f %8f %8f\n", data.device_timestamp,
           (double)data.enc_ticks[0], (double)data.enc_ticks[1],
           (double)data.enc_ticks[2], (double)data.enc_ticks[3]);
  }

#if 1
  static FILE *f = fopen("enc.txt", "wt");
  fprintf(f, "%15.7f", data.device_timestamp);
  for (int k = 0; k < data.enc_ticks.size(); k++)
    fprintf(f, " %f", (double)data.enc_ticks[k]);
  fprintf(f, "\n");
#endif
}

int main(int argc, char **argv) {
  // Declare the object that represents one LibreDAQ physical device
  libredaq::Device daq;

  // Establish communications:
  const std::string sSerialPort = "ttyACM0";
  daq.connect_serial_port(sSerialPort);

  daq.set_callback_ENC(&my_callback_ENC);

  printf("Starting ENCODERS task...\n");
  daq.start_task_encoders(100);

  libredaq::sleep_ms(10000);

  printf("Stopping ENCODERS task...\n");
  daq.stop_all_tasks(); // This is not required as it will be done anyway in the
                        // destructor, but it is good practice.

  return 0;
}
