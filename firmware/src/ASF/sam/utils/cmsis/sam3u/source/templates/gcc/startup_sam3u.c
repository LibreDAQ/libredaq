/**
 * \file
 *
 * \brief Startup file for SAM3U.
 *
 * Copyright (c) 2011-2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "exceptions.h"
#include "sam3u.h"
#include "system_sam3u.h"

/* Initialize segments */
extern uint32_t _sfixed;
extern uint32_t _efixed;
extern uint32_t _etext;
extern uint32_t _srelocate;
extern uint32_t _erelocate;
extern uint32_t _szero;
extern uint32_t _ezero;
extern uint32_t _sstack;
extern uint32_t _estack;

/** \cond DOXYGEN_SHOULD_SKIP_THIS */
int main(void);
/** \endcond */

void __libc_init_array(void);

/* Exception Table */
__attribute__ ((section(".vectors")))
IntFunc exception_table[] = {

	/* Configure Initial Stack Pointer, using linker-generated symbols */
	(IntFunc) (&_estack),
	Reset_Handler,
	NMI_Handler,
	HardFault_Handler,
	MemManage_Handler,
	BusFault_Handler,
	UsageFault_Handler,
	0, 0, 0, 0,	          /*  Reserved */
	SVC_Handler,
	DebugMon_Handler,
	0,	                  /*  Reserved */
	PendSV_Handler,
	SysTick_Handler,

	/* Configurable interrupts */
	SUPC_Handler,      /* 0  Supply Controller */
	RSTC_Handler,      /* 1  Reset Controller */
	RTC_Handler,       /* 2  Real Time Clock */
	RTT_Handler,       /* 3  Real Time Timer */
	WDT_Handler,       /* 4  Watchdog Timer */
	PMC_Handler,       /* 5  PMC */
	EFC0_Handler,      /* 6  EEFC 0 */
	EFC1_Handler,      /* 7  EEFC 1 */
	UART_Handler,      /* 8  UART0 */
	SMC_Handler,       /* 9  SMC */
	PIOA_Handler,      /* 10 Parallel IO Controller A */
	PIOB_Handler,      /* 11 Parallel IO Controller B */
#ifdef ID_PIOC
	PIOC_Handler,      /* 12 Parallel IO Controller C */
#else
	Dummy_Handler,
#endif
	USART0_Handler,    /* 13 USART 0 */
	USART1_Handler,    /* 14 USART 1 */
	USART2_Handler,    /* 15 USART 2 */
#ifdef ID_USART3
	USART3_Handler,    /* 16 USART 3 */
#else
	Dummy_Handler,
#endif
	HSMCI_Handler,     /* 17 MCI */
	TWI0_Handler,      /* 18 TWI 0 */
	TWI1_Handler,      /* 19 TWI 1 */
	SPI_Handler,       /* 20 SPI */
	SSC_Handler,       /* 21 SSC */
	TC0_Handler,       /* 22 Timer Counter 0 */
	TC1_Handler,       /* 23 Timer Counter 1 */
	TC2_Handler,       /* 24 Timer Counter 2 */
	PWM_Handler,       /* 25 PWM */
	ADC12B_Handler,    /* 26 ADC12B controller */
	ADC_Handler,       /* 27 ADC controller */
	DMAC_Handler,      /* 28 DMA controller */
	UDPHS_Handler,     /* 29 USB High Speed Port */
	Dummy_Handler      /* 30 not used */
};

/* TEMPORARY PATCH FOR SCB */
#define SCB_VTOR_TBLBASE_Pos               29                            /*!< SCB VTOR: TBLBASE Position */
#define SCB_VTOR_TBLBASE_Msk               (1UL << SCB_VTOR_TBLBASE_Pos) /*!< SCB VTOR: TBLBASE Mask */

/**
 * \brief This is the code that gets called on processor reset.
 * To initialize the device, and call the main() routine.
 */
void Reset_Handler(void)
{
	uint32_t *pSrc, *pDest;

	/* Initialize the relocate segment */
	pSrc = &_etext;
	pDest = &_srelocate;

	if (pSrc != pDest) {
		for (; pDest < &_erelocate;) {
			*pDest++ = *pSrc++;
		}
	}

	/* Clear the zero segment */
	for (pDest = &_szero; pDest < &_ezero;) {
		*pDest++ = 0;
	}

	/* Set the vector table base address */
	pSrc = (uint32_t *) & _sfixed;
	SCB->VTOR = ((uint32_t) pSrc & SCB_VTOR_TBLOFF_Msk);

	if (((uint32_t) pSrc >= IRAM0_ADDR) && ((uint32_t) pSrc < IRAM0_ADDR + IRAM_SIZE)) {
		SCB->VTOR |= 1 << SCB_VTOR_TBLBASE_Pos;
	}

	/* Initialize the C library */
	__libc_init_array();

	/* Branch to main function */
	main();

	/* Infinite loop */
	while (1);
}
