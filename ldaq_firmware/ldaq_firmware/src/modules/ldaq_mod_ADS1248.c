/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_mod_ADS1248.h"
#if HAVE_LDAQ_SLOT_BUS

/* SPI frames for the ADS1248 chip
           xx bits
     MSB --- LSB (MSB first)
|A1 A0  |  C1 C0 | D11 ... D0 |

Commands:
- POLARITY: CPOL=0 (inactive SCK=low level)
- PHASE: CPHA=0 (master changes data on the RISING edge of clock)

Timing: 
 - SCK freq: Max = 2.049 MHz. Min to assure stability at 2000SPS = 1.923 Mhz  ==> Use 2.0 Mhz
 - last SCK to nCS->rise: MIN 1.71 us !!
 - nCS rise pulse: MIN 1.22 us
 - START high pulse: MIN 0.73 us
 - Internal FIR filter: Not used if we read with START pulses (it only works with START=1)
 
SPI registers: 
 - addr=0x00 (MUX0)
 
*/

// Addresses of each chip SPI register:
#define ADS1248_REG_MUX0          0x00
#define ADS1248_REG_VBIAS         0x01
#define ADS1248_REG_MUX1          0x02
#define ADS1248_REG_SYS0          0x03
#define ADS1248_REG_IDAC0         0x0a
#define ADS1248_REG_IDAC1         0x0b

// SPI commands:
#define ADS1248_SPICMD_SYNC       0x04
#define ADS1248_SPICMD_RESET      0x06
#define ADS1248_SPICMD_NOP        0xff
#define ADS1248_SPICMD_RDATA_ONCE 0x12
#define ADS1248_SPICMD_RDATA_CONT 0x14
#define ADS1248_SPICMD_SDATA_CONT 0x16
#define ADS1248_SPICMD_READ_REG(_reg_idx)   (0x20 | (_reg_idx & 0x0f))  // RREG
#define ADS1248_SPICMD_WRITE_REG(_reg_idx)  (0x40 | (_reg_idx & 0x0f))  // WREG


void mod_ads1248_init(const uint16_t desired_gain,const uint16_t desired_rate)
{
	// CONVST: default=1
	pio_configure_pin(DAQ_nCS_GPIO, PIO_OUTPUT_1);
	pio_configure_pin(DAQ_START_GPIO, PIO_OUTPUT_0);
	pio_configure_pin(DAQ_nDRDY_GPIO, PIO_INPUT | PIO_PULLUP);

	// Initialize USART in SPI Master Mode.
	static usart_spi_opt_t USART_SPI_OPTIONS =
	{
		.baudrate     = min( 1800000 ,LDAQ_SLOTS_SPI_BAUDRATE),
		.char_length   = US_MR_CHRL_8_BIT,
		.spi_mode      = SPI_MODE_1,  // SPI_MODE_1: CPHA=1, CPOL=0
		.channel_mode  = US_MR_CHMODE_NORMAL
	};
	sysclk_enable_peripheral_clock(LDAQ_SLOTS_SPI_ID_USART);
	usart_init_spi_master(LDAQ_SLOTS_SPI_USART, &USART_SPI_OPTIONS, sysclk_get_main_hz());
	usart_enable_tx(LDAQ_SLOTS_SPI_USART);
	usart_enable_rx(LDAQ_SLOTS_SPI_USART);


	// --------
	// Configure sense interrupt:
	// A fall edge in DAQ_nDRDY.
	//pio_configure_interrupt
	// PIO_IT_FALL_EDGE
	pio_handler_set(DAQ_nDRDY_PIO, DAQ_nDRDY_ID, 1<<DAQ_nDRDY_IDX, PIO_IT_FALL_EDGE, ads1248_busy_edge_handler);
	pio_enable_interrupt(DAQ_nDRDY_PIO,1<< DAQ_nDRDY_IDX);
	NVIC_EnableIRQ(DAQ_nDRDY_IRQn);
	 // --------
	
	// Send reset pulse: (min 50ns)
	// Already done in global reset of all slots.
	
	// Set up reference voltage:
	// Register MUX1 (addr=0x02)
	//  - Int. ref is always ON
	//  - REF0: onboard ref selected
	mod_ads1248_write_register(ADS1248_REG_MUX1, 0b00111000);
//#warning TODO: Remove, just for testing!
//	mod_ads1248_write_register(ADS1248_REG_MUX1, 0b00111110);  // Test mode

	mod_ads1248_set_gain_and_rate(desired_gain,desired_rate);
}

