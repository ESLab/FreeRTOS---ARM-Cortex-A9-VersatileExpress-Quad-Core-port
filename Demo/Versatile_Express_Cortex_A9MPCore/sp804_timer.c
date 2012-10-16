/*
    FreeRTOS V7.0.1 - Copyright (C) 2011 Real Time Engineers Ltd.


	FreeRTOS supports many tools and architectures. V7.0.0 is sponsored by:
	Atollic AB - Atollic provides professional embedded systems development
	tools for C/C++ development, code analysis and test automation.
	See http://www.atollic.com


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

#include "FreeRTOS.h"
/*----------------------------------------------------------------------------*/

//#define TIMER_0_1_BASE		( 0x10011000 )	/* Realview PBX-A9 */
#define TIMER_0_1_BASE		( 0x60005000 )	/* nVidia Tegra 2 */
#define TIMER_1_LOAD		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x0 ) )	/* Load Register */
#define TIMER_1_VALUE		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x04 ) )	/* Current Value Register */
#define TIMER_1_CONTROL		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x08 ) )	/* Control Register */
#define TIMER_1_INTCLR		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x0C ) )	/* Interrupt Clear Register */
#define TIMER_1_RIS			( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x10 ) )	/* Raw Interrupt Status Register */
#define TIMER_1_MIS			( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x14 ) )	/* Masked Interrupt Status Register */
#define TIMER_1_BGLOAD		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x18 ) )	/* Background Load Register */
#define TIMER_2_LOAD		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x20 ) )	/* Load Register */
#define TIMER_2_VALUE		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x24 ) )	/* Current Value Register */
#define TIMER_2_CONTROL		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x28 ) )	/* Control Register */
#define TIMER_2_INTCLR		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x2C ) )	/* Interrupt Clear Register */
#define TIMER_2_RIS			( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x30 ) )	/* Raw Interrupt Status Register */
#define TIMER_2_MIS			( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x34 ) )	/* Masked Interrupt Status Register */
#define TIMER_2_BGLOAD		( ( unsigned long * volatile ) ( TIMER_0_1_BASE + 0x38 ) )	/* Background Load Register */
/*----------------------------------------------------------------------------*/

void vTimer0Initialise( unsigned long ulLoadValue )
{
	*TIMER_1_LOAD = ulLoadValue;
}
/*----------------------------------------------------------------------------*/

void vTimer0Enable( void )
{
	*TIMER_1_CONTROL = 0xE0UL;
}
/*----------------------------------------------------------------------------*/

void vTimer0InterruptHandler( void *pvParameter )
{
extern void vTaskIncrementTick( void );
	*TIMER_1_INTCLR = 1;

	vTaskIncrementTick();

#if configUSE_PREEMPTION
	portEND_SWITCHING_ISR( 1 );
#endif /* configUSE_PREEMPTION */
}
/*----------------------------------------------------------------------------*/
