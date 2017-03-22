/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_mod_HCTL2032.h"
#if HAVE_LDAQ_SLOT_BUS

void mod_hctl2032_init(void)
{
	// ADDR*: tri-state until we do a real read op.
	pio_configure_pin(HCTL2032_ADDR0_GPIO, PIO_OUTPUT_1);
	pio_configure_pin(HCTL2032_ADDR1_GPIO, PIO_OUTPUT_1);
	pio_configure_pin(HCTL2032_ADDR2_GPIO, PIO_OUTPUT_1);

	// CS's: all unselected
	pio_configure_pin(HCTL2032_nCS1_GPIO, PIO_OUTPUT_1);
	pio_configure_pin(HCTL2032_nCS2_GPIO, PIO_OUTPUT_1);
	
	// Send reset pulse: (min 50ns)
	// Already done in global reset of all slots.
}


#endif //HAVE_LDAQ_SLOT_BUS
