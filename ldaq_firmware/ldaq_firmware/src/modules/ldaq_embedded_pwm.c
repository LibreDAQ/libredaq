/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>
#include "ldaq_embedded_pwm.h"
#include "pwm.h"

/* JP1 connector exposed 5V versions of: 
 - PB10
 - PB14
 - PB17: PWML0  (PERIPH B)
 - PB18: PWML1  (PERIPH B)
 - PB19: PWML2  (PERIPH B)
 - PB20: PWML3  (PERIPH B)
 - PB23
*/

pwm_channel_t pwm_channel_instance;

/** Initialize */
void embedded_pwm_init(void)
{
	pmc_enable_periph_clk(ID_PWM);
	pwm_channel_disable(PWM, PWM_CHANNEL_0);
	pwm_clock_t clock_setting = {
		.ul_clka = 1000 * 100,
		.ul_clkb = 0,
		.ul_mck = 48000000
	};
	pwm_init(PWM, &clock_setting);
	pwm_channel_instance.ul_prescaler = PWM_CMR_CPRE_CLKA;
	pwm_channel_instance.ul_period = 100;
	pwm_channel_instance.ul_duty = 25;
	pwm_channel_instance.channel = PWM_CHANNEL_0;
	pwm_channel_init(PWM, &pwm_channel_instance);
	
	pwm_channel_enable(PWM, PWM_CHANNEL_0);
	
	// Assign IO function:
	pio_set_peripheral(
		PIOB,
		PIO_PERIPH_B,
		(1u << 17) | (1u << 18) | (1u << 19) | (1u << 20)
		);
}

/** Set freq & duty cycle */
void embedded_pwm_set_duty_cycle(uint8_t pwm_idx, uint32_t duty_cycle)
{
	pwm_channel_update_duty(PWM,&pwm_channel_instance,duty_cycle);
	
}

