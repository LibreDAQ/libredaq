/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

 /* This file is intended to contain definitions and configuration details for
 * features and devices that are available on the board, e.g., frequency and
 * startup time for an external crystal, external memory devices, LED and USART
 * pins.
 */
#pragma once

// On-board LEDs --------------------
#define LED1_GPIO    PIO_PA27_IDX
#define LED2_GPIO    PIO_PB13_IDX

// Include all HW & SW to handle the parallel bus to pluggable slots:
#define HAVE_LDAQ_SLOT_BUS     1  // Yes

// 8-bit PARALLEL BUS
#define LDAQ_PARALLEL_BUS_PIO  PIOA
#define LDAQ_PARALLEL_BUS_PINS  0x00ff
// Use to read the 8bit parallel bus pins
#define LDAQ_READ_PARALLEL_BUS  (LDAQ_PARALLEL_BUS_PIO->PIO_PDSR & LDAQ_PARALLEL_BUS_PINS)

// LDAQ_RESET (global to all slots) = PA12
#define LDAQ_ALL_SLOTS_RESET_GPIO  PIO_PA12_IDX

// I2C BUS for PnP detection of modules in SLOTs: SDA=PB21. SCL/CRTL4=PB22  
#define LDAQ_PNP_I2C_SDA_GPIO  PIO_PB21_IDX
#define LDAQ_PNP_I2C_SCL_GPIO  PIO_PB22_IDX
#define LDAQ_SLOTx_CTRL4_GPIO  PIO_PB22_IDX    // This control signal is shared among all slots.
#define LDAQ_SLOTx_CTRL4_PIO   PIOB
#define LDAQ_SLOTx_CTRL4_IDX   22

// Common programmable clock to all slots: PB24 / PCK1
#define LDAQ_SLOTS_PCLK_PMC_PCK   PMC_PCK_1
#define LDAQ_SLOTS_PCLK_GPIO      PIO_PB24_IDX
#define LDAQ_SLOTS_PCLK_PIO       PIOB
#define LDAQ_SLOTS_PCLK_IDX       24

// Common SPI bus to all slots: TXD2(PA22), RXD2(PA23), SCK2(PA25)
#define LDAQ_SLOTS_SPI_USART         USART2
#define LDAQ_SLOTS_SPI_ID_USART      ID_USART2
#define LDAQ_SLOTS_SPI_BAUDRATE      10000000u   // (CKL freq, in Hz)

#define LDAQ_SLOTS_SPI_TXD_PIO       PIOA
#define LDAQ_SLOTS_SPI_TXD_IDX       22
#define LDAQ_SLOTS_SPI_TXD_PERIP     PIO_PERIPH_A

#define LDAQ_SLOTS_SPI_RXD_PIO       PIOA
#define LDAQ_SLOTS_SPI_RXD_IDX       23
#define LDAQ_SLOTS_SPI_RXD_PERIP     PIO_PERIPH_A

#define LDAQ_SLOTS_SPI_SCK_PIO       PIOA
#define LDAQ_SLOTS_SPI_SCK_IDX       25
#define LDAQ_SLOTS_SPI_SCK_PERIP     PIO_PERIPH_B



// SLOT 0 --------------
// SLOT0_CTRL0 = PA30
// SLOT0_CTRL1 = PB3
// SLOT0_CTRL2 = PB4
// SLOT0_CTRL3 = PA13
#define LDAQ_SLOT0_CTRL0_PIO  PIOA
#define LDAQ_SLOT0_CTRL0_IDX  30
#define LDAQ_SLOT0_CTRL0_GPIO PIO_PA30_IDX

#define LDAQ_SLOT0_CTRL1_PIO  PIOB
#define LDAQ_SLOT0_CTRL1_IDX  3
#define LDAQ_SLOT0_CTRL1_GPIO PIO_PB3_IDX
#define LDAQ_SLOT0_CTRL1_ID   ID_PIOB
#define LDAQ_SLOT0_CTRL1_IRQn PIOB_IRQn

#define LDAQ_SLOT0_CTRL2_PIO  PIOB
#define LDAQ_SLOT0_CTRL2_IDX  4
#define LDAQ_SLOT0_CTRL2_GPIO PIO_PB4_IDX
#define LDAQ_SLOT0_CTRL2_ID   ID_PIOB
#define LDAQ_SLOT0_CTRL2_IRQn PIOB_IRQn

