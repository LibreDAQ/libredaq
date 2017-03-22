/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

// Chip-embedded PWM generators

/** Initialize */
void embedded_pwm_init(void);

/** Set freq & duty cycle */
void embedded_pwm_set_duty_cycle(uint8_t pwm_idx, uint32_t duty_cycle);

