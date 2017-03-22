/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

// ====================================================
//  LibreDAQ firmware parameters: GENERAL
// ====================================================
#define LDAQ_SCHEDULER_FREQ_HZ   (50 * 1000)

#define LDAQ_SCHEDULER_REALTIME_MAX_IDLE_TASKS      4
#define LDAQ_SCHEDULER_REALTIME_MAX_FIXED_TASKS     4
#define LDAQ_SCHEDULER_REALTIME_MAX_PERIODIC_TASKS  4


// Maximum time after the last byte received as part of an incomplete 
// frame before discarding the input buffer.
#define TIMEOUT_WITHIN_RX_COMMANDS_BYTES_SEC         5  // (seconds)

// ====================================================


// LibreDAQ: Important to achieve real real-time performance: 
// Assign a low (reduced) priority to USB interrupts, so timer-based interrupts have priority. 
// (ARM priority levels: 0=highest, 15=lowest)
#define LDAQ_INT_PRIORITY_LEVEL_USB              14
#define LDAQ_INT_PRIORITY_LEVEL_SYSTICK          1
#define LDAQ_INT_PRIORITY_LEVEL_SAMPLING_TIMERS  0

#define LDAQ_INT_PRIORITY_BASE_CRITICAL_SECTIONS    8

