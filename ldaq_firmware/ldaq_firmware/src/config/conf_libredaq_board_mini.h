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
#define LED1_GPIO    PIO_PA23_IDX
#define LED2_GPIO    PIO_PA25_IDX

// Include all HW & SW to handle the parallel bus to pluggable slots: 
#define HAVE_LDAQ_SLOT_BUS     0  // No


