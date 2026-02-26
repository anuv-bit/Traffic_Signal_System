/*
 * Traffic Flow Header
 * 
 * Traffic Flow Adjustment Task: Reads the potentiometer value and sends
 * it to other tasks via queues for traffic generation and light timing.
 */

#ifndef TRAFFICFLOW_H
#define TRAFFICFLOW_H

#include <stdint.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "hardware.h"

/* Queue handles - defined in main.c */
extern QueueHandle_t xFlowToLightQueue;      /* Flow rate to Traffic Light Task */
extern QueueHandle_t xFlowToGeneratorQueue;  /* Flow rate to Traffic Generator Task */

/* Task function */
void TrafficFlowAdjustTask(void *pvParameters);

#endif /* TRAFFICFLOW_H */
