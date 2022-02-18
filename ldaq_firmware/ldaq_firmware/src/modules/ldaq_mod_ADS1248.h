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

// ADC module with chip ADS1248: 

/*
 ALL_RESET : -> /RESET
 CTRL0           : /CS
 CTRL1           : /ADC_DRDY
 CTRL2           :  ADC_START
 LDAQ_SLOTS_SCK  : SCK
 LDAQ_SLOTS_MOSI : DIN
 LDAQ_SLOTS_MISO : DOUT
*/

#define DAQ_nCS_GPIO         LDAQ_SLOT1_CTRL0_GPIO
#define DAQ_nCS_PIO          LDAQ_SLOT1_CTRL0_PIO
#define DAQ_nCS_IDX          LDAQ_SLOT1_CTRL0_IDX

#define DAQ_START_GPIO       LDAQ_SLOT1_CTRL2_GPIO
#define DAQ_START_PIO        LDAQ_SLOT1_CTRL2_PIO
#define DAQ_START_IDX        LDAQ_SLOT1_CTRL2_IDX

#define DAQ_nDRDY_GPIO        LDAQ_SLOT1_CTRL1_GPIO
#define DAQ_nDRDY_PIO         LDAQ_SLOT1_CTRL1_PIO
#define DAQ_nDRDY_IDX         LDAQ_SLOT1_CTRL1_IDX
#define DAQ_nDRDY_ID          LDAQ_SLOT1_CTRL1_ID
#define DAQ_nDRDY_IRQn        LDAQ_SLOT1_CTRL1_IRQn


/** Initialize the device */
void mod_ads1248_init(const uint16_t desired_gain,const uint16_t desired_rate);

/** Set PGA desired gain & ADC conversion rate.
  * The chip is programmed to work in the closest rate equal or above the one indicated by the user.
  */
void mod_ads1248_set_gain_and_rate(const uint16_t desired_gain,const uint16_t desired_rate);


/** Generate a convert start pulse */
static inline void mod_ads1248_convst(void)
{
	// Minimum: 3 * (1/4.096MHz) ~~ 750 ns
	LDAQ_SBI(DAQ_START);
	delay_us(1);
	LDAQ_CBI(DAQ_START);
}

/** Checks whether the ADC is busy during a conversion (returns!=0) or it has done (returns 0) */
static inline int mod_ads1248_check_busy(void) 
{
	return (DAQ_nDRDY_PIO->PIO_PDSR & (1<<DAQ_nDRDY_IDX));
}

uint8_t mod_ads1248_txrx_spi(uint8_t tx_word);

/** Changes the multiplexer in the ADC to sample the given differential analog inputs */
void mod_ads1248_set_mux(uint8_t channel_pos, uint8_t channel_neg);

/** Issues a single data read to the ADC, reading the 24 bits (3 bytes) for the last ADC converssion. */
void mod_ads1248_read_conv_result(uint8_t * out_result_uint24 );

/** Sends a WREG command to the chip */
void mod_ads1248_write_register(uint8_t reg_idx, uint8_t reg_value);

void task_ads1248_start_conversion(void);
void ads1248_busy_edge_handler(uint32_t a,uint32_t b);


#endif //HAVE_LDAQ_SLOT_BUS
