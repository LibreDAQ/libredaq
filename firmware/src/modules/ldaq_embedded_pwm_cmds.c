/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_embedded_pwm.h"
#include <comms/ldaq_process_cmds.h>
#include <comms/ldaq_frames.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <utils/ldaq_util_macros.h>

// -------------------------------------------
// CMD: PWM set duty cycle
// -------------------------------------------
static void process_embedded_pwm_init(const uint8_t *buf)
{
//	const struct TFrame_LDAQCMD_ADC_Start *cmd = (const struct TFrame_LDAQCMD_ADC_Start *)buf;
	embedded_pwm_init();
	#warning TODO: Continue!!
}

// ======== Register command handler ========
LDAQ_INIT_BLOCK_START(MOD_EMBED_PWM)
ldaq_comms_register_cmd_handler(FRAMECMD_PWM_INIT, &process_embedded_pwm_init);
LDAQ_INIT_BLOCK_END()

