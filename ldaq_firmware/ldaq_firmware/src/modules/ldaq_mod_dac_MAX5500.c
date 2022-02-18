/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "modules/ldaq_mod_dac_MAX5500.h"
#if HAVE_LDAQ_SLOT_BUS

/* SPI frames for the MAX5500 chip
           16 bits
     MSB --- LSB (MSB first)
|A1 A0  |  C1 C0 | D11 ... D0 |

Commands C1.C0:
 * 0 1: Load DAC, do NOT update DAC output.
 * 1 1: Load DAC, update ALL DAC outputs.
 
- POLARITY: CPOL=0 (inactive SCK=low level)
- PHASE: CPHA=1 (master changes data on the falling edge of clock)
 
*/

void mod_dac_max5500_init(void)
{
	// nCS: default=1
	pio_configure_pin(DAC_nCS_GPIO, PIO_OUTPUT_1);

	// Initialize USART in SPI Master Mode.
	static usart_spi_opt_t USART_SPI_OPTIONS =
	{
		.baudrate     = LDAQ_SLOTS_SPI_BAUDRATE,
		.char_length   = US_MR_CHRL_8_BIT,
		.spi_mode      = SPI_MODE_0,  // SPI_MODE_0: CPHA=1, CPOL=0
		.channel_mode  = US_MR_CHMODE_NORMAL
	};
	sysclk_enable_peripheral_clock(LDAQ_SLOTS_SPI_ID_USART);
	usart_init_spi_master(LDAQ_SLOTS_SPI_USART, &USART_SPI_OPTIONS, sysclk_get_main_hz());
	usart_enable_tx(LDAQ_SLOTS_SPI_USART);
	usart_enable_rx(LDAQ_SLOTS_SPI_USART);


	// Set all chip outputs to 0V
	//mod_dac_max5500_send_spi_word(0x8000);
}

void mod_dac_max5500_update_single_DAC(uint8_t dac_idx, uint16_t dac_value)
{
	// See word format at the top of this file
	const uint16_t tx_word =
	(((uint16_t)dac_idx) << 14) |
	(((uint16_t)0x03)    << 12) |
	(dac_value & 0x0fff);
	
	mod_dac_max5500_send_spi_word(tx_word);
}

void mod_dac_max5500_update_all_DACs(const uint16_t *dac_values)
{
	for (int i=0;i<4;i++)
	{
		// See word format at the top of this file
		const uint16_t tx_word =
		(((uint16_t)(i & 0x03)) << 14) |
		(((uint16_t) (i==3 ? 0x03 : 0x01))    << 12) |
		(dac_values[i] & 0x0fff);

		mod_dac_max5500_send_spi_word(tx_word);
	}
}


#endif //HAVE_LDAQ_SLOT_BUS
