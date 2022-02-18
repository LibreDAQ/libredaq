/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

/** This function is meant to contain board-specific initialization code
* for, e.g., the I/O pins. The initialization can rely on application-
* specific board configuration, found in conf_board.h.
*/
void board_init(void)
{
#warning TODO: Add all initialization of IO for LibreDAQ

	// Enable PIO clock: (seems to be needed to READ pins)
	pmc_enable_periph_clk(ID_PIOA);
	pmc_enable_periph_clk(ID_PIOB);

	// LEDS:
	gpio_configure_pin(LED1_GPIO, PIO_OUTPUT_1);
	gpio_configure_pin(LED2_GPIO, PIO_OUTPUT_1);
	
	
	// ---------------------------------------------
	//  LDAQ Bus: Only for LibreDAQ Base model
	// ---------------------------------------------
#if HAVE_LDAQ_SLOT_BUS
	// Bus data:
	gpio_configure_group(LDAQ_PARALLEL_BUS_PIO, LDAQ_PARALLEL_BUS_PINS, PIO_INPUT | PIO_PULLUP);
	
	// Plug-n-play I2C bus:
	gpio_configure_pin(LDAQ_SLOTx_CTRL4_GPIO, PIO_OUTPUT_1);
	
	// Start SLOTS common bus clock:
	pio_set_peripheral(LDAQ_SLOTS_PCLK_PIO,PIO_PERIPH_B,(1u << LDAQ_SLOTS_PCLK_IDX));
	pmc_disable_pck(LDAQ_SLOTS_PCLK_PMC_PCK);
	pmc_switch_pck_to_pllack(LDAQ_SLOTS_PCLK_PMC_PCK,PMC_PCK_PRES_CLK_8);  // PMC_PCK_PRES_CLK_8= 24 MHz
//	pmc_switch_pck_to_sclk(LDAQ_SLOTS_PCLK_PMC_PCK,PMC_PCK_PRES_CLK_64);  // Slow clock, for testing
	pmc_enable_pck(LDAQ_SLOTS_PCLK_PMC_PCK);

	// Init SPI bus common to all slots:
	pio_set_peripheral(LDAQ_SLOTS_SPI_TXD_PIO,LDAQ_SLOTS_SPI_TXD_PERIP,(1u << LDAQ_SLOTS_SPI_TXD_IDX));
	pio_set_peripheral(LDAQ_SLOTS_SPI_RXD_PIO,LDAQ_SLOTS_SPI_RXD_PERIP,(1u << LDAQ_SLOTS_SPI_RXD_IDX));
	pio_set_peripheral(LDAQ_SLOTS_SPI_SCK_PIO,LDAQ_SLOTS_SPI_SCK_PERIP,(1u << LDAQ_SLOTS_SPI_SCK_IDX));
	
	// Generate reset pulse:     __---__
	gpio_configure_pin(LDAQ_ALL_SLOTS_RESET_GPIO, PIO_OUTPUT_0); 
	delay_ms(1);
	pio_set_pin_high(LDAQ_ALL_SLOTS_RESET_GPIO);
	delay_ms(1);
	pio_set_pin_low(LDAQ_ALL_SLOTS_RESET_GPIO);
	// Give time slowest modules to recover from the global reset:
	delay_ms(3);
	
#endif // HAVE_LDAQ_SLOT_BUS
	
	
}
