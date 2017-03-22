/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */
#pragma once

#include <conf_libredaq_firmware.h>
#include <compiler.h>

/** Only access directly to this variable from non-interruptible code. 
  * In general, call RealTimeScheduler::now() */
extern volatile uint32_t timer_us;

// RealTimeScheduler

void rt_sched_init(void); // Must be called at start up

/** Get current number of ticks in real-time high-res clock */
static inline uint32_t rt_sched_now(void)
{
	return timer_us;
}

void rt_sched_run_idle_tasks(void);

/** Register a new idle task to be run on the main idle busy loop */
uint8_t rt_sched_register_idle_task( void (*ptr_func)(void) );
/** Register a new task to be run on every tick */
uint8_t rt_sched_register_task_every_tick( void (*ptr_func)(void) );
/** Register a new task to be run on every n'th tick */
uint8_t rt_sched_register_task_periodic( void (*ptr_func)(void), unsigned int period );

/** Remove (end) all registered tasks from the scheduler */
void rt_sched_end_all_task(void);

