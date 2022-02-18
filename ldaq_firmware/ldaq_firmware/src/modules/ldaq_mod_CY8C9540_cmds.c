/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_mod_CY8C9540.h"
#include <comms/ldaq_process_cmds.h>
#include <comms/ldaq_frames.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <utils/ldaq_util_macros.h>
#if HAVE_LDAQ_SLOT_BUS

// -------------------------------------------
// CMD: AMP-ADC start
// -------------------------------------------
static void process_amp_adc_start(const uint8_t *buf)
{
	//const struct TFrame_LDAQCMD_ADC_AMP_Start *cmd = (const struct TFrame_LDAQCMD_ADC_AMP_Start *)buf;

	#warning TODO: Move init to auto-detection phase
	//mod_ads1248_init(cmd->desired_gain, cmd->sampling_rate_hz);
	

}


// ======== Register command handler ========
LDAQ_INIT_BLOCK_START(MOD_CY8C9540)
ldaq_comms_register_cmd_handler(FRAMECMD_ADC_AMP_START, &process_amp_adc_start);
LDAQ_INIT_BLOCK_END()


#endif //HAVE_LDAQ_SLOT_BUS
