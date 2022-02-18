/*+-------------------------------------------------------------------------+
  |                             LibreDAQ                                    |
  |                                                                         |
  | Copyright (C) 2015  Jose Luis Blanco Claraco                            |
  | Distributed under GNU General Public License version 3                  |
  |   See <http://www.gnu.org/licenses/>                                    |
  +-------------------------------------------------------------------------+  */

#pragma once

#include <config/conf_libredaq_firmware.h>
#include <compiler.h>

// Allows executing start-up code in C:
#define LDAQ_INIT_BLOCK_START(__FUNC_NAME__)  \
	void init_block_##__FUNC_NAME__(void) __attribute__((constructor)); \
	void init_block_##__FUNC_NAME__(void) {
#define LDAQ_INIT_BLOCK_END()  }


// GPIO Helpers:
#define LDAQ_CBI_(_PIO_NAME,_PIO_IDX)  _PIO_NAME->PIO_CODR = 1 << _PIO_IDX;
#define LDAQ_CBI(_LDAQ_SIGNAL_NAME)  LDAQ_CBI_(_LDAQ_SIGNAL_NAME##_PIO,_LDAQ_SIGNAL_NAME##_IDX)

#define LDAQ_SBI_(_PIO_NAME,_PIO_IDX)  _PIO_NAME->PIO_SODR = 1 << _PIO_IDX;
#define LDAQ_SBI(_LDAQ_SIGNAL_NAME)  LDAQ_SBI_(_LDAQ_SIGNAL_NAME##_PIO,_LDAQ_SIGNAL_NAME##_IDX)

// NOP sequences:
#define NOP_DELAY_1()  __NOP();
#define NOP_DELAY_2()  __NOP();__NOP();
#define NOP_DELAY_3()  __NOP();__NOP();__NOP();
#define NOP_DELAY_4()  __NOP();__NOP();__NOP();__NOP();
#define NOP_DELAY_5()  __NOP();__NOP();__NOP();__NOP();__NOP();
#define NOP_DELAY_6()  __NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
#define NOP_DELAY_10()  NOP_DELAY_5() NOP_DELAY_5()
#define NOP_DELAY_20()  NOP_DELAY_10() NOP_DELAY_10()

// Preemptive interruption system:
static inline uint32_t ldaq_enter_cs(void)
{
	__DMB();
	uint32_t  ret = __get_BASEPRI();
	__set_BASEPRI( ((LDAQ_INT_PRIORITY_BASE_CRITICAL_SECTIONS << (8 - __NVIC_PRIO_BITS)) & 0xff) );
	return ret;
}

// Preemptive interruption system:
static inline void ldaq_leave_cs(uint32_t saved_prio)
{
	__DMB();
	__set_BASEPRI(saved_prio);
}

