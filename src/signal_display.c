/*
 * Traffic Display Task Implementation
 * 
 * System display task: This task controls all LEDs in the system and is 
 * responsible for visualizing all vehicle traffic and traffic lights.
 * 
 * LED layout (19 LEDs total):
 * - Index 0-7: Before stop line (8 LEDs) - cars approach from index 0
 * - Index 8: Stop line position (9th LED, next to traffic light)
 * - Index 9-18: After stop line (10 LEDs) - cars exit toward index 18
 * 
 * Cars move from index 0 toward index 18.
 */

#include "signal_display.h"
#include <stdio.h>
#include <string.h>

/* Traffic array - each element is 1 (car) or 0 (empty) */
static uint16_t traffic[19] = {0};

/* Convert traffic array to bit pattern for shift register */
static uint32_t TrafficArrayToPattern(void)
{
    uint32_t pattern = 0;
    for (int i = 0; i < 19; i++)
    {
        if (traffic[i])
        {
            pattern |= (1UL << (18 - i));  /* Index 0 -> bit 18, Index 18 -> bit 0 */
        }
    }
    return pattern;
}

void TrafficDisplayTask(void *pvParameters)
{
    uint16_t newCar = 0;
    TrafficLightState_t lightState = LIGHT_STATE_GREEN;
    /* Display refresh rate: 500ms = 2 positions per second */
    const TickType_t xDisplayInterval = pdMS_TO_TICKS(500);
    TickType_t xLastWakeTime = xTaskGetTickCount();
    /* Clear all LEDs initially */
    memset(traffic, 0, sizeof(traffic));
    Hardware_SPC_Load(0);
    /* Wait for initial light state to be available */
    while (xQueuePeek(xLightStateQueue, &lightState, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        /* Wait for traffic light task to initialize */
    }
    while(1)
    {
        /* Get new car from creator task (non-blocking) */
        if (xQueueReceive(xCarQueue, &newCar, 0) != pdTRUE)
        {
            newCar = 0;
        }
        /* Get current light state */
        xQueuePeek(xLightStateQueue, &lightState, 0);
        if (lightState == LIGHT_STATE_GREEN)
        {
            /* GREEN: All cars move forward by one position */
            for (int i = 18; i > 0; i--)
            {
                traffic[i] = traffic[i - 1];
            }
            /* New car enters at position 0 */
            traffic[0] = newCar;
        }
        else if (lightState == LIGHT_STATE_RED || lightState == LIGHT_STATE_YELLOW)
        {
            /* RED/YELLOW: 
             * - Cars PAST stop line (9-18) continue moving and exit
             * - Cars AT/BEFORE stop line (0-8) bunch up toward stop line
             * - Car at position 8 (stop line) does NOT cross
             */
            /* Step 1: Shift cars PAST stop line forward (they keep moving) */
            for (int i = 18; i > 9; i--)
            {
                traffic[i] = traffic[i - 1];
            }
            /* Position 9 becomes empty - car at stop line does NOT cross */
            traffic[9] = 0;
            /* Step 2: Bunch up cars BEFORE stop line (positions 0-8) */
            uint16_t gap_found = 0;
            uint16_t next_traffic[9] = {0};
            for (int i = 8; i > 0; i--)
            {
                if (traffic[i] == 0)
                {
                    gap_found = 1;
                }
                if (gap_found)
                {
                    next_traffic[i] = traffic[i - 1];
                }
                else
                {
                    next_traffic[i] = traffic[i];
                }
            }
            if (gap_found)
            {
                next_traffic[0] = newCar;
            }
            else
            {
                next_traffic[0] = traffic[0];
            }
            for (int i = 0; i <= 8; i++)
            {
                traffic[i] = next_traffic[i];
            }
        }
        /* Convert array to bit pattern and update display */
        uint32_t pattern = TrafficArrayToPattern();
        Hardware_SPC_Load(pattern);
        vTaskDelayUntil(&xLastWakeTime, xDisplayInterval);
    }
}
