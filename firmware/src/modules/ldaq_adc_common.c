/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <asf.h>

#include "ldaq_adc_common.h"
#include <comms/ldaq_process_cmds.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <utils/ldaq_util_macros.h>

// -------------------------------------------
// AUX FOR CMD: ADC start
// -------------------------------------------
#pragma pack(push, 1) // exact fit - no padding

struct TFrame_LDAQDATA_ADC_16bx8 frame_adc[ADC_FRAME_BUFFERS][ADC_FRAME_BLOCK_SIZE];

#pragma pack(pop)

LDAQ_INIT_BLOCK_START(INITIALIZE_frame_adc_buffers)
	for (int k=0;k<ADC_FRAME_BUFFERS;k++)
		for (int i=0;i<ADC_FRAME_BLOCK_SIZE;i++)
			TFrame_LDAQDATA_ADC_16bx8_init(&frame_adc[k][i]);
LDAQ_INIT_BLOCK_END()

volatile int frame_adc_buffer_idx = 0;
volatile int frame_adc_next_idx = 0;
volatile uint32_t adc_is_converting_time = 0;  // 0 if not converting; a timestep otherwise

volatile int frame_adc_buffer_idx_done = -1; // Points to a just filled-in block of data ready to be sent.

void task_adc_send_full_frame(void)
{
	if (frame_adc_buffer_idx_done>=0)
	{
		ldaq_comms_enque_tx_frame(&frame_adc[frame_adc_buffer_idx_done][0],sizeof(frame_adc[0][0])*ADC_FRAME_BLOCK_SIZE);
		frame_adc_buffer_idx_done=-1;
	}
}

