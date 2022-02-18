/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <conf_libredaq_board.h>
#if HAVE_LDAQ_SLOT_BUS

#include <utils/ldaq_util_macros.h>

// hctl2032, 4x encoders module

/*
 ALL_RESET : -> inverter -> /RSTX, /RSTY in each chip.
 SLOTS_PNP_SCL: ENC_ADDR0  (=SEL1)
 CTRL0        : ENC_ADDR1  (=SEL2)
 CTRL1        : ENC_ADDR2  (= X/nY)
 CTRL2        : /ENC1_CS
 CTRL3        : /ENC2_CS
*/

#define HCTL2032_ADDR0_GPIO      LDAQ_SLOTx_CTRL4_GPIO
#define HCTL2032_ADDR0_PIO       LDAQ_SLOTx_CTRL4_PIO
#define HCTL2032_ADDR0_IDX       LDAQ_SLOTx_CTRL4_IDX

#define HCTL2032_ADDR1_GPIO      LDAQ_SLOT2_CTRL0_GPIO
#define HCTL2032_ADDR1_PIO       LDAQ_SLOT2_CTRL0_PIO
#define HCTL2032_ADDR1_IDX       LDAQ_SLOT2_CTRL0_IDX

#define HCTL2032_ADDR2_GPIO      LDAQ_SLOT2_CTRL1_GPIO
#define HCTL2032_ADDR2_PIO       LDAQ_SLOT2_CTRL1_PIO
#define HCTL2032_ADDR2_IDX       LDAQ_SLOT2_CTRL1_IDX

#define HCTL2032_nCS1_GPIO       LDAQ_SLOT2_CTRL2_GPIO
#define HCTL2032_nCS1_PIO        LDAQ_SLOT2_CTRL2_PIO
#define HCTL2032_nCS1_IDX        LDAQ_SLOT2_CTRL2_IDX

#define HCTL2032_nCS2_GPIO       LDAQ_SLOT2_CTRL3_GPIO
#define HCTL2032_nCS2_PIO        LDAQ_SLOT2_CTRL3_PIO
#define HCTL2032_nCS2_IDX        LDAQ_SLOT2_CTRL3_IDX

/** Initialize the device */
void mod_hctl2032_init(void);

/** Safe read data bus, following chip manufacturer datasheet recommendations... */
static inline uint8_t internal_mod_hctl2032_read_stable_bus(void)
{
#if 0
	return LDAQ_READ_PARALLEL_BUS;
#else
	uint8_t b1,b2; 
	do {
		b1 = LDAQ_READ_PARALLEL_BUS;
		NOP_DELAY_3();
		b2 = LDAQ_READ_PARALLEL_BUS;
	} while (b1!=b2);
	return b1;
#endif
}

/** Read one channel of one chip (X/Y): 4 bytes reads. We must read 4 bytes max at a time because of the inhibit logic of the chip! */
static inline void internal_mod_hctl2032_read_one_channel(uint32_t **buf, Pio *OE_PIO, const int OE_PIN_IDX)
{
	// Important: the bytes must be read in this order to activate the internal chip inhibit logic! See datasheet.
	
	// MSB. Addr=01
	LDAQ_CBI(HCTL2032_ADDR0);  // SEL1=0, SEL2=1
	LDAQ_SBI(HCTL2032_ADDR1);
	OE_PIO->PIO_CODR = 1 << OE_PIN_IDX;  // nOE=0  (*after* setting the address above)
	NOP_DELAY_10();
	const uint32_t b1 = internal_mod_hctl2032_read_stable_bus();
	// 2nd byte. Addr=11
	LDAQ_SBI(HCTL2032_ADDR0);  // SEL1=1, SEL2=1
	NOP_DELAY_10();
	const uint32_t b2 = internal_mod_hctl2032_read_stable_bus();
	// 3rd byte. Addr=00
	LDAQ_CBI(HCTL2032_ADDR0);  // SEL1=0, SEL2=0
	LDAQ_CBI(HCTL2032_ADDR1);
	NOP_DELAY_10();
	const uint32_t b3 = internal_mod_hctl2032_read_stable_bus();
	// LSB. Addr=10
	LDAQ_SBI(HCTL2032_ADDR0);   // SEL1=1, SEL2=0
	NOP_DELAY_10();
	const uint32_t b4 = internal_mod_hctl2032_read_stable_bus();

	OE_PIO->PIO_SODR = 1 << OE_PIN_IDX;  // nOE=1
	
	// Assemble data words in little endian:
	const uint32_t READ_TICKS_A = b4 | (b3<<8) | (b2<<16) | (b1<<24);
	*(*buf)++ = READ_TICKS_A;
}


/** Read all 4 channels. Output buffer must be an array of 4 x 32bit ints 
  * \note Execution time: 6.97us @ 96MHz
  */
static inline void mod_hctl2032_read_all(uint32_t *buf)
{
	// Bus: remove pull-up:
	gpio_configure_group(LDAQ_PARALLEL_BUS_PIO, LDAQ_PARALLEL_BUS_PINS, PIO_INPUT | PIO_DEGLITCH);
	
	// Chip 1/2
	// Channel X:
	LDAQ_CBI(HCTL2032_ADDR2); 
	internal_mod_hctl2032_read_one_channel(&buf, HCTL2032_nCS1_PIO, HCTL2032_nCS1_IDX );
	NOP_DELAY_6();
	// Channel Y:
	LDAQ_SBI(HCTL2032_ADDR2);
	internal_mod_hctl2032_read_one_channel(&buf, HCTL2032_nCS1_PIO, HCTL2032_nCS1_IDX );
	NOP_DELAY_2();
	
	// Chip 2/2
	// Channel X:
	LDAQ_CBI(HCTL2032_ADDR2);
	internal_mod_hctl2032_read_one_channel(&buf, HCTL2032_nCS2_PIO, HCTL2032_nCS2_IDX );
	NOP_DELAY_6();
	// Channel Y:
	LDAQ_SBI(HCTL2032_ADDR2);
	internal_mod_hctl2032_read_one_channel(&buf, HCTL2032_nCS2_PIO, HCTL2032_nCS2_IDX );

	// Bus: restore pull-up:
	gpio_configure_group(LDAQ_PARALLEL_BUS_PIO, LDAQ_PARALLEL_BUS_PINS, PIO_INPUT | PIO_PULLUP);
}

#endif //HAVE_LDAQ_SLOT_BUS
