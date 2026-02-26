/*
 * Traffic Light Header
 * 
 * Traffic Light State Task: Controls the timing of the traffic lights 
 * using software timers. Timing is affected by the traffic flow rate.
 */

#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <stdint.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/timers.h"
#include "stm32f4xx_gpio.h"
#include "hardware.h"

/* Traffic light states */
typedef enum {
    LIGHT_STATE_GREEN = 0,
    LIGHT_STATE_YELLOW = 1,
    LIGHT_STATE_RED = 2
} TrafficLightState_t;

/* Queue handles - defined in main.c */
extern QueueHandle_t xFlowToLightQueue;   /* Receives flow rate */
extern QueueHandle_t xLightStateQueue;    /* Sends light state to display task */

/* Software timer handles - defined in main.c */
extern TimerHandle_t xGreenLightTimer;
extern TimerHandle_t xYellowLightTimer;
extern TimerHandle_t xRedLightTimer;

/* Timer callback functions */
void vGreenLightTimerCallback(TimerHandle_t xTimer);
void vYellowLightTimerCallback(TimerHandle_t xTimer);
void vRedLightTimerCallback(TimerHandle_t xTimer);

/* Task function */
void TrafficLightTask(void *pvParameters);

/* Initialize traffic light hardware */
void TrafficLight_Init(void);

/* Set traffic light color */
void TrafficLight_SetGreen(void);
void TrafficLight_SetYellow(void);
void TrafficLight_SetRed(void);

#endif /* TRAFFIC_LIGHT_H */
