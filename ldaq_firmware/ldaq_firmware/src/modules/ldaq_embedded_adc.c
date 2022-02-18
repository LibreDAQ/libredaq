/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>
#include <adc.h>

void ADC_Handler(void)
{
#if SAM3U
#define ADC_ISR_DRDY 0x0100 
#endif

	// Check the ADC conversion status
	if ((adc_get_status(ADC) & ADC_ISR_DRDY) == ADC_ISR_DRDY)
	{
		// Get latest digital data value from ADC and can be used by application
		uint32_t result = adc_get_latest_value(ADC);
		//printf("%05u\r\n",(unsigned int)result);
	}
}
static void adc_setup(void)
{
	/*
	ADC_FREQ_MIN = 1000000 (1Mhz)
	TRACKING_TIME = 1
	ADC_SETTLING_TIME_3 = 17
	TRANSFER_PERIOD = 1
 
	Sampling rate =  ((TRACKING_TIME * 2 + 3) + (TRANSFER_PERIOD + 1) + ADC_SETTLING_TIME_3) / ADC_FREQ_MIN = (1 + 1 + 17)/1000000 = 52.6Khz
	*/
#define ADC_CLOCK 100000
	
	adc_init(ADC, sysclk_get_main_hz(), ADC_CLOCK, 8);
	adc_configure_timing(ADC, 0);
	adc_set_resolution(ADC, ADC_12_BITS);
	adc_enable_channel(ADC, ADC_CHANNEL_5);
	adc_enable_interrupt(ADC, ADC_IER_DRDY);
	adc_configure_trigger(ADC, ADC_TRIG_SW);
}


