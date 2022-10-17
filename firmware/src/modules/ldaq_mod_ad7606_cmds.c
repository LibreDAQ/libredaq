/*+-------------------------------------------------------------------------+
|                             LibreDAQ                                    |
|                                                                         |
| Copyright (C) 2015  Jose Luis Blanco Claraco                            |
| Distributed under GNU General Public License version 3                  |
|   See <http://www.gnu.org/licenses/>                                    |
+-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_adc_common.h"
#include "ldaq_mod_ad7606.h"
#include <comms/ldaq_process_cmds.h>
#include <comms/ldaq_frames.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <utils/ldaq_util_macros.h>
#if HAVE_LDAQ_SLOT_BUS

void adc_busy_edge_handler(uint32_t a,uint32_t b)
{
	irqflags_t flags = ldaq_enter_cs();
	struct TFrame_LDAQDATA_ADC_16bx8 * trg = &frame_adc[frame_adc_buffer_idx][frame_adc_next_idx];
	trg->time = adc_is_converting_time;
	adc_is_converting_time=0;
	mod_ad7606_read_all(trg->adcs);
	ldaq_leave_cs(flags);

	#warning TODO: Put real slot index!!
	trg->src_slot = 0;
	
	++frame_adc_next_idx;
	frame_adc_next_idx &= ADC_FRAME_BLOCK_SIZE_MASK;
	if (!frame_adc_next_idx)
	{
		frame_adc_buffer_idx_done = frame_adc_buffer_idx;
		if (++frame_adc_buffer_idx>=ADC_FRAME_BUFFERS)
		frame_adc_buffer_idx=0;
	}
}

// Called from scheduler (GlobInterr=Disabled)
void task_ad7606_start_conversion(void)
{
	if (!adc_is_converting_time)
	{
		mod_ad7606_convst();
		adc_is_converting_time = rt_sched_now();
		//__NOP(); __NOP(); __NOP(); __NOP(); // minimum latency to make sure the BUSY pin is read correctly as BUSY
	}
}

// -------------------------------------------
// CMD: ADC start
// -------------------------------------------
static void process_adc_start(const uint8_t *buf)
{
	const struct TFrame_LDAQCMD_ADC_Start *cmd = (const struct TFrame_LDAQCMD_ADC_Start *)buf;

	#warning TODO: Move init to auto-detection phase
	mod_ad7606_init();
	
	//uint8_t task1 =
	rt_sched_register_idle_task(&task_adc_send_full_frame);
	//uint8_t task2 =
	rt_sched_register_task_periodic(&task_ad7606_start_conversion,LDAQ_SCHEDULER_FREQ_HZ / cmd->sampling_rate_hz);
}

// ======== Register command handler ========
LDAQ_INIT_BLOCK_START(MOD_AD7606)
ldaq_comms_register_cmd_handler(FRAMECMD_ADC_START, &process_adc_start);
LDAQ_INIT_BLOCK_END()


#endif //HAVE_LDAQ_SLOT_BUS
