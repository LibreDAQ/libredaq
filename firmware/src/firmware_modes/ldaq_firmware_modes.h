/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <comms/ldaq_frames.h>

/** Functor types for firmware mode */
typedef void (*procedure_t)(void);

/** Does nothing: can be used as default values in \a ldaq_register_firmware_mode() */
void dummy_do_nothing(void);

/** Register a firmware mode */
void ldaq_register_firmware_mode(const firmware_mode_t mode, procedure_t main_loop_functor, procedure_t on_enter_mode, procedure_t on_leave_mode);


/** Contains the current firmware mode */
extern firmware_mode_t FIRMWARE_MODE;
extern procedure_t firm_mode_main_loops[COUNT_FIRMWARE_MODES];
extern procedure_t firm_mode_on_enter[COUNT_FIRMWARE_MODES];
extern procedure_t firm_mode_on_leave[COUNT_FIRMWARE_MODES];

/** To be called from inside main()'s infinite loop */
static inline void firm_mode_run_main_loop(void)
{
	(*firm_mode_main_loops[FIRMWARE_MODE])();
}


