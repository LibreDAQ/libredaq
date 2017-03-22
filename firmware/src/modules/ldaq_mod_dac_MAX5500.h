/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <conf_libredaq_board.h>
#include <utils/ldaq_util_macros.h>
#if HAVE_LDAQ_SLOT_BUS

// DAC module for chip MAX5500

/*
 CTRL0           : /CS
 LDAQ_SLOTS_SCK  : SCK
 LDAQ_SLOTS_MOSI : DIN
*/

#define DAC_nCS_GPIO      LDAQ_SLOT3_CTRL0_GPIO
#define DAC_nCS_PIO       LDAQ_SLOT3_CTRL0_PIO
#define DAC_nCS_IDX       LDAQ_SLOT3_CTRL0_IDX


/** Initialize the device */
void mod_dac_max5500_init(void);

static inline void mod_dac_max5500_send_spi_word(uint16_t tx_word)
{
	// nCS -> 0
	LDAQ_CBI(DAC_nCS);  
	NOP_DELAY_2();  // Min. 40ns: nCS->0 to SCK
	
	// Send HiByte:
	while (!(LDAQ_SLOTS_SPI_USART->US_CSR & US_CSR_TXEMPTY)) {}
	LDAQ_SLOTS_SPI_USART->US_THR = US_THR_TXCHR( ((tx_word & 0xff00) >> 8) );
	NOP_DELAY_2();
	// Send LoByte:
	while (!(LDAQ_SLOTS_SPI_USART->US_CSR & US_CSR_TXEMPTY)) {}
	LDAQ_SLOTS_SPI_USART->US_THR = US_THR_TXCHR( (tx_word & 0x00ff) );
	NOP_DELAY_2();

	// Wait till end of SPI tx:
	while (!(LDAQ_SLOTS_SPI_USART->US_CSR & US_CSR_TXEMPTY)) {}
	//NOP_DELAY_4();

	// nCS -> 1
	LDAQ_SBI(DAC_nCS);
	NOP_DELAY_1();
}


/** Updates the analog output of one single DAC channel.  
  * \param dac_idx In range [0,3]
  * \param dac_value Value in range [0x000, 0xffff]
  * \note Execution time : ~3us
  */
void mod_dac_max5500_update_single_DAC(uint8_t dac_idx, uint16_t dac_value);

/** Updates the analog output of one single DAC channel.  
  * \param dac_values An array [0-3] of values in range [0x000, 0xffff]
  * \note Execution time: xx
  */
void mod_dac_max5500_update_all_DACs(const uint16_t *dac_values);
#endif //HAVE_LDAQ_SLOT_BUS
