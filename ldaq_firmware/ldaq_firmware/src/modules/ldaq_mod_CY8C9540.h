/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <conf_libredaq_board.h>
#include <utils/ldaq_util_macros.h>
#if HAVE_LDAQ_SLOT_BUS

// PWM/IO module with chip CY8C9540: 

/* I2C bus on slot-specific signals: 
 CTRL0           : SCL
 CTRL1           : SDA
 ALL_SLOTS_RESET : RESET
 
 I2C: 
 - Max 100 kHz
 
*/

#define PWM_CYPRESS_SCL_GPIO        LDAQ_SLOT3_CTRL0_GPIO
#define PWM_CYPRESS_SCL_PIO         LDAQ_SLOT3_CTRL0_PIO
#define PWM_CYPRESS_SCL_IDX         LDAQ_SLOT3_CTRL0_IDX

#define PWM_CYPRESS_SDA_GPIO        LDAQ_SLOT3_CTRL1_GPIO
#define PWM_CYPRESS_SDA_PIO         LDAQ_SLOT3_CTRL1_PIO
#define PWM_CYPRESS_SDA_IDX         LDAQ_SLOT3_CTRL1_IDX

/** Initialize the device */
void mod_cy8c9540_init(void);


/** Sets one output as PWM */
void mod_cy8c9540_set_pwm(int pin);


#endif //HAVE_LDAQ_SLOT_BUS
