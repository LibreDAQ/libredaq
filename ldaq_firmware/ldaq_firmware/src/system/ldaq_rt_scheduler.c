/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */


#include <sam3u.h> //<sysclk.h>

#include <system/ldaq_rt_scheduler.h>
#include <firmware_modes/ldaq_firmware_modes.h>
#include <utils/ldaq_util_macros.h>

volatile uint32_t timer_us;

uint8_t  registered_idle_tasks;
uint8_t  registered_fixed_tasks;
uint8_t  registered_periodic_tasks;

struct InfoPerFixedTask
{
	void (*func_ptr)(void);
};

struct InfoPerPeriodicTask
{
	void (*func_ptr)(void);
	unsigned int     period; //!< =1, execute on every tick, =10, execute on every 10'th,...
	unsigned int     cnt; //!< Internal counter, will execute it when it reaches "period"
};

struct InfoPerFixedTask    list_idle_tasks[LDAQ_SCHEDULER_REALTIME_MAX_IDLE_TASKS];
struct InfoPerFixedTask    list_fixed_tasks[LDAQ_SCHEDULER_REALTIME_MAX_FIXED_TASKS];
struct InfoPerPeriodicTask list_periodic_tasks[LDAQ_SCHEDULER_REALTIME_MAX_PERIODIC_TASKS]; 

// a task that does nothing: used as a fill-in for un-assigned task slots. 
// It may be faster to call + return than check if a func_ptr!=NULL in a loop.
static void rt_sched_dummy_task(void)
{
}


void rt_sched_run_idle_tasks(void)
{
	//---------------------------------
	// IDLE tasks list
	//---------------------------------
	for (uint8_t i=0;i<LDAQ_SCHEDULER_REALTIME_MAX_IDLE_TASKS;i++)
			list_idle_tasks[i].func_ptr();
}

void SysTick_Handler(void)
{
	++timer_us;
	// In the "normal" firmware, execute the slotted tasks. Otherwise, just increment time counter
	if (FIRMWARE_MODE!=FIRM_MODE_NORMAL) {
		return;
	}
	// Normal procedure:
	irqflags_t irqsave = ldaq_enter_cs();

	//---------------------------------
	// PERIODIC task list: execute with configurable period
	//---------------------------------
	for (uint8_t i=0;i<LDAQ_SCHEDULER_REALTIME_MAX_PERIODIC_TASKS;i++)
	{
		if (list_periodic_tasks[i].func_ptr!=NULL)
		{
			if (++list_periodic_tasks[i].cnt==list_periodic_tasks[i].period)
			{
				list_periodic_tasks[i].func_ptr();
				list_periodic_tasks[i].cnt=0;
			}
		}
	}
	
	//---------------------------------
	// FIXED task list: execute always
	//---------------------------------
	for (uint8_t i=0;i<LDAQ_SCHEDULER_REALTIME_MAX_FIXED_TASKS;i++)
		list_fixed_tasks[i].func_ptr();
	
	ldaq_leave_cs(irqsave);
}

// Must be called at start up
void rt_sched_init()
{
	// Init variables:
	timer_us=0;
	rt_sched_end_all_task(); // Init all tasks-related variables

	// Set sys-tick frequency:
	SysTick_Config( sysclk_get_cpu_hz() / LDAQ_SCHEDULER_FREQ_HZ );

	// Force a high priority for the systick:
	NVIC_SetPriority (SysTick_IRQn, LDAQ_INT_PRIORITY_LEVEL_SYSTICK);  /* set Priority for Systick Interrupt */
}

/** Register a new task to be run on every tick */
uint8_t  rt_sched_register_task_every_tick( void (*ptr_func)(void) )
{
	list_fixed_tasks[registered_fixed_tasks].func_ptr = ptr_func;
	
	return ++registered_fixed_tasks;
}

/** Register a new idle task to be run on the main idle busy loop */
uint8_t  rt_sched_register_idle_task( void (*ptr_func)(void) )
{
	list_idle_tasks[registered_idle_tasks].func_ptr = ptr_func;
	
	return ++registered_idle_tasks;
}


/** Register a new task to be run on every n'th tick */
uint8_t rt_sched_register_task_periodic( void (*ptr_func)(void), unsigned int period )
{
	list_periodic_tasks[registered_periodic_tasks].func_ptr = ptr_func;
	list_periodic_tasks[registered_periodic_tasks].period = period;
	list_periodic_tasks[registered_periodic_tasks].cnt = 0;

	return ++registered_periodic_tasks;
}

/** Remove (end) all registered tasks from the scheduler */
void rt_sched_end_all_task(void)
{
	registered_idle_tasks     = 0;
	registered_fixed_tasks    = 0;
	registered_periodic_tasks = 0;
	for (uint8_t i=0;i<LDAQ_SCHEDULER_REALTIME_MAX_IDLE_TASKS;i++)
	list_idle_tasks[i].func_ptr= &rt_sched_dummy_task;
	
	for (uint8_t i=0;i<LDAQ_SCHEDULER_REALTIME_MAX_FIXED_TASKS;i++)
	list_fixed_tasks[i].func_ptr= &rt_sched_dummy_task;

	for (uint8_t i=0;i<LDAQ_SCHEDULER_REALTIME_MAX_PERIODIC_TASKS;i++) {
		list_periodic_tasks[i].cnt = 0;
		list_periodic_tasks[i].func_ptr = NULL; //&rt_sched_dummy_task;
		list_periodic_tasks[i].period = 0;
	}
}


