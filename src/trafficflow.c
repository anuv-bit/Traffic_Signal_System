/*
 * Traffic Flow Task Implementation
 * 
 * Traffic flow adjustment task: The traffic flow that enters the intersection 
 * is set by a potentiometer. This task reads the value of the potentiometer 
 * at an appropriate interval and sends its value to other tasks via queues.
 * 
 * Low potentiometer resistance = light traffic
 * High potentiometer resistance = heavy traffic
 * 
 * ADC value range: 0-4095 (12-bit)
 * Converted to flow rate: 0-7 (8 levels)
 */

#include "trafficflow.h"
#include <stdio.h>

void TrafficFlowAdjustTask(void *pvParameters)
{
    uint16_t adc_value = 0;
    uint16_t flow_rate = 0;
    uint16_t prev_flow_rate = 0xFF;  /* Invalid initial value to force first send */
    
    const TickType_t xReadInterval = pdMS_TO_TICKS(100);  /* Read ADC every 100ms */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    
    while(1)
    {
        /* Read ADC value from potentiometer */
        adc_value = Hardware_ADC_Read();
        
        /* Convert 12-bit ADC (0-4095) to 8 levels (0-7) */
        flow_rate = adc_value / 512;
        if (flow_rate > 7)
        {
            flow_rate = 7;
        }
        
        /* Only send if flow rate changed to reduce queue traffic */
        if (flow_rate != prev_flow_rate)
        {
            /* Send to Traffic Light Task queue */
            xQueueOverwrite(xFlowToLightQueue, &flow_rate);
            
            /* Send to Traffic Generator Task queue */
            xQueueOverwrite(xFlowToGeneratorQueue, &flow_rate);
            
            prev_flow_rate = flow_rate;
        }
        
        /* Delay until next read interval */
        vTaskDelayUntil(&xLastWakeTime, xReadInterval);
    }
}
