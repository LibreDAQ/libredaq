/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_mod_ADS1248.h"
#include <comms/ldaq_process_cmds.h>
#include <comms/ldaq_frames.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <utils/ldaq_util_macros.h>
#if HAVE_LDAQ_SLOT_BUS

// -------------------------------------------
// AUX FOR CMD: ADC start
// -------------------------------------------
#define ADC24b_FRAME_BUFFERS          (2)
#define ADC24b_FRAME_BLOCK_SIZE       (1<<3)
#define ADC24b_FRAME_BLOCK_SIZE_MASK  (ADC24b_FRAME_BLOCK_SIZE-1)
#pragma pack(push, 1) // exact fit - no padding
struct TFrame_LDAQDATA_ADC_24bx4 frame_adc24[ADC24b_FRAME_BUFFERS][ADC24b_FRAME_BLOCK_SIZE];
#pragma pack(pop)

LDAQ_INIT_BLOCK_START(INITIALIZE_frame_adc24_buffers)
for (int k=0;k<ADC24b_FRAME_BUFFERS;k++)
	for (int i=0;i<ADC24b_FRAME_BLOCK_SIZE;i++)
		TFrame_LDAQDATA_ADC_24bx4_init(&frame_adc24[k][i]);
LDAQ_INIT_BLOCK_END()


volatile int frame_adc24_buffer_idx = 0;
volatile int frame_adc24_next_idx = 0;
volatile uint32_t adc24_is_converting_time = 0;  // 0 if not converting; a timestep otherwise
volatile int frame_adc24_buffer_idx_done = -1; // Points to a just filled-in block of data ready to be sent.

#define ADS1248_ADC_CHANNELS                8  // [0-7] Normally we set 4 differential inputs
volatile int adc24_converting_mux_channel = 0; // Next positive MUX channel to convert in the ADC


static void task_ads1248_send_done_frames(void)
{
	if (frame_adc24_buffer_idx_done>=0)
	{
		ldaq_comms_enque_tx_frame(&frame_adc24[frame_adc24_buffer_idx_done][0],sizeof(frame_adc24[0][0])*ADC24b_FRAME_BLOCK_SIZE);
		frame_adc24_buffer_idx_done=-1;
	}
}

// -------------------------------------------
// CMD: AMP-ADC start
// -------------------------------------------
static void process_amp_adc_start(const uint8_t *buf)
{
	const struct TFrame_LDAQCMD_ADC_AMP_Start *cmd = (const struct TFrame_LDAQCMD_ADC_AMP_Start *)buf;

	#warning TODO: Move init to auto-detection phase
	mod_ads1248_init(cmd->desired_gain, cmd->sampling_rate_hz);
	
	// ** uncomment when the line above is removed! *
	// Set user-set params:
	//mod_ads1248_set_gain_and_rate(cmd->desired_gain, cmd->sampling_rate_hz);
	
	// Start sampling these channels:
	adc24_converting_mux_channel=0;
	mod_ads1248_set_mux(adc24_converting_mux_channel,adc24_converting_mux_channel+1);
	
	//uint8_t task1 =
	rt_sched_register_task_periodic(&task_ads1248_start_conversion,LDAQ_SCHEDULER_FREQ_HZ / cmd->sampling_rate_hz);
	//uint8_t task2 =
	rt_sched_register_idle_task(&task_ads1248_send_done_frames);
}

void task_ads1248_start_conversion(void)
{
	if (!adc24_is_converting_time)
	{
		adc24_is_converting_time = rt_sched_now();
		mod_ads1248_convst();
	}
}

/** Interrupt handler: To be called for each conversion done (one single ADC channel on the chip!) */
void ads1248_busy_edge_handler(uint32_t a,uint32_t b)
{
	struct TFrame_LDAQDATA_ADC_24bx4 * trg;
	{
		irqflags_t flags = ldaq_enter_cs();
		trg = &frame_adc24[frame_adc24_buffer_idx][frame_adc24_next_idx];
		// Only save convert time for the first differential channel-pair:
		if (adc24_converting_mux_channel==0) {
			trg->time = adc24_is_converting_time;
		}
	
		// Read conversion result:
		mod_ads1248_read_conv_result( trg->adcs + 3*(adc24_converting_mux_channel>>1) );
	
		// Switch MUX to next differential channels:
		adc24_converting_mux_channel+=2;
		if (adc24_converting_mux_channel>=ADS1248_ADC_CHANNELS)
		{
			// we are done with all analog inputs:
			adc24_converting_mux_channel=0;
		}
		mod_ads1248_set_mux(adc24_converting_mux_channel,adc24_converting_mux_channel+1);
			
		adc24_is_converting_time=0; // Signal for the timer to start a new conversion
		ldaq_leave_cs(flags);
	}

	// Only when we are done with one entire set of ADC measurements (all channels have been scanned):
	if (adc24_converting_mux_channel==0)
	{
		#warning TODO: Put real slot index!!
		trg->src_slot = 0;

		++frame_adc24_next_idx;
		frame_adc24_next_idx &= ADC24b_FRAME_BLOCK_SIZE_MASK;
		if (!frame_adc24_next_idx)
		{
			frame_adc24_buffer_idx_done = frame_adc24_buffer_idx;
			if (++frame_adc24_buffer_idx>=ADC24b_FRAME_BUFFERS)
			frame_adc24_buffer_idx=0;
		}
	}
}

// ======== Register command handler ========
LDAQ_INIT_BLOCK_START(MOD_ADS1248)
ldaq_comms_register_cmd_handler(FRAMECMD_ADC_AMP_START, &process_amp_adc_start);
LDAQ_INIT_BLOCK_END()


#endif //HAVE_LDAQ_SLOT_BUS
