#ifndef HARDWARE_H
#define HARDWARE_H
/* Traffic lights */
#define TRAFFIC_LIGHT_RED    GPIO_Pin_0   // PC0
#define TRAFFIC_LIGHT_YELLOW GPIO_Pin_1   // PC1
#define TRAFFIC_LIGHT_GREEN  GPIO_Pin_2   // PC2

/* Shift register */
#define SHIFT_RESET  GPIO_Pin_8   // PC8
#define SHIFT_CLOCK  GPIO_Pin_7   // PC7
#define SHIFT_DATA   GPIO_Pin_6   // PC6

/* ADC pin */
#define ADC_PIN      GPIO_Pin_3   // PC3


#include <stdint.h>

/* GPIO initialization */
void Hardware_GPIO_Init(void);

/* ADC functions */
void Hardware_ADC_Init(void);
uint16_t Hardware_ADC_Read(void);

/* Shift register (SPC) functions */
void Hardware_SPC_Reset(void);
void Hardware_SPC_ShiftBit(uint8_t bit);
void Hardware_SPC_Load(uint32_t pattern);

/* ITM/SWV Debug Printf initialization */
void Hardware_ITM_Init(void);

#endif
