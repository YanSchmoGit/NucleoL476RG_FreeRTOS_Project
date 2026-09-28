//
// Created by yan on 9/28/26.
//

#include "../Inc/Utilities.h"

#include "stm32l476xx.h"


void EnableLcdTimer()
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;

    TIM7->CR1 &= ~TIM_CR1_CEN;
    TIM7->PSC = (80 - 1);
    TIM7->ARR = (0xFFFF); // Max. value

    TIM7->EGR |= TIM_EGR_UG;
}

void WaitTime_us(uint16_t time_us)
{
    // time --> us

    // Start timer
    TIM7->CNT = 0;
    TIM7->CR1 |= TIM_CR1_CEN;

    while (TIM7->CNT < time_us)
    {
    };

    TIM7->CR1 &= ~TIM_CR1_CEN;
};

void WaitTime_ms(uint16_t time_ms)
{
    while (time_ms--)
    {
        WaitTime_us(1000);
    };


}