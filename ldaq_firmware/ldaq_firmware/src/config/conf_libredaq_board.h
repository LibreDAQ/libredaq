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

// ====================================================
//  LibreDAQ board configuration
// ====================================================
#define BOARD_FREQ_SLCK_XTAL      (32768UL)      // Slow xtal
#define BOARD_FREQ_SLCK_BYPASS    (32768UL)
#define BOARD_FREQ_MAINCK_XTAL    (12000000UL)   // Main xtal
#define BOARD_FREQ_MAINCK_BYPASS  (12000000UL)
#define BOARD_OSC_STARTUP_US      (15625UL)    // The board main clock xtal startup time has not been defined.
// ====================================================


// ====================================================
//                Peripherals
// ====================================================
#if defined(LDAQ_BOARD_BASE)
# include "conf_libredaq_board_base.h"
#elif defined(LDAQ_BOARD_MINI)
# include "conf_libredaq_board_mini.h"
#else
#	error **FATAL ERROR** You must define LDAQ_BOARD_{*} according to the target board model!
#endif

