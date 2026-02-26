/*
 * Traffic Creator Task Implementation
 * 
 * Traffic generator task: This task randomly generates new traffic with a 
 * rate that is proportional to the potentiometer's value received from 
 * the Traffic Flow Adjustment Task.
 * 
 * Requirements:
 * - At maximum flow (7): bumper-to-bumper traffic (no gaps)
 * - At minimum flow (0): gap of ~5-6 LEDs between cars
 * 
 * Traffic is represented as 1 (car present) or 0 (no car).
 */

#include "trafficgenerator.h"
#include <stdio.h>

void TrafficGeneratorTask(void *pvParameters)
{
    uint16_t flow_rate = 4;  /* Default middle value */
    uint16_t car_value = 1;
    uint16_t random_threshold;
    
    /* 
     * Car generation runs at the same rate as display refresh
     * This creates 1-3 LEDs per second movement when combined with display
     */
    const TickType_t xGenerateInterval = pdMS_TO_TICKS(500);  /* 2 positions/sec */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    /* Seed random number generator */
    srand(xTaskGetTickCount());
    
    while(1)
    {
        /* Try to receive updated flow rate (non-blocking peek) */
        xQueuePeek(xFlowToGeneratorQueue, &flow_rate, 0);
        
        /*
         * Car generation probability based on flow rate (0-7):
         * 
         * Flow 7 (max): 100% chance - bumper to bumper (every cycle)
         * Flow 0 (min): ~12.5% chance - gap of 5-6 LEDs between cars
         * 
         * Formula: rand() % 8 < (flow_rate + 1)
         */
        random_threshold = rand() % 8;
        
        if (random_threshold < (flow_rate + 1))
        {
            /* Generate car - send 1 to display */
            car_value = 1;
            xQueueSend(xCarQueue, &car_value, 0);  /* Non-blocking send */
        }
        /* If no car generated, don't send anything - display handles empty queue */
        
        vTaskDelayUntil(&xLastWakeTime, xGenerateInterval);
    }
}
