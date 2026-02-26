/*
 * Traffic Display Header
 * 
 * System Display Task: Controls all LEDs in the system.
 * Visualizes vehicle traffic and traffic lights.
 */

#ifndef SIGNAL_DISPLAY_H
#define SIGNAL_DISPLAY_H

#include <stdint.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "hardware.h"
#include "trafficlights.h"

/* Number of LEDs for traffic display (19 green LEDs for cars) */
#define NUM_TRAFFIC_LEDS    19

/* Position of traffic stop line (bit 9 = 10th LED from entry/left) */
#define STOP_LINE_POSITION  9

/* Queue handles - defined in main.c */
extern QueueHandle_t xCarQueue;           /* Receives car data from creator */
extern QueueHandle_t xLightStateQueue;    /* Receives light state */

/* Task function */
void TrafficDisplayTask(void *pvParameters);

#endif /* SIGNAL_DISPLAY_H */