/** Set PGA desired gain & ADC conversion rate */
void mod_ads1248_set_gain_and_rate(const uint16_t desired_gain,const uint16_t desired_rate)
{	
	// SYS0 register:
	uint8_t pga_val= 0;
	switch (desired_gain) {
		case 1:   pga_val = 0; break;
		case 2:   pga_val = 1; break;
		case 4:   pga_val = 2; break;
		case 8:   pga_val = 3; break;
		case 16:  pga_val = 4; break;
		case 32:  pga_val = 5; break;
		case 64:  pga_val = 6; break;
		case 128: pga_val = 7; break;
	}
	uint8_t dor_val= 0;
	if (desired_rate>1000)      dor_val = 9; // 2000 SPS
	else if (desired_rate>640)  dor_val = 8; // 1000 SPS
	else if (desired_rate>320)  dor_val = 7; // 640  SPS
	else if (desired_rate>160)  dor_val = 6; // 320  SPS
	else if (desired_rate>80)   dor_val = 5; // 160  SPS
	else if (desired_rate>40)   dor_val = 4; // 80   SPS
	else if (desired_rate>20)   dor_val = 3; // 40   SPS
	else if (desired_rate>10)   dor_val = 2; // 20   SPS
	else if (desired_rate>5)    dor_val = 1; // 10   SPS
	else                        dor_val = 0; // 5    SPS

	mod_ads1248_write_register(ADS1248_REG_SYS0, (pga_val << 4) | dor_val );  // Test mode: measure mid vcc

}

void mod_ads1248_write_register(uint8_t reg_idx, uint8_t reg_value)
{
	// nCS -> 0
	LDAQ_CBI(DAQ_nCS);
	NOP_DELAY_2();  // Min. 10ns: nCS->0 to SCK
	
	// Send WREG cmd:
	mod_ads1248_txrx_spi( ADS1248_SPICMD_WRITE_REG(reg_idx) );
	const uint8_t num_bytes_to_write = 1;
	mod_ads1248_txrx_spi( (num_bytes_to_write-1) & 0x0f );

	// Send data:
	mod_ads1248_txrx_spi( reg_value );


	delay_us(2);       // last SCK to nCS->rise: MIN 1.71 us !!
	LDAQ_SBI(DAQ_nCS); // nCS -> 1
	delay_us(2);       // nCS rise pulse: MIN 1.22 us
}

/** Changes the multiplexer in the ADC to sample the given differential analog inputs */
void mod_ads1248_set_mux(uint8_t channel_pos, uint8_t channel_neg)
{
	mod_ads1248_write_register(ADS1248_REG_MUX0,  
		(0 << 6)  | // sensor detect current source
		((channel_pos & 0x07) << 3 ) | 
		((channel_neg & 0x07) << 0 )
		);
}


/** Issues a single data read to the ADC, reading the 24 bits (3 bytes) for the last ADC converssion. */
void mod_ads1248_read_conv_result(uint8_t * out_result_uint24 )
{
	// nCS -> 0
	LDAQ_CBI(DAQ_nCS);
	NOP_DELAY_2();  // Min. 10ns: nCS->0 to SCK
	
	// Send WREG cmd:
	mod_ads1248_txrx_spi( ADS1248_SPICMD_RDATA_ONCE );

	// Read:
	uint8_t bMSB = mod_ads1248_txrx_spi( ADS1248_SPICMD_NOP );
	uint8_t bMid = mod_ads1248_txrx_spi( ADS1248_SPICMD_NOP );
	uint8_t bLSB = mod_ads1248_txrx_spi( ADS1248_SPICMD_NOP );

	delay_us(2);       // last SCK to nCS->rise: MIN 1.71 us !!
	LDAQ_SBI(DAQ_nCS); // nCS -> 1
	delay_us(2);       // nCS rise pulse: MIN 1.22 us
	
	*out_result_uint24++ = bLSB;
	*out_result_uint24++ = bMid;
	*out_result_uint24++ = bMSB;
}

uint8_t mod_ads1248_txrx_spi(uint8_t tx_word)
{
	// Send byte:
	while (!(LDAQ_SLOTS_SPI_USART->US_CSR & US_CSR_TXEMPTY)) {}
	LDAQ_SLOTS_SPI_USART->US_THR = US_THR_TXCHR( tx_word );
	NOP_DELAY_3();
	// Wait till end of SPI tx:
	while (!(LDAQ_SLOTS_SPI_USART->US_CSR & US_CSR_TXEMPTY)) {}
	NOP_DELAY_3();
	while (!(LDAQ_SLOTS_SPI_USART->US_CSR & US_CSR_RXRDY)) {}
	uint8_t ret = LDAQ_SLOTS_SPI_USART->US_RHR;
	return ret;
}


#endif //HAVE_LDAQ_SLOT_BUS
