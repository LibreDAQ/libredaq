/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <conf_libredaq_firmware.h>
#include <firmware_modes/ldaq_firmware_modes.h>
#include <system/ldaq_rt_scheduler.h>
#include <utils/ldaq_util_macros.h>

// ------------------------------------------------------
//            FIRMWARE MODE: FIRM_MODE_NORMAL
// ------------------------------------------------------
static void firmware_normal_main_loop(void)
{
	rt_sched_run_idle_tasks();
}
static void firmware_normal_on_enter(void)
{
}
static void firmware_normal_on_leave(void)
{
}

// ======== Register firmware mode ========
LDAQ_INIT_BLOCK_START(REGFRMMODE_NORMAL)
	ldaq_register_firmware_mode(FIRM_MODE_NORMAL, &firmware_normal_main_loop, &firmware_normal_on_enter,&firmware_normal_on_leave );
LDAQ_INIT_BLOCK_END()

