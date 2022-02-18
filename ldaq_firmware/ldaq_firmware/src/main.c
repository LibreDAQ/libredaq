/*+-------------------------------------------------------------------------+
|                             LibreDAQ                                    |
|                                                                         |
| Copyright (C) 2015  Jose Luis Blanco Claraco                            |
| Distributed under GNU General Public License version 3                  |
|   See <http://www.gnu.org/licenses/>                                    |
+-------------------------------------------------------------------------+  */

/**
* \mainpage User Application template doxygen documentation
*
*
*/
#include <asf.h>

#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <firmware_modes/ldaq_firmware_modes.h>

// for debugging
//#include <modules/ldaq_mod_ADS1248.h>
//#include <modules/ldaq_mod_CY8C9540.h>

int main (void)
{
	// Init everything:
	// ---------------------------------------------------------
	irq_initialize_vectors();
	cpu_irq_enable();
	sleepmgr_init();
	sysclk_init();
	board_init();

	wdt_disable(WDT);  // Make sure the WDT is disabled.
	sysclk_init();     // Initialize system clock
	board_init();      // Init peripherals
	ldaq_comms_init(); // This also sets up the USB stack
	rt_sched_init();   // LibreDAQ Real-time Scheduler system
	cpu_irq_enable();  // Enable interrupts

	// LED flashes to signal restart:
	// ---------------------------------------------------------
	pio_set_pin_high(LED1_GPIO);pio_set_pin_high(LED2_GPIO); delay_ms(150);
	pio_set_pin_low (LED1_GPIO);pio_set_pin_high(LED2_GPIO); delay_ms(150);
	pio_set_pin_high(LED1_GPIO);pio_set_pin_low (LED2_GPIO); delay_ms(150);
	pio_set_pin_low(LED1_GPIO); pio_set_pin_low (LED2_GPIO); delay_ms(150);
	pio_set_pin_high(LED1_GPIO);pio_set_pin_high(LED2_GPIO);

	// Main infinite loop:
	// ---------------------------------------------------------
	while (1)
	{
		ldaq_comms_dispatch_incoming_cmds();  // Fixed idle task: dispatch rx cmds
		ldaq_comms_task_outbounds_queue();    // Fixed idle task: out pkgs
		firm_mode_run_main_loop();            // The rest of tasks: depending on user-set firmware mode
	}

}
