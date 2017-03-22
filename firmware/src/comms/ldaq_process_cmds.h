/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <stdint.h>

/** Functor type */
typedef void (*cmd_handler_t)(const uint8_t *buf);

void ldaq_comms_process_rx_cmd(const uint8_t *buf);

/** Register a handler for a certain incoming OPCODE */
void ldaq_comms_register_cmd_handler(const uint8_t opcode, cmd_handler_t functor);


