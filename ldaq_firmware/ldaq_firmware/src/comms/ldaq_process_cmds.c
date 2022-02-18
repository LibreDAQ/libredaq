/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <comms/ldaq_process_cmds.h>
#include <comms/ldaq_frames.h>
#include <system/ldaq_rt_scheduler.h>
#include <comms/ldaq_comms.h>
#include <asf.h>

// The main table of opcodes -> handler.
// PC->LDAQ opcodes range is FRAMECMD_START_PC2BOARD_CMD_CODES-0xff, hence the table length.
cmd_handler_t opcode_handlers_table[0x100-FRAMECMD_START_PC2BOARD_CMD_CODES];   // Automatically initialized to zeros, as by C standard

/** Register a handler for a certain incoming OPCODE */
void ldaq_comms_register_cmd_handler(const uint8_t opcode, cmd_handler_t functor)
{
	if (opcode<FRAMECMD_START_PC2BOARD_CMD_CODES) 
		return; // Error!
		
	opcode_handlers_table[opcode-FRAMECMD_START_PC2BOARD_CMD_CODES] = functor;
}

// -------------------------------------------
//  Process frames: enter point
// -------------------------------------------
void ldaq_comms_process_rx_cmd(const uint8_t *buf)
{
	uint8_t OPCODE = buf[1];
	cmd_handler_t handler = opcode_handlers_table[OPCODE-FRAMECMD_START_PC2BOARD_CMD_CODES];
	
	if (handler) {
		(*handler)(buf);
	}
	else {
		#warning TODO: Return an error code
	}
}


