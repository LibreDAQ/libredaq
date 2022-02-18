/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <conf_libredaq_board.h>
#include <comms/ldaq_frames.h>

#define ADC_FRAME_BUFFERS          (3)
#define ADC_FRAME_BLOCK_SIZE       (1<<5)
#define ADC_FRAME_BLOCK_SIZE_MASK  (ADC_FRAME_BLOCK_SIZE-1)

#pragma pack(push, 1) // exact fit - no padding

extern struct TFrame_LDAQDATA_ADC_16bx8 frame_adc[ADC_FRAME_BUFFERS][ADC_FRAME_BLOCK_SIZE];

#pragma pack(pop)

extern volatile int frame_adc_buffer_idx;
extern volatile int frame_adc_next_idx;
extern volatile uint32_t adc_is_converting_time;  // 0 if not converting; a timestep otherwise

extern volatile int frame_adc_buffer_idx_done; // Points to a just filled-in block of data ready to be sent.


void task_adc_send_full_frame(void);


