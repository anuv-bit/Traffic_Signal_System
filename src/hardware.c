#include "hardware.h"
//#include "../config/CommonConfig.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_rcc.h"

/* ---------- Private delay for SPC timing ---------- */
static void SPC_Delay(void)
{
    for (volatile int i = 0; i < 100; i++);
}

/* ---------- GPIO Initialization ---------- */
void Hardware_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin =
        TRAFFIC_LIGHT_RED |
        TRAFFIC_LIGHT_YELLOW |
        TRAFFIC_LIGHT_GREEN |
        SHIFT_DATA |
        SHIFT_CLOCK |
        SHIFT_RESET;

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* Ensure known startup state */
    GPIO_ResetBits(GPIOC,
        TRAFFIC_LIGHT_RED |
        TRAFFIC_LIGHT_YELLOW |
        TRAFFIC_LIGHT_GREEN);
}

/* ---------- ADC Initialization ---------- */
void Hardware_ADC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef  ADC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    /* PC3 as analog input */
    GPIO_InitStructure.GPIO_Pin  = ADC_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    ADC_StructInit(&ADC_InitStructure);
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_Cmd(ADC1, ENABLE);
}

/* ---------- ADC Read ---------- */
uint16_t Hardware_ADC_Read(void)
{
    ADC_RegularChannelConfig(
        ADC1,
        ADC_Channel_13,
        1,
        ADC_SampleTime_3Cycles
    );

    ADC_SoftwareStartConv(ADC1);
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

    return ADC_GetConversionValue(ADC1);
}

/* ---------- Shift Register (SPC) ---------- */
void Hardware_SPC_Reset(void)
{
    GPIO_ResetBits(GPIOC, SHIFT_RESET);
    SPC_Delay();
    GPIO_SetBits(GPIOC, SHIFT_RESET);
}

void Hardware_SPC_ShiftBit(uint8_t bit)
{
    if (bit)
        GPIO_SetBits(GPIOC, SHIFT_DATA);
    else
        GPIO_ResetBits(GPIOC, SHIFT_DATA);

    SPC_Delay();

    GPIO_SetBits(GPIOC, SHIFT_CLOCK);
    SPC_Delay();
    GPIO_ResetBits(GPIOC, SHIFT_CLOCK);
}

void Hardware_SPC_Load(uint32_t pattern)
{
    Hardware_SPC_Reset();

    /* Shift 18 bits: MSB first */
    for (int i = 17; i >= 0; i--)
    {
        uint8_t bit = (pattern >> i) & 0x01;
        Hardware_SPC_ShiftBit(bit);
    }
}
