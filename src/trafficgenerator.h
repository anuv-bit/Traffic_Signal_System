/*
 * Traffic Generator Header
 * 
 * Traffic Generator Task: Randomly generates new traffic with a rate 
 * proportional to the potentiometer's value.
 */

#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include <stdint.h>
#include <stdlib.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/queue.h"

/* Queue handles - defined in main.c */
extern QueueHandle_t xFlowToGeneratorQueue;  /* Receives flow rate */
extern QueueHandle_t xCarQueue;              /* Sends car data to display */

/* Task function */
void TrafficGeneratorTask(void *pvParameters);

#endif /* TRAFFIC_GENERATOR_H */
