/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <stdint.h>

/** Init comms data; setup USB stack; setup RS485 stack. */
void ldaq_comms_init(void);

/** Enqueue one frame for transmission when possible */
void ldaq_comms_enque_tx_frame(void *buf, uint16_t num_bytes);

/** To be called from the main idle loop: checks for complete incoming frames and dispatch them */
void ldaq_comms_dispatch_incoming_cmds(void);

/** To be called from the main idle loop: sends pending USB pkgs */
void ldaq_comms_task_outbounds_queue(void);

