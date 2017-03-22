/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <comms/ldaq_comms.h>
#include <utils/circular_buffer_fixsize.h>
#include <comms/ldaq_frames.h>
#include <comms/ldaq_process_cmds.h>
#include <system/ldaq_rt_scheduler.h>
#include <conf_libredaq_firmware.h>

struct circular_buffer_fixsize  buffer_usb_rx;

// Another circular buffer of pointers to buffers:
struct tx_queue_entry_t
{
	void *buf;   //!< Set to NULL means entry is unassigned / free.
	uint16_t num_bytes;
};
#define TX_BUF_MAX_ENTRIES  (1<<4)
#define TX_BUF_COUNT_MASK   (TX_BUF_MAX_ENTRIES-1)
uint8_t tx_buf_next_read_index,tx_buf_next_write_index;
struct tx_queue_entry_t tx_queue_entries[TX_BUF_MAX_ENTRIES];


/** Init comms data; setup USB stack; setup RS485 stack. */
void ldaq_comms_init(void)
{
	// Init data:
	circular_buffer_fixsize_init(&buffer_usb_rx);
	
	tx_buf_next_read_index=0;
	tx_buf_next_write_index=0;
	for (int i=0;i<TX_BUF_MAX_ENTRIES;i++) {
		tx_queue_entries[i].buf=NULL;
		tx_queue_entries[i].num_bytes = 0;
	}

	// Init USB:
	udc_start();
}

uint32_t time_last_rx_cmd;

void ldaq_usb_callback_rx_notify(uint8_t port)
{
	uint8_t tmp_buf[100];
	
	// Append incoming USB data to the circular buffer:
	uint8_t nBytesRx = udi_cdc_get_nb_received_data();
	while (nBytesRx>0)
	{
		// udi_cdc_read_buf() returns the number of data remaining
		const iram_size_t nToRead = Min(nBytesRx,sizeof(tmp_buf));
		nBytesRx = udi_cdc_read_buf((uint8_t*)tmp_buf, nToRead);
		
		#warning TODO: Check for buffer overflow, mark some error flag, etc.
		circular_buffer_fixsize_push_many(&buffer_usb_rx,tmp_buf,nToRead);
	}
	
	// Reset timeout of last RX data:
	time_last_rx_cmd = rt_sched_now();
}

/** This function is called periodically to process the data in the RX circular buffer and 
  * interpret them as potential commands.
  */
void ldaq_comms_dispatch_incoming_cmds(void)
{
	uint8_t tmp_rx_valid_frame[0x114];
	bool    valid_frame_rx = false;
	
	// --- START CRITICAL SECTION ------ // Disable interrupts during accessing the RX circular buffer
	irqflags_t irqsave = ldaq_enter_cs();
	if (!circular_buffer_fixsize_empty(&buffer_usb_rx))
	{
		const iram_size_t RX_LEN = circular_buffer_fixsize_size(&buffer_usb_rx);
		if (LDAQ_FRAME_START!=circular_buffer_fixsize_peek(&buffer_usb_rx, 0))
		{
			// Discard this byte, since it cannot be a valid start of frame:
			circular_buffer_fixsize_pop(&buffer_usb_rx);
		}
		else if (RX_LEN>=4)
		{
				const uint8_t EXPECTED_LEN =  4 + circular_buffer_fixsize_peek(&buffer_usb_rx, 2);
				if (RX_LEN==EXPECTED_LEN)
				{
					// The frame is ready: do final flag check
					if (LDAQ_FRAME_END!=circular_buffer_fixsize_peek(&buffer_usb_rx, RX_LEN-1) )
					{
						// Abort rx frame: skip at least the first byte and continue trying interpreting at that point:
						circular_buffer_fixsize_pop(&buffer_usb_rx);
					}
					else
					{
						// -------------------
						// RX FRAME LOOKS OK
						// -------------------
						circular_buffer_fixsize_pop_many(&buffer_usb_rx,tmp_rx_valid_frame,EXPECTED_LEN);
						valid_frame_rx = true;
					}
				}
		}
		// TIMEOUT if no new data arrived in a prudent time span:
		//if ( (rt_sched_now()-time_last_rx_cmd) > TIMEOUT_WITHIN_RX_COMMANDS_BYTES_SEC * LDAQ_SCHEDULER_FREQ_HZ )
			//circular_buffer_fixsize_clear(&buffer_usb_rx);
	}
	ldaq_leave_cs(irqsave);
	// --- END CRITICAL SECTION ------
	
	// Process frame:
	// -------------------
	if (valid_frame_rx)
	{
		ldaq_comms_process_rx_cmd( tmp_rx_valid_frame );
		pio_toggle_pin(LED2_GPIO);
	}
}


extern volatile bool udi_cdc_data_running;  // In udi_cdc.c

void ldaq_comms_task_outbounds_queue()
{
	// 1st: If the TX is full and would lead to a busy wait, just skip it for now and avoid blocking!
	{
		const int port=0;
		if (!udi_cdc_multi_is_tx_ready(port) && udi_cdc_data_running) 
			return;
	}
	
	// Get ptr to data:
	struct tx_queue_entry_t *tx_entry = NULL;
	
	{
		irqflags_t flags = ldaq_enter_cs();
		// Do we have pending entries??
		if (tx_buf_next_write_index!=tx_buf_next_read_index)
		{
			tx_entry = &tx_queue_entries[tx_buf_next_read_index];
		}
		ldaq_leave_cs(flags);
	}
	
	if (tx_entry)
	{
		udi_cdc_multi_write_buf(0 /*port*/,tx_entry->buf,tx_entry->num_bytes);

		irqflags_t flags = ldaq_enter_cs();
		++tx_buf_next_read_index;
		tx_buf_next_read_index&=TX_BUF_COUNT_MASK;
		ldaq_leave_cs(flags);
		pio_toggle_pin(LED1_GPIO);
	}
}

// ------------------------------------------
// Outbounds tx buffer. Interrupt-driven
// ------------------------------------------
/** Enqueue one frame for transmission when possible */
void ldaq_comms_enque_tx_frame(void *buf, uint16_t num_bytes)
{
	// Enqueue:
	irqflags_t flags = ldaq_enter_cs();
	tx_queue_entries[tx_buf_next_write_index].buf = buf;
	tx_queue_entries[tx_buf_next_write_index].num_bytes = num_bytes;
	++tx_buf_next_write_index;
	tx_buf_next_write_index&=TX_BUF_COUNT_MASK;
	ldaq_leave_cs(flags);
}

void ldaq_usb_callback_tx_empty_notify(uint8_t port)
{
}

