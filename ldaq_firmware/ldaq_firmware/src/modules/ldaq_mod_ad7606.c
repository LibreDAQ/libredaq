/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>
#include "ldaq_mod_ad7606.h"
#if HAVE_LDAQ_SLOT_BUS

void mod_ad7606_init(void)
{
	// CONVST: default=1
	pio_configure_pin(DAQ_CONVST_GPIO, PIO_OUTPUT_1);

	// RD: default=1
	pio_configure_pin(DAQ_nRD_GPIO, PIO_OUTPUT_1);

	// DAQ_BUSY: input
	pio_configure_pin(DAQ_BUSY_GPIO, PIO_INPUT | PIO_IT_FALL_EDGE);

	// DAQ_1STDATA: input
	pio_configure_pin(DAQ_1STDATA_GPIO, PIO_INPUT);
	
	 // --------
	// Configure sense interrupt:
	// A fall edge in DAQ_BUSY.
	//pio_configure_interrupt
	// PIO_IT_FALL_EDGE
	pio_handler_set(DAQ_BUSY_PIO, DAQ_BUSY_ID, 1<<DAQ_BUSY_IDX, PIO_IT_FALL_EDGE, adc_busy_edge_handler);
	pio_enable_interrupt(DAQ_BUSY_PIO,1<< DAQ_BUSY_IDX);
	NVIC_EnableIRQ(DAQ_BUSY_IRQn);
	 // --------
	
	// Send reset pulse: (min 50ns)
	// Already done in global reset of all slots.
}

#warning TODO in mod_ad7606_read_all(): Check 1stDATA line to avoid accidental channel shifts


#endif //HAVE_LDAQ_SLOT_BUS
