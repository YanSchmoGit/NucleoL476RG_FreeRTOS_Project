//
// Created by yan on 9/22/26.
//

#include "../Inc/Lcd.h"
#include "stm32l476xx.h"
#include <stdbool.h>


// Config LCD screen

#define LCD_DB4_PIN 0
#define LCD_DB4_PORT GPIOC
#define LCD_DB5_PIN 1
#define LCD_DB5_PORT GPIOC
#define LCD_DB6_PIN 2
#define LCD_DB6_PORT GPIOC
#define LCD_DB7_PIN 3
#define LCD_DB7_PORT GPIOC

#define LCD_E_PIN 10
#define LCD_E_PORT GPIOC
#define LCD_RS_PIN 11
#define LCD_RS_PORT GPIOC
#define LCD_RW_PIN 12
#define LCD_RW_PORT GPIOC

#define LCD_WAIT_TIME 5


#define LCD_INITIALIZE 0b00110000
#define LCD_4_PIN_OPERATION 0b00100000
#define LCD_4BIT_2COL_5X8CHARS 0b00101000
#define LCD_DISPLAY_ON_CURSOR_OFF 0b00001110
#define LCD_DISPLAY_CLEAR 0b00000001
#define LCD_CURSOR_AUTO 0b00000110

void ConfigLcdScreen()
{
    // Enable clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // Set MODER
    GPIOC->MODER &= ~(GPIO_MODER_MODE0_Msk | GPIO_MODER_MODE1_Msk | GPIO_MODER_MODE2_Msk | GPIO_MODER_MODE3_Msk |
        GPIO_MODER_MODE10_Msk | GPIO_MODER_MODE11_Msk | GPIO_MODER_MODE12_Msk);

    GPIOC->MODER |= (GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 |
        GPIO_MODER_MODE10_0 | GPIO_MODER_MODE11_0 | GPIO_MODER_MODE12_0); // Set to output mode

    // Set OSPEEDR
    GPIOC->OSPEEDR |= (GPIO_OSPEEDR_OSPEED0_0 | GPIO_OSPEEDR_OSPEED1_0 | GPIO_OSPEEDR_OSPEED2_0 | GPIO_OSPEEDR_OSPEED3_0
        | GPIO_OSPEEDR_OSPEED10_0 | GPIO_OSPEEDR_OSPEED11_0 | GPIO_OSPEEDR_OSPEED12_0); // Set output speed to high

    // Set PUDPR
    GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD0_Msk | GPIO_PUPDR_PUPD1_Msk | GPIO_PUPDR_PUPD2_Msk | GPIO_PUPDR_PUPD3_Msk |
        GPIO_PUPDR_PUPD10_Msk | GPIO_PUPDR_PUPD11_Msk | GPIO_PUPDR_PUPD12_Msk); // No pull-up / no pull-down

    EnableLcdTimer();
}

void SetLcdPin(GPIO_TypeDef* GPIOx, uint8_t Pin, bool Value)
{
    if (Value)
    {
        GPIOx->BSRR |= 1 << Pin;
    }
    else
    {
        GPIOx->BRR |= 1 << Pin;
    }
};

void ResetLcdPins()
{
    SetLcdPin(LCD_DB4_PORT, LCD_DB4_PIN, false);
    SetLcdPin(LCD_DB5_PORT, LCD_DB5_PIN, false);
    SetLcdPin(LCD_DB6_PORT, LCD_DB6_PIN, false);
    SetLcdPin(LCD_DB7_PORT, LCD_DB7_PIN, false);
}

void EnableLcdTimer()
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN;

    TIM7->CR1 &= ~TIM_CR1_CEN;
    TIM7->PSC = (80 - 1);
    TIM7->ARR = (0xFFFF); // Max. value

    TIM7->EGR |= TIM_EGR_UG;
}

void WaitTimeLcd_us(uint16_t time_us)
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

void WaitTimeLcd_ms(uint16_t time_ms)
{
    while (time_ms--)
    {
        WaitTimeLcd_us(1000);
    };


}

