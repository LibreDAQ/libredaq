/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#include <conf_libredaq_firmware.h>
#include <firmware_modes/ldaq_firmware_modes.h>
#include <comms/ldaq_frames.h>
#include <comms/ldaq_process_cmds.h>  // ldaq_comms_register_cmd_handler()
#include <utils/ldaq_util_macros.h>


procedure_t firm_mode_main_loops[COUNT_FIRMWARE_MODES];
procedure_t firm_mode_on_enter[COUNT_FIRMWARE_MODES];
procedure_t firm_mode_on_leave[COUNT_FIRMWARE_MODES];

// Contains the current firmware mode:
firmware_mode_t FIRMWARE_MODE = FIRM_MODE_NORMAL;

/** Register a firmware mode */
void ldaq_register_firmware_mode(
	const firmware_mode_t mode,
	procedure_t main_loop_functor,
	procedure_t on_enter_mode,
	procedure_t on_leave_mode)
{
	firm_mode_main_loops[mode] = main_loop_functor;
	firm_mode_on_enter[mode] = on_enter_mode ? on_enter_mode : &dummy_do_nothing;
	firm_mode_on_leave[mode] = on_leave_mode ? on_leave_mode : &dummy_do_nothing;
}

// -------------------------------------------
// CMD: FRAMECMD_FIRMWARE_MODE
// -------------------------------------------
static void process_cmd_switch_firmware_mode(const uint8_t *buf)
{
	const struct TFrame_LDAQCMD_SwitchFirmwareMode *cmd = (const struct TFrame_LDAQCMD_SwitchFirmwareMode *)buf;
	
	if (cmd->new_firmware_mode>=COUNT_FIRMWARE_MODES)
		return; // nothing to do! Report error?

	// Call on_leave() on current mode:
	(*firm_mode_on_leave[FIRMWARE_MODE])();

	// Call on_enter() for the new mode:
	(*firm_mode_on_enter[cmd->new_firmware_mode])();
	
	// Just change the mode variable, it will make effect upon next pass thru firm_mode_run_main_loop():
	FIRMWARE_MODE = cmd->new_firmware_mode;
}

// ======== Register command handler ========
LDAQ_INIT_BLOCK_START(REGCMD_FIRMWARE_MODE)
	ldaq_comms_register_cmd_handler(FRAMECMD_FIRMWARE_MODE, &process_cmd_switch_firmware_mode);
	// Init variables:
	for (int i=0;i<COUNT_FIRMWARE_MODES;i++) {
		firm_mode_main_loops[i] = 
		firm_mode_on_enter[i] = 
		firm_mode_on_leave[i] = &dummy_do_nothing;
	}
LDAQ_INIT_BLOCK_END()


/** Does nothing: can be used as default values in \a ldaq_register_firmware_mode() */
void dummy_do_nothing(void) { }
	
