/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <conf_libredaq_board.h>
#if HAVE_LDAQ_SLOT_BUS

#include <conf_libredaq_firmware.h>
#include <firmware_modes/ldaq_firmware_modes.h>
#include <system/ldaq_rt_scheduler.h>
#include <utils/ldaq_util_macros.h>
#include <modules/ldaq_adc_common.h>
#include <modules/ldaq_mod_ad7606.h>

#include <tc.h>  // ASF Timer counter
#include <pmc.h> // pmc_enable_periph_clk()
#include <sysclk.h> 

// ------------------------------------------------------
//            FIRMWARE MODE: REGFRMMODE_HS_ADC
// ------------------------------------------------------
static void firmware_hsadc_main_loop(void)
{
	task_adc_send_full_frame();
}
static void firmware_hsadc_on_enter(void)
{
	// Init HW:
	#warning TODO: Make general for other ADC modules, etc.
	mod_ad7606_init();
	
	// Set up high-freq timer:
	// ------------------------------
	const uint32_t freq_desired = 100000;  // Max: 50 kHz!
	
	//	freq_desired	The desired rate at which to call the ISR, Hz.
	const uint32_t ul_sysclk = sysclk_get_cpu_hz();
	uint32_t ul_div;
	uint32_t ul_tcclks;
	// Configure PMC
	pmc_enable_periph_clk(ID_TC0);
	// Configure TC and trigger on RC compare.
	tc_find_mck_divisor(
		(uint32_t)freq_desired,	// The desired frequency as a uint32.
		ul_sysclk,				// Master clock freq in Hz.
		&ul_div,				// Pointer to register where divisor will be stored.
		&ul_tcclks,				// Pointer to reg where clock selection number is stored.
		ul_sysclk);				// Board clock freq in Hz.
	tc_init(TC0, 0, ul_tcclks | TC_CMR_CPCTRG);

	// Find the best estimate of counts, then write it to TC register C.
	const uint32_t counts = (ul_sysclk/ul_div)/(freq_desired);
	tc_write_rc(TC0, 0, counts);

	// Enable interrupts for this TC, and start the TC.
	tc_enable_interrupt(TC0,0, TC_IER_CPCS);
	tc_start(TC0,0);
	
	NVIC_DisableIRQ(TC0_IRQn);
	NVIC_ClearPendingIRQ(TC0_IRQn);
	NVIC_SetPriority(TC0_IRQn,LDAQ_INT_PRIORITY_LEVEL_SAMPLING_TIMERS);
	NVIC_EnableIRQ(TC0_IRQn);
	
	// Disable SysTick:
//	SysTick->CTRL  = 0;
//	NVIC_SetPriority (SysTick_IRQn, 15);  /* set Priority for Systick Interrupt */
}
static void firmware_hsadc_on_leave(void)
{
	// Disable timer:
	tc_stop(TC0,0);
	tc_disable_interrupt(TC0, 0, TC_IER_CPCS);
	NVIC_DisableIRQ(TC0_IRQn);

	// Re-Enable SysTick:
//	SysTick_Config( sysclk_get_cpu_hz() / LDAQ_SCHEDULER_FREQ_HZ );
//	NVIC_SetPriority (SysTick_IRQn, LDAQ_INT_PRIORITY_LEVEL_SYSTICK);  /* set Priority for Systick Interrupt */
}

void TC0_Handler(void)
{
//	timer_us++;
	
	/* Clear status bit to acknowledge interrupt */
	volatile uint32_t ul_dummy = ((TcChannel *)(TC0->TC_CHANNEL + 0))->TC_SR; // tc_get_status(TC0, 0); //The compare bit is cleared by reading the register
	UNUSED(ul_dummy);

	task_ad7606_start_conversion();
	//task_adc_send_full_frame(); 
}


// ======== Register firmware mode ========
LDAQ_INIT_BLOCK_START(REGFRMMODE_HS_ADC)
	ldaq_register_firmware_mode(FIRM_MODE_HIGHSPEED_ADC, &firmware_hsadc_main_loop, &firmware_hsadc_on_enter,&firmware_hsadc_on_leave );
LDAQ_INIT_BLOCK_END()

#endif // HAVE_LDAQ_SLOT_BUS