void SendLcdInstructionByte(uint8_t instruction)
{
    SetLcdPin(LCD_DB4_PORT, LCD_DB4_PIN, 0b0001 & (instruction >> 4));
    SetLcdPin(LCD_DB5_PORT, LCD_DB5_PIN, 0b0010 & (instruction >> 4));
    SetLcdPin(LCD_DB6_PORT, LCD_DB6_PIN, 0b0100 & (instruction >> 4));
    SetLcdPin(LCD_DB7_PORT, LCD_DB7_PIN, 0b1000 & (instruction >> 4));
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, true);
    WaitTimeLcd_ms(LCD_WAIT_TIME);
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, false);
    ResetLcdPins();
    SetLcdPin(LCD_DB4_PORT, LCD_DB4_PIN, 0b00010000 & (instruction << 4));
    SetLcdPin(LCD_DB5_PORT, LCD_DB5_PIN, 0b00100000 & (instruction << 4));
    SetLcdPin(LCD_DB6_PORT, LCD_DB6_PIN, 0b01000000 & (instruction << 4));
    SetLcdPin(LCD_DB7_PORT, LCD_DB7_PIN, 0b10000000 & (instruction << 4));
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, true);
    WaitTimeLcd_ms(LCD_WAIT_TIME);
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, false);
    ResetLcdPins();
    WaitTimeLcd_ms(LCD_WAIT_TIME);
};

void SendLcdInstructionNibble(uint8_t instruction)
{
    SetLcdPin(LCD_DB4_PORT, LCD_DB4_PIN, 0b0001 & (instruction >> 4));
    SetLcdPin(LCD_DB5_PORT, LCD_DB5_PIN, 0b0010 & (instruction >> 4));
    SetLcdPin(LCD_DB6_PORT, LCD_DB6_PIN, 0b0100 & (instruction >> 4));
    SetLcdPin(LCD_DB7_PORT, LCD_DB7_PIN, 0b1000 & (instruction >> 4));
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, true);
    WaitTimeLcd_ms(5);
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, false);
    ResetLcdPins();

};

void InitializeLcdScreen()
{
    SendLcdInstructionNibble(LCD_INITIALIZE); // Initialize
    SendLcdInstructionNibble(LCD_INITIALIZE); // Initialize
    SendLcdInstructionNibble(LCD_INITIALIZE); // Initialize
    SendLcdInstructionNibble(LCD_4_PIN_OPERATION); // Set to 4 bis operation

    SendLcdInstructionByte(LCD_4BIT_2COL_5X8CHARS); // 4 Bit, 2 columns, 5×8 chars
    SendLcdInstructionByte(LCD_DISPLAY_ON_CURSOR_OFF); // Display on, cursor off
    SendLcdInstructionByte(LCD_DISPLAY_CLEAR); // Display clear
    SendLcdInstructionByte(LCD_CURSOR_AUTO); // Cursor auto
}

void SendLcdChar(char data)
{
    SetLcdPin(LCD_RS_PORT, LCD_RS_PIN, true);
    SendLcdInstructionByte(data);
    SetLcdPin(LCD_RS_PORT, LCD_RS_PIN, false);
}

void SendLcdString(char* data)
{
    while (*data != '\0')
    {
        SendLcdChar(*data++);
    }
}

uint8_t LengthNumber(uint32_t number)
{
    uint8_t count = 0;
    while (number > 0)
    {
        count++;
        number = number / 10;
    }

    return count;
}

void SendLcdInteger(uint32_t data)
{
    uint8_t character[LengthNumber(data) + 1];
    uint8_t i = sizeof(character) - 1;

    // Fill array from top to bottom

    character[i] = '\0';

    if (data == 0)
    {
        character[--i] = '0';
    }
    else
    {
        while (data > 0)
        {
            character[--i] = (data % 10) + '0';
            data = data / 10;
        }
    }

    SendLcdString((char*)character);
}

void SetLcdCursorPosition(uint8_t x, uint8_t y)
{
    // calculate the position on the display

    uint8_t position = x + (64 * y);


    SendLcdInstructionByte(0b10000000 + position);

    ResetLcdPins();
};