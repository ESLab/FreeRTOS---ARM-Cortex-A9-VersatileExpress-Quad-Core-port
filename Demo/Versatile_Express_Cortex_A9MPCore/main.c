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

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Demo Includes. */
#include "integer.h"
#include "PollQ.h"
#include "semtest.h"
#include "BlockQ.h"
#include "countsem.h"
#include "dynamic.h"
#include "flop.h"
#include "GenQTest.h"
#include "QPeek.h"
#include "recmutex.h"
#include "serial.h"
#include "comtest2.h"
/*----------------------------------------------------------------------------*/

/* Delay between cycles of the 'check' task. */
#define mainCHECK_DELAY						( ( portTickType ) 5000 / portTICK_RATE_MS )

/* Demo task priorities. */
#define mainQUEUE_POLL_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY		( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY		( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define mainGEN_Q_PRIORITY			( tskIDLE_PRIORITY )

/* Misc. */
#define mainQUEUE_SIZE				( 3 )
#define mainNO_DELAY				( ( portTickType ) 0 )

#define mainPRINT_PORT				( configUART_PORT )
#if configPLATFORM == 2
#define mainPRINT_BAUDRATE			( 38400 )
#else
#define mainPRINT_BAUDRATE			( 115200 )
#endif

/*----------------------------------------------------------------------------*/

/* Periodically checks to see whether the demo tasks are still running. */
static void vCheckTask( void *pvParameters );
/*----------------------------------------------------------------------------*/

/*
 * Configure the clocks, GPIO and other peripherals as required by the demo.
 */
static void prvSetupHardware( void );
/*----------------------------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName );

int main( void )
{
	/* Initialise the Hardware. */
	prvSetupHardware();

	/* Start the standard demo tasks. */
	vStartIntegerMathTasks( tskIDLE_PRIORITY );
	vStartPolledQueueTasks( mainQUEUE_POLL_PRIORITY );
	vStartSemaphoreTasks( mainSEM_TEST_PRIORITY );
	vStartBlockingQueueTasks( mainBLOCK_Q_PRIORITY );
	vStartCountingSemaphoreTasks();
	vStartDynamicPriorityTasks();
	vStartMathTasks( tskIDLE_PRIORITY );
	vStartGenericQueueTasks( mainGEN_Q_PRIORITY );
	vStartQueuePeekTasks();
	vStartRecursiveMutexTasks();

	/* Start the tasks defined within the file. */
	xTaskCreate( vCheckTask, (const signed char *)"Check", configMINIMAL_STACK_SIZE, NULL, mainCHECK_TASK_PRIORITY, NULL );

	/* Start the scheduler. */
	vTaskStartScheduler();

	/* Should never reach here. */
	vSerialPutString((xComPortHandle)mainPRINT_PORT, (const signed char * const)"Should never reach here!\r\n", 26 );

	/* Will only get here if there was not enough heap space to create the idle task. */
	return 0;
}
/*----------------------------------------------------------------------------*/

void vApplicationTickHook( void )
{

}
/*----------------------------------------------------------------------------*/

void vApplicationIdleHook( void )
{
signed char cChar;
	if ( pdTRUE == xSerialGetChar( (xComPortHandle)mainPRINT_PORT, &cChar, 0UL ) )
	{
		(void)xSerialPutChar( (xComPortHandle)mainPRINT_PORT, cChar, 0UL );
	}
}
/*----------------------------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle *pxTask, signed char *pcTaskName )
{
	( void ) pxTask;
	( void ) pcTaskName;
	 printf("StackOverflowHook\n");
	/* If the parameters have been corrupted then inspect pxCurrentTCB to
	identify which task has overflowed its stack. */
	for( ;; );
}



