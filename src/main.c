/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

#include "hardware.h"
/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

/* Traffic Light System includes */
#include "trafficflow.h"
#include "trafficgenerator.h"
#include "trafficlights.h"
#include "signal_display.h"

/* Queue Handles - Global definitions for inter-task communication */

QueueHandle_t xFlowToLightQueue = NULL;    /* Flow rate -> Traffic Light Task */
QueueHandle_t xFlowToGeneratorQueue = NULL;  /* Flow rate -> Traffic Generator Task */
QueueHandle_t xCarQueue = NULL;            /* New cars -> Traffic Display Task */
QueueHandle_t xLightStateQueue = NULL;     /* Light state -> Traffic Display Task */


/* Software Timer Handles-For traffic light control */

TimerHandle_t xGreenLightTimer = NULL;
TimerHandle_t xYellowLightTimer = NULL;
TimerHandle_t xRedLightTimer = NULL;


TaskHandle_t xTrafficFlowTaskHandle = NULL;
TaskHandle_t xTrafficGeneratorTaskHandle = NULL;
TaskHandle_t xTrafficLightTaskHandle = NULL;
TaskHandle_t xTrafficDisplayTaskHandle = NULL;

/* Task Priorities */

#define TRAFFIC_FLOW_TASK_PRIORITY      (tskIDLE_PRIORITY + 2)
#define TRAFFIC_GENERATOR_TASK_PRIORITY   (tskIDLE_PRIORITY + 2)
#define TRAFFIC_LIGHT_TASK_PRIORITY     (tskIDLE_PRIORITY + 3)
#define TRAFFIC_DISPLAY_TASK_PRIORITY   (tskIDLE_PRIORITY + 3)


#define TASK_STACK_SIZE     (configMINIMAL_STACK_SIZE * 2)


static void prvSetupHardware(void);
static void prvCreateQueues(void);
static void prvCreateTimers(void);
static void prvCreateTasks(void);

/*-----------------------------------------------------------*/

int main(void)
{
    /* Initialize hardware */
    prvSetupHardware();
    Hardware_GPIO_Init();
    Hardware_ADC_Init();
    Hardware_ITM_Init();  /* Enable SWV/ITM for printf output */
    
    /* Debug prints temporarily disabled for testing */
    printf("=================================\n");
    printf("Traffic Light System Initializing\n");
    printf("=================================\n");
    
    /* Create queues for inter-task communication */
    prvCreateQueues();
    
    /* Create software timers for traffic light control */
    prvCreateTimers();
    
    /* Create tasks */
    prvCreateTasks();
    
    /* Clear traffic LEDs */
    Hardware_SPC_Load(0);
    
    //printf("Starting FreeRTOS scheduler...\n");
    
    /* Start the scheduler */
    vTaskStartScheduler();
    
    /* Should never reach here */
    printf("ERROR: Scheduler returned!\n");
    for(;;);
}

/*-----------------------------------------------------------*/
/* Create all queues for inter-task communication */
/*-----------------------------------------------------------*/
static void prvCreateQueues(void)
{
    /* 
     * xFlowToLightQueue: Single item queue (overwrite mode)
     * Holds latest flow rate for traffic light timing
     */
    xFlowToLightQueue = xQueueCreate(1, sizeof(uint16_t));
    if (xFlowToLightQueue == NULL)
    {
        printf("ERROR: Failed to create xFlowToLightQueue\n");
    }
    
    /*
     * xFlowToGeneratorQueue: Single item queue (overwrite mode)
     * Holds latest flow rate for traffic generation
     */
    xFlowToGeneratorQueue = xQueueCreate(1, sizeof(uint16_t));
    if (xFlowToGeneratorQueue == NULL)
    {
        printf("ERROR: Failed to create xFlowToGeneratorQueue\n");
    }
    
    /*
     * xCarQueue: Queue for new car data
     * Holds car generation data (car present = 1, no car = 0)
     */
    xCarQueue = xQueueCreate(10, sizeof(uint16_t));
    if (xCarQueue == NULL)
    {
        printf("ERROR: Failed to create xCarQueue\n");
    }
    
    /*
     * xLightStateQueue: Single item queue (overwrite mode)
     * Holds current traffic light state for display task
     */
    xLightStateQueue = xQueueCreate(1, sizeof(TrafficLightState_t));
    if (xLightStateQueue == NULL)
    {
        printf("ERROR: Failed to create xLightStateQueue\n");
    }
    
    /* Add queues to registry for kernel aware debugging */
    vQueueAddToRegistry(xFlowToLightQueue, "FlowToLight");
    vQueueAddToRegistry(xFlowToGeneratorQueue, "FlowToGenerator");
    vQueueAddToRegistry(xCarQueue, "CarQueue");
    vQueueAddToRegistry(xLightStateQueue, "LightState");
    
    printf("Queues created successfully.\n");
}

