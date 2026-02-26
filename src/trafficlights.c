/*
 * Traffic Light Task Implementation
 * 
 * Traffic light state task: This task controls the timing of the traffic 
 * lights and outputs its current state via a queue (NO global variables).
 * 
 * Requirements:
 * - Green duration: proportional to traffic flow rate
 * - Red duration: inversely proportional to traffic flow rate
 * - Yellow duration: constant
 * 
 * At maximum flow (7):
 *   - Green ~10 seconds, Red ~5 seconds (green 2x longer than red)
 * 
 * At minimum flow (0):
 *   - Green ~5 seconds, Red ~10 seconds (red 2x longer than green)
 * 
 * Yellow: constant 2 seconds
 */

#include "trafficlights.h"
#include <stdio.h>
#include <stdlib.h>

/* Current light state - used by timer callbacks */
static volatile TrafficLightState_t currentLightState = LIGHT_STATE_GREEN;
static volatile uint16_t currentFlowRate = 3;  /* Default middle value */

/* Timing constants (milliseconds) */
#define YELLOW_DURATION_MS      2000    /* Constant 2 seconds */
#define GREEN_MIN_DURATION_MS   4000    /* Minimum green at flow 0 */
#define GREEN_MAX_DURATION_MS   8000    /* Maximum green at flow 7 */
#define RED_MIN_DURATION_MS     4000    /* Minimum red at flow 7 */
#define RED_MAX_DURATION_MS     8000    /* Maximum red at flow 0 */

/*
 * Calculate green light duration based on flow rate
 * Flow 0: 4000ms, Flow 7: 8000ms
 * At max flow (7): green=8s, red=4s (2:1 ratio)
 * At min flow (0): green=4s, red=8s (1:2 ratio)
 */
static uint32_t CalculateGreenDuration(uint16_t flow_rate)
{
    return GREEN_MIN_DURATION_MS + (flow_rate * (GREEN_MAX_DURATION_MS - GREEN_MIN_DURATION_MS) / 7);
}

/*
 * Calculate red light duration based on flow rate
 * Flow 0: 8000ms, Flow 7: 3000ms (inversely proportional)
 * Formula: 8000 - (flow_rate * 714)
 */
static uint32_t CalculateRedDuration(uint16_t flow_rate)
{
    return RED_MAX_DURATION_MS - (flow_rate * (RED_MAX_DURATION_MS - RED_MIN_DURATION_MS) / 7);
}

/* Set traffic light to green */
void TrafficLight_SetGreen(void)
{
    GPIO_ResetBits(GPIOC, TRAFFIC_LIGHT_RED | TRAFFIC_LIGHT_YELLOW);
    GPIO_SetBits(GPIOC, TRAFFIC_LIGHT_GREEN);
}

/* Set traffic light to yellow */
void TrafficLight_SetYellow(void)
{
    GPIO_ResetBits(GPIOC, TRAFFIC_LIGHT_RED | TRAFFIC_LIGHT_GREEN);
    GPIO_SetBits(GPIOC, TRAFFIC_LIGHT_YELLOW);
}

/* Set traffic light to red */
void TrafficLight_SetRed(void)
{
    GPIO_ResetBits(GPIOC, TRAFFIC_LIGHT_GREEN | TRAFFIC_LIGHT_YELLOW);
    GPIO_SetBits(GPIOC, TRAFFIC_LIGHT_RED);
}

/* Initialize traffic light - start with green */
void TrafficLight_Init(void)
{
    TrafficLight_SetGreen();
    currentLightState = LIGHT_STATE_GREEN;
}

/* Green light timer callback - switch to yellow */
void vGreenLightTimerCallback(TimerHandle_t xTimer)
{
    TrafficLightState_t newState = LIGHT_STATE_YELLOW;
    
    TrafficLight_SetYellow();
    currentLightState = LIGHT_STATE_YELLOW;
    
    /* Send new state to display task via queue */
    xQueueOverwrite(xLightStateQueue, &newState);
    
    /* Start yellow timer */
    xTimerStart(xYellowLightTimer, 0);
}

/* Yellow light timer callback - switch to red */
void vYellowLightTimerCallback(TimerHandle_t xTimer)
{
    TrafficLightState_t newState = LIGHT_STATE_RED;
    
    TrafficLight_SetRed();
    currentLightState = LIGHT_STATE_RED;
    
    /* Send new state to display task via queue */
    xQueueOverwrite(xLightStateQueue, &newState);
    
    /* Update red timer period based on current flow rate */
    uint32_t redDuration = CalculateRedDuration(currentFlowRate);
    xTimerChangePeriod(xRedLightTimer, pdMS_TO_TICKS(redDuration), 0);
    
    /* Start red timer */
    xTimerStart(xRedLightTimer, 0);
}

/* Red light timer callback - switch to green */
void vRedLightTimerCallback(TimerHandle_t xTimer)
{
    TrafficLightState_t newState = LIGHT_STATE_GREEN;
    
    TrafficLight_SetGreen();
    currentLightState = LIGHT_STATE_GREEN;
    
    /* Send new state to display task via queue */
    xQueueOverwrite(xLightStateQueue, &newState);
    
    /* Update green timer period based on current flow rate */
    uint32_t greenDuration = CalculateGreenDuration(currentFlowRate);
    xTimerChangePeriod(xGreenLightTimer, pdMS_TO_TICKS(greenDuration), 0);
    
    /* Start green timer */
    xTimerStart(xGreenLightTimer, 0);
}

/* Traffic Light Task - monitors flow rate and adjusts timing */
void TrafficLightTask(void *pvParameters)
{
    uint16_t newFlowRate = 3;
    TrafficLightState_t initialState = LIGHT_STATE_GREEN;
    
    /* Initialize traffic light to green */
    TrafficLight_Init();
    
    /* Send initial state to display task */
    xQueueOverwrite(xLightStateQueue, &initialState);
    
    /* Calculate initial durations */
    uint32_t greenDuration = CalculateGreenDuration(currentFlowRate);
    
    /* Update timer period and start */
    xTimerChangePeriod(xGreenLightTimer, pdMS_TO_TICKS(greenDuration), 0);
    xTimerStart(xGreenLightTimer, 0);
    
    while(1)
    {
        /* Check for updated flow rate from Traffic Flow Task */
        if (xQueuePeek(xFlowToLightQueue, &newFlowRate, pdMS_TO_TICKS(200)) == pdTRUE)
        {
            if (newFlowRate != currentFlowRate)
            {
                currentFlowRate = newFlowRate;
                
                /* 
                 * Timer periods will be updated when the respective timers
                 * are started in their callback functions. This ensures
                 * smooth transitions without interrupting current light phase.
                 */
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
