/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include <comms/ldaq_process_cmds.h>
#include <comms/ldaq_frames.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <utils/ldaq_util_macros.h>

// -------------------------------------------
// CMD:
// -------------------------------------------
static void process_end_all_tasks(const uint8_t *buf)
{
	rt_sched_end_all_task();
}

// ======== Register command handler ========
LDAQ_INIT_BLOCK_START(GENERAL_CONTROL)
ldaq_comms_register_cmd_handler(FRAMECMD_STOP_ALL, &process_end_all_tasks);
LDAQ_INIT_BLOCK_END()

