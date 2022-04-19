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

// ADC module with chip AD7606

/*
 ALL_RESET : DAQ_RESET
 CTRL0: DAQ_CONVST
 CTRL1: /DAQ_RD
 CTRL2: DAQ_BUSY
 CTRL3: DAQ_1STDATA
*/

#define DAQ_CONVST_GPIO      LDAQ_SLOT0_CTRL0_GPIO
#define DAQ_CONVST_PIO       LDAQ_SLOT0_CTRL0_PIO
#define DAQ_CONVST_IDX       LDAQ_SLOT0_CTRL0_IDX

#define DAQ_nRD_GPIO         LDAQ_SLOT0_CTRL1_GPIO
#define DAQ_nRD_PIO          LDAQ_SLOT0_CTRL1_PIO
#define DAQ_nRD_IDX          LDAQ_SLOT0_CTRL1_IDX

#define DAQ_BUSY_GPIO        LDAQ_SLOT0_CTRL2_GPIO
#define DAQ_BUSY_PIO         LDAQ_SLOT0_CTRL2_PIO
#define DAQ_BUSY_IDX         LDAQ_SLOT0_CTRL2_IDX
#define DAQ_BUSY_ID          LDAQ_SLOT0_CTRL2_ID
#define DAQ_BUSY_IRQn        LDAQ_SLOT0_CTRL2_IRQn

#define DAQ_1STDATA_GPIO     LDAQ_SLOT0_CTRL3_GPIO


/** Initialize the device */
void mod_ad7606_init(void);

/** Generate a convert start pulse */
static inline void mod_ad7606_convst(void)
{
	DAQ_CONVST_PIO->PIO_CODR = 1 << (DAQ_CONVST_IDX); // pio_set_pin_low(DAQ_CONVST_GPIO);
	__NOP(); __NOP(); __NOP(); // minimum low pulse: 25 ns
	DAQ_CONVST_PIO->PIO_SODR = 1 << (DAQ_CONVST_IDX); // pio_set_pin_high(DAQ_CONVST_GPIO);
}

/** Busy loops until the ADC gets out of "busy" (converting) */
static inline void mod_ad7606_wait_busy(void)
{
	__NOP(); __NOP();  __NOP();  // Leave time to the BUSY rise edge to propagate to PDSR
	while ( DAQ_BUSY_PIO->PIO_PDSR & (1<<DAQ_BUSY_IDX) )
	{
	}
}

/** Checks whether the ADC is busy during a conversion (returns!=0) or it has done (returns 0) */
static inline int mod_ad7606_check_busy(void) 
{
	return (DAQ_BUSY_PIO->PIO_PDSR & (1<<DAQ_BUSY_IDX));
}


/** Read all 8 channels. Output buffer must be [2*8] bytes long */
static inline void mod_ad7606_read_all(int16_t *buf)
{
	// Store int16_t as little endian:
	for (uint8_t i=0;i<8;i++)
	{
		// /RD low pulse: should be ~25ns (+ delay in propagation until PIO_PDSR)
		LDAQ_CBI(DAQ_nRD);  // nRD=0
		NOP_DELAY_4();
		const uint8_t bh = LDAQ_READ_PARALLEL_BUS;
		LDAQ_SBI(DAQ_nRD);  // nRD=1
		NOP_DELAY_2();
		LDAQ_CBI(DAQ_nRD);  // nRD=0
		NOP_DELAY_4();
		const uint8_t bl = LDAQ_READ_PARALLEL_BUS;
		LDAQ_SBI(DAQ_nRD);  // nRD=1
		*buf++ = (((uint16_t)bl) << 8) | bh;
	}
}

void task_ad7606_start_conversion(void);
void adc_busy_edge_handler(uint32_t a,uint32_t b);

#endif //HAVE_LDAQ_SLOT_BUS