/*-----------------------------------------------------------*/
/* Create software timers for traffic light control */
/*-----------------------------------------------------------*/
static void prvCreateTimers(void)
{
    /*
     * Green Light Timer
     * One-shot timer, period adjusted dynamically based on flow rate
     * Initial period: 5000ms (will be updated by task)
     */
    xGreenLightTimer = xTimerCreate(
        "GreenTimer",
        pdMS_TO_TICKS(5000),
        pdFALSE,                    /* One-shot timer */
        (void *)0,
        vGreenLightTimerCallback
    );
    if (xGreenLightTimer == NULL)
    {
        printf("ERROR: Failed to create xGreenLightTimer\n");
    }
    
    /*
     * Yellow Light Timer
     * One-shot timer, constant period (2 seconds)
     */
    xYellowLightTimer = xTimerCreate(
        "YellowTimer",
        pdMS_TO_TICKS(2000),
        pdFALSE,                    /* One-shot timer */
        (void *)1,
        vYellowLightTimerCallback
    );
    if (xYellowLightTimer == NULL)
    {
        printf("ERROR: Failed to create xYellowLightTimer\n");
    }
    
    /*
     * Red Light Timer
     * One-shot timer, period adjusted dynamically based on flow rate
     * Initial period: 5000ms (will be updated by task)
     */
    xRedLightTimer = xTimerCreate(
        "RedTimer",
        pdMS_TO_TICKS(5000),
        pdFALSE,                    /* One-shot timer */
        (void *)2,
        vRedLightTimerCallback
    );
    if (xRedLightTimer == NULL)
    {
        printf("ERROR: Failed to create xRedLightTimer\n");
    }
    
    printf("Software timers created successfully.\n");
}

/*-----------------------------------------------------------*/
/* Create all tasks */
/*-----------------------------------------------------------*/
static void prvCreateTasks(void)
{
    BaseType_t xResult;
    
    /*
     * Traffic Flow Adjustment Task
     * Reads potentiometer and sends flow rate to other tasks
     */
    xResult = xTaskCreate(
        TrafficFlowAdjustTask,
        "FlowTask",
        TASK_STACK_SIZE,
        NULL,
        TRAFFIC_FLOW_TASK_PRIORITY,
        &xTrafficFlowTaskHandle
    );
    if (xResult != pdPASS)
    {
        printf("ERROR: Failed to create Traffic Flow Task\n");
    }
    
    /*
     * Traffic Generator Task
     * Generates random traffic based on flow rate
     */
    xResult = xTaskCreate(
        TrafficGeneratorTask,
        "GeneratorTask",
        TASK_STACK_SIZE,
        NULL,
        TRAFFIC_GENERATOR_TASK_PRIORITY,
        &xTrafficGeneratorTaskHandle
    );
    if (xResult != pdPASS)
    {
        printf("ERROR: Failed to create Traffic Generator Task\n");
    }
    
    /*
     * Traffic Light State Task
     * Controls traffic light timing using software timers
     */
    xResult = xTaskCreate(
        TrafficLightTask,
        "LightTask",
        TASK_STACK_SIZE,
        NULL,
        TRAFFIC_LIGHT_TASK_PRIORITY,
        &xTrafficLightTaskHandle
    );
    if (xResult != pdPASS)
    {
        printf("ERROR: Failed to create Traffic Light Task\n");
    }
    
    /*
     * System Display Task
     * Controls all LEDs and visualizes traffic
     */
    xResult = xTaskCreate(
        TrafficDisplayTask,
        "DisplayTask",
        TASK_STACK_SIZE,
        NULL,
        TRAFFIC_DISPLAY_TASK_PRIORITY,
        &xTrafficDisplayTaskHandle
    );
    if (xResult != pdPASS)
    {
        printf("ERROR: Failed to create Traffic Display Task\n");
    }
    
    printf("Tasks created successfully.\n");
}

/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