static void vCheckTask( void *pvParameters )
{
portBASE_TYPE xErrorOccurred = pdFALSE;
portTickType xLastExecutionTime, xFailureTime = 0;
signed char cBuffer[64];

	/* First Task Started. */
	vSerialPutString((xComPortHandle)mainPRINT_PORT,(const signed char * const)"FTS\r\n", 5 );

	/* Initialise xLastExecutionTime so the first call to vTaskDelayUntil()
	works correctly. */
	xLastExecutionTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Perform this check every mainCHECK_DELAY milliseconds. */
		vTaskDelayUntil( &xLastExecutionTime, mainCHECK_DELAY );

		/* Has an error been found in any task? */

		if( xAreIntegerMathsTaskStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: Integer Maths\r\n", strlen( "Fail: Integer Maths\r\n" ) );
		}

		if( xArePollingQueuesStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: Polling Queues\r\n", strlen( "Fail: Polling Queues\r\n" ) );
		}

		if( xAreSemaphoreTasksStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: Sem Tasks\r\n", strlen( "Fail: Sem Tasks\r\n" ) );
		}

		if( xAreBlockingQueuesStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: BlockQ\r\n", strlen( "Fail: BlockQ\r\n" ) );
		}

		if( xAreCountingSemaphoreTasksStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: SemCount\r\n", strlen( "Fail: SemCount\r\n" ) );
		}

		if( xAreDynamicPriorityTasksStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: DynamicPrio\r\n", strlen( "Fail: DynamicPrio\r\n" ) );
		}

		if( xAreMathsTaskStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: FloatMaths\r\n", strlen( "Fail: FloatMaths\r\n" ) );
		}

		if( xAreGenericQueueTasksStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: GenQ\r\n", strlen( "Fail: GenQ\r\n" ) );
		}

		if( xAreQueuePeekTasksStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: PeekQ\r\n", strlen( "Fail: PeekQ\r\n" ) );
		}

		if( xAreRecursiveMutexTasksStillRunning() != pdTRUE )
		{
			xErrorOccurred = pdTRUE;
			vSerialPutString( (xComPortHandle)mainPRINT_PORT, (const signed char * const)"Fail: RecMutex\r\n", strlen( "Fail: RecMutex\r\n" ) );
		}


		/* Send either a pass or fail message.  If an error is found it is
		never cleared again.  We do not write directly to the LCD, but instead
		queue a message for display by the print task. */
		if( xErrorOccurred == pdTRUE )
		{
			if ( 0 == xFailureTime )
			{
				xFailureTime = xLastExecutionTime;
			}
			sprintf( (char *)cBuffer, "Fail: %lu\r\n", xFailureTime / 1000 );
		}
		else
		{
			sprintf( (char *)cBuffer, "Pass: %lu\r\n", xLastExecutionTime / 1000 );

			sprintf( (char *)cBuffer, "From Core: %ld\r\n", portCORE_ID());
		
		}
		vSerialPutString((xComPortHandle)mainPRINT_PORT, (const signed char * const)cBuffer, strlen((char *)cBuffer) );
	}
}
/*-----------------------------------------------------------*/

#if configPLATFORM == 2

static unsigned long prvReadP15_C1_C0_0( void )
{
volatile unsigned long ulReturn = 0UL;
	/* Read Configuration Register C15, c0, 0. */
	__asm volatile(
			" mrc p15, 0, %[retval], c1, c0, 0 			\n"
			: [retval] "=r" (ulReturn) : :
			);
	return ulReturn;
}

/*----------------------------------------------------------------------------*/

static unsigned long prvReadP15_C1_C0_1( void )
{
volatile unsigned long ulReturn = 0UL;
	/* Read Configuration Register C15, c0, 0. */
	__asm volatile(
			" mrc p15, 0, %[retval], c1, c0, 1 			\n"
			: [retval] "=r" (ulReturn) : :
			);
	return ulReturn;
}
/*----------------------------------------------------------------------------*/

static unsigned long prvReadP15_C1_C0_2( void )
{
volatile unsigned long ulReturn = 0UL;
	/* Read Configuration Register C15, c0, 0. */
	__asm volatile(
			" mrc p15, 0, %[retval], c1, c0, 2 			\n"
			: [retval] "=r" (ulReturn) : :
			);
	return ulReturn;
}
/*----------------------------------------------------------------------------*/

static unsigned long prvReadP15_C1_C1_0( void )
{
volatile unsigned long ulReturn = 0UL;
	/* Read Configuration Register C15, c0, 0. */
	__asm volatile(
			" mrc p15, 0, %[retval], c1, c1, 0 			\n"
			: [retval] "=r" (ulReturn) : :
			);
	return ulReturn;
}

/*----------------------------------------------------------------------------*/

