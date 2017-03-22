/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_mod_HCTL2032.h"
#include <comms/ldaq_process_cmds.h>
#include <comms/ldaq_frames.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <utils/ldaq_util_macros.h>
#if HAVE_LDAQ_SLOT_BUS


// -------------------------------------------
// AUX FOR CMD: ENC start
// -------------------------------------------
#define ENC_FRAME_BUFFERS          (4)
#define ENC_FRAME_BLOCK_SIZE       (1<<5)
#define ENC_FRAME_BLOCK_SIZE_MASK  (ENC_FRAME_BLOCK_SIZE-1)
#pragma pack(push, 1) // exact fit - no padding
struct TFrame_LDAQDATA_ENC_32bx4 frame_enc[ENC_FRAME_BUFFERS][ENC_FRAME_BLOCK_SIZE];
#pragma pack(pop)

LDAQ_INIT_BLOCK_START(INITIALIZE_frame_enc_buffers)
for (int k=0;k<ENC_FRAME_BUFFERS;k++)
	for (int i=0;i<ENC_FRAME_BLOCK_SIZE;i++)
		TFrame_LDAQDATA_ENC_32bx4_init(&frame_enc[k][i]);
LDAQ_INIT_BLOCK_END()


volatile int frame_enc_buffer_idx = 0;
volatile int frame_enc_next_idx = 0;
volatile int frame_enc_buffer_idx_done = -1; // Points to a just filled-in block of data ready to be sent.

static void TASK_ENC_READ_ALL(void)
{
	{
		irqflags_t flags = ldaq_enter_cs();
		struct TFrame_LDAQDATA_ENC_32bx4 *trg = &frame_enc[frame_enc_buffer_idx][frame_enc_next_idx];
		trg->time = rt_sched_now();
		mod_hctl2032_read_all(trg->tickpos);
		++frame_enc_next_idx;
		frame_enc_next_idx &= ENC_FRAME_BLOCK_SIZE_MASK;
		ldaq_leave_cs(flags);
		
#warning TODO: Put real slot number!
		trg->src_slot = 0;
	}

	if (!frame_enc_next_idx)
	{
		frame_enc_buffer_idx_done = frame_enc_buffer_idx;
		if (++frame_enc_buffer_idx>=ENC_FRAME_BUFFERS)
			frame_enc_buffer_idx=0;
	}
}

static void task_encoder_send_full_frames(void)
{
	if (frame_enc_buffer_idx_done>=0)
	{
		ldaq_comms_enque_tx_frame(&frame_enc[frame_enc_buffer_idx_done][0],sizeof(frame_enc[0][0])*ENC_FRAME_BLOCK_SIZE);
		frame_enc_buffer_idx_done=-1;
	}
	
}


// -------------------------------------------
// CMD: ENC start
// -------------------------------------------
static void process_enc_start(const uint8_t *buf)
{
	const struct TFrame_LDAQCMD_ENC_Start *cmd = (const struct TFrame_LDAQCMD_ENC_Start *)buf;
	
	#warning TODO: Move init to auto-detection phase
	mod_hctl2032_init();
	
	//uint8_t task1 =
	rt_sched_register_task_periodic(&TASK_ENC_READ_ALL,LDAQ_SCHEDULER_FREQ_HZ / cmd->sampling_rate_hz);
	//uint8_t task1 =
	rt_sched_register_idle_task(&task_encoder_send_full_frames);
}

// ======== Register command handler ========
LDAQ_INIT_BLOCK_START(MOD_HCTL2032)
ldaq_comms_register_cmd_handler(FRAMECMD_ENC_START, &process_enc_start);
LDAQ_INIT_BLOCK_END()



#endif //HAVE_LDAQ_SLOT_BUS
