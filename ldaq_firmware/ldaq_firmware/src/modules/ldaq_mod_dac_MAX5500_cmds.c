/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "modules/ldaq_mod_dac_MAX5500.h"
#include <comms/ldaq_process_cmds.h>
#include <comms/ldaq_frames.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <utils/ldaq_util_macros.h>
#if HAVE_LDAQ_SLOT_BUS

//uint16_t waveform_memory[4][1024];

uint16_t waveform_sine[40] = {
	2048, 2368, 2681, 2978, 3252, 3496, 3705, 3873, 3996, 4071, 4095, 4071, 
	3996, 3873, 3705, 3496, 3252, 2978, 2681, 2368, 2048, 1728, 1415, 1118, 
	844, 600, 391, 223, 100, 25, 0, 25, 100, 223, 391, 600, 844, 1118, 1415, 1728
};

static void test_generate_sine_waveform(void)
{
	uint16_t vals[4] = {0,0,0,0};
	int i1=0,i2=10;
	for (;;)
	{
		vals[0] = waveform_sine[i1++];  if (i1>=sizeof(waveform_sine)/sizeof(waveform_sine[0])) i1=0;
		vals[1] = waveform_sine[i2++];  if (i2>=sizeof(waveform_sine)/sizeof(waveform_sine[0])) i2=0;
		
		mod_dac_max5500_update_all_DACs(vals);
		delay_us(10);
		//delay_s(1);
	}
	
}


// -------------------------------------------
// CMD: DAC output
// -------------------------------------------
static void process_dac_set_values(const uint8_t *buf)
{
	const struct TFrame_LDAQCMD_DAC_SetValues *cmd = (const struct TFrame_LDAQCMD_DAC_SetValues *)buf;

#warning TODO: Move init to auto-detection phase
	mod_dac_max5500_init();
	
	mod_dac_max5500_update_all_DACs(cmd->dac_values);
}


// ======== Register command handler ========
LDAQ_INIT_BLOCK_START(MOD_MAX5500)
ldaq_comms_register_cmd_handler(FRAMECMD_DAC_SET_VALUES, &process_dac_set_values);
LDAQ_INIT_BLOCK_END()


#endif //HAVE_LDAQ_SLOT_BUS