static unsigned long prvReadP15_C1_C1_1( void )
{
volatile unsigned long ulReturn = 0UL;
	/* Read Configuration Register C15, c0, 0. */
	__asm volatile(
			" mrc p15, 0, %[retval], c1, c1, 1 			\n"
			: [retval] "=r" (ulReturn) : :
			);
	return ulReturn;
}
/*----------------------------------------------------------------------------*/

static unsigned long prvReadP15_C1_C1_2( void )
{
volatile unsigned long ulReturn = 0UL;
	/* Read Configuration Register C15, c0, 0. */
	__asm volatile(
			" mrc p15, 0, %[retval], c1, c1, 2 			\n"
			: [retval] "=r" (ulReturn) : :
			);
	return ulReturn;
}

/*----------------------------------------------------------------------------*/

static unsigned long prvReadP15_C12_C0_1( void )
{
volatile unsigned long ulReturn = 0UL;
	/* Read Configuration Register C15, c0, 0. */
	__asm volatile(
			" mrc p15, 0, %[retval], c12, c0, 1 		\n"
			: [retval] "=r" (ulReturn) : :
			);
	return ulReturn;
}

/*----------------------------------------------------------------------------*/

#endif

static void prvSetupHardware( void )
{
unsigned long ulVector = 0UL;
#if configPLATFORM == 2
unsigned long ulValue = 0UL;
char cAddress[32];
#endif

	portDISABLE_INTERRUPTS();

	/* Install the Spurious Interrupt Handler to help catch interrupts. */
extern void vPortUnknownInterruptHandler( void *pvParameter );
extern void vPortInstallInterruptHandler( void (*vHandler)(void *), void *pvParameter, unsigned long ulVector, unsigned char ucEdgeTriggered, unsigned char ucPriority, unsigned char ucProcessorTargets );
	for ( ulVector = 0; ulVector < portMAX_VECTORS; ulVector++ )
		vPortInstallInterruptHandler( vPortUnknownInterruptHandler, (void *)ulVector, ulVector, pdTRUE, configMAX_SYSCALL_INTERRUPT_PRIORITY, 1 );

extern void vUARTInitialise(unsigned long ulUARTPeripheral, unsigned long ulBaud, unsigned long ulQueueSize );
	vUARTInitialise( mainPRINT_PORT, mainPRINT_BAUDRATE, 64 );

#if configPLATFORM == 2
	ulValue = portCORE_ID();
	sprintf( cAddress, "Core: %ld\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );

	ulValue = (unsigned long)main;
	sprintf( cAddress, "Main: 0x%08lX\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );

	/* Read the Configuration Registers to find the vital information. */
	ulValue = prvReadP15_C1_C0_0();
	sprintf( cAddress, "SCTLR: 0x%08lX\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );

	/* Read the Configuration Registers to find the vital information. */
	ulValue = prvReadP15_C1_C0_1();
	sprintf( cAddress, "ACTLR: 0x%08lX\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );

	/* Read the Configuration Registers to find the vital information. */
	ulValue = prvReadP15_C1_C0_2();
	sprintf( cAddress, "CPACR: 0x%08lX\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );

	/* Read the Configuration Registers to find the vital information. */
	ulValue = prvReadP15_C1_C1_0();
	sprintf( cAddress, "SCR: 0x%08lX\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );

	/* Read the Configuration Registers to find the vital information. */
	ulValue = prvReadP15_C1_C1_1();
	sprintf( cAddress, "SDER: 0x%08lX\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );

	/* Read the Configuration Registers to find the vital information. */
	ulValue = prvReadP15_C1_C1_2();
	sprintf( cAddress, "NSACR: 0x%08lX\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );

	/* Read the Configuration Registers to find the vital information. */
	ulValue = prvReadP15_C12_C0_1();
	sprintf( cAddress, "MVBAR: 0x%08lX\r\n", ulValue );
	vSerialPutString((xComPortHandle)configUART_PORT,(const signed char * const)cAddress, strlen(cAddress) );
#endif /* configPLATFORM == 2 */

	/* Perform any other peripheral configuration. */
}
/*----------------------------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	__asm volatile (" smc #0 ");
}
/*----------------------------------------------------------------------------*/

extern void vAssertCalled( char *file, int line )
{
	printf("Assertion failed at %s, line %d\n\r",file,line);
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