#define LDAQ_SLOT0_CTRL3_PIO  PIOA
#define LDAQ_SLOT0_CTRL3_IDX  13
#define LDAQ_SLOT0_CTRL3_GPIO PIO_PA13_IDX

// SLOT 1 --------------
// SLOT1_CTRL0 = PA14
// SLOT1_CTRL1 = PA15
// SLOT1_CTRL2 = PA16
// SLOT1_CTRL3 = PA17
#define LDAQ_SLOT1_CTRL0_PIO  PIOA
#define LDAQ_SLOT1_CTRL0_IDX  14
#define LDAQ_SLOT1_CTRL0_GPIO PIO_PA14_IDX

#define LDAQ_SLOT1_CTRL1_PIO  PIOA
#define LDAQ_SLOT1_CTRL1_IDX  15
#define LDAQ_SLOT1_CTRL1_GPIO PIO_PA15_IDX
#define LDAQ_SLOT1_CTRL1_ID   ID_PIOA
#define LDAQ_SLOT1_CTRL1_IRQn PIOA_IRQn

#define LDAQ_SLOT1_CTRL2_PIO  PIOA
#define LDAQ_SLOT1_CTRL2_IDX  16
#define LDAQ_SLOT1_CTRL2_GPIO PIO_PA16_IDX
#define LDAQ_SLOT1_CTRL2_ID   ID_PIOA
#define LDAQ_SLOT1_CTRL2_IRQn PIOA_IRQn

#define LDAQ_SLOT1_CTRL3_PIO  PIOA
#define LDAQ_SLOT1_CTRL3_IDX  17
#define LDAQ_SLOT1_CTRL3_GPIO PIO_PA17_IDX

// SLOT 2 --------------
// SLOT2_CTRL0 = PA8
// SLOT2_CTRL1 = PA9
// SLOT2_CTRL2 = PA10
// SLOT2_CTRL3 = PA11
#define LDAQ_SLOT2_CTRL0_PIO  PIOA
#define LDAQ_SLOT2_CTRL0_IDX  8
#define LDAQ_SLOT2_CTRL0_GPIO PIO_PA8_IDX

#define LDAQ_SLOT2_CTRL1_PIO  PIOA
#define LDAQ_SLOT2_CTRL1_IDX  9
#define LDAQ_SLOT2_CTRL1_GPIO PIO_PA9_IDX
#define LDAQ_SLOT2_CTRL1_ID   ID_PIOA
#define LDAQ_SLOT2_CTRL1_IRQn PIOA_IRQn

#define LDAQ_SLOT2_CTRL2_PIO  PIOA
#define LDAQ_SLOT2_CTRL2_IDX  10
#define LDAQ_SLOT2_CTRL2_GPIO PIO_PA10_IDX
#define LDAQ_SLOT2_CTRL2_ID   ID_PIOA
#define LDAQ_SLOT2_CTRL2_IRQn PIOA_IRQn

#define LDAQ_SLOT2_CTRL3_PIO  PIOA
#define LDAQ_SLOT2_CTRL3_IDX  11
#define LDAQ_SLOT2_CTRL3_GPIO PIO_PA11_IDX

// SLOT 3 --------------
// SLOT3_CTRL0 = PB11
// SLOT3_CTRL1 = PB2
// SLOT3_CTRL2 = PB1
// SLOT3_CTRL3 = PB0
#define LDAQ_SLOT3_CTRL0_PIO  PIOB
#define LDAQ_SLOT3_CTRL0_IDX  11
#define LDAQ_SLOT3_CTRL0_GPIO PIO_PB11_IDX

#define LDAQ_SLOT3_CTRL1_PIO  PIOB
#define LDAQ_SLOT3_CTRL1_IDX  2
#define LDAQ_SLOT3_CTRL1_GPIO PIO_PB2_IDX
#define LDAQ_SLOT3_CTRL1_ID   ID_PIOB
#define LDAQ_SLOT3_CTRL1_IRQn PIOB_IRQn

#define LDAQ_SLOT3_CTRL2_PIO  PIOB
#define LDAQ_SLOT3_CTRL2_IDX  1
#define LDAQ_SLOT3_CTRL2_GPIO PIO_PB1_IDX
#define LDAQ_SLOT3_CTRL2_ID   ID_PIOB
#define LDAQ_SLOT3_CTRL2_IRQn PIOB_IRQn

#define LDAQ_SLOT3_CTRL3_PIO  PIOB
#define LDAQ_SLOT3_CTRL3_IDX  0
#define LDAQ_SLOT3_CTRL3_GPIO PIO_PB0_IDX


