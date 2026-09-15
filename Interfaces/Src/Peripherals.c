//
// Created by yan on 8/28/26.
//

#include "../Inc/Peripherals.h"

#include <stdbool.h>

#include "cmsis_os2.h"
#include "stm32l476xx.h"


// SPI interface

void ConfigSpiInterface()
{
    // Enable clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Config GPIO
    // Set AF Mode 5 for pins PA5, PA6, PA7

    // Set MODER
    GPIOA->MODER &= ~(GPIO_MODER_MODE5_Msk | GPIO_MODER_MODE6_Msk | GPIO_MODER_MODE7_Msk);
    GPIOA->MODER |= (GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);

    // Set AFRL
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5_Msk | GPIO_AFRL_AFSEL6_Msk | GPIO_AFRL_AFSEL7_Msk);
    GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_0) | // AF 5 -> SPI1_SCK
        (GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL6_0) | // AF 5 -> SPI1_MISO
        (GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_0); // AF 5 -> SPI1_MOSI

    // Set speed for SPI pins
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED5_1 | GPIO_OSPEEDR_OSPEED6_1 | GPIO_OSPEEDR_OSPEED7_1);

    // Set PA8 to output for chip select

    // Set MODER
    GPIOA->MODER &= ~(GPIO_MODER_MODE8_Msk);
    GPIOA->MODER |= GPIO_MODER_MODE8_0; // Set to output mode

    // Set OSPEEDR
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED8_Msk);
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_0; // Set to very high speed
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_1;

    // Set PUPDR
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8; // No pull-up / no pull-down
    //GPIOA->PUPDR |= GPIO_PUPDR_PUPD8_1; // Set du pull-up

    // Configure SPI

    SPI1->CR1 &= ~SPI_CR1_SPE; // Disable SPI interface

    SPI1->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0; // Set baudrate to clock/16
    SPI1->CR1 |= SPI_CR1_MSTR; // Set to master
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 &= ~SPI_CR1_CPOL_Msk; // Clock polarity -> Set clock to 0 when idle
    SPI1->CR1 &= ~SPI_CR1_CPHA_Msk; // Clock phase -> The first clock transition is the first data capture edge

    SPI1->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2); // Set data size to 8 bit
    SPI1->CR2 |= SPI_CR2_FRXTH; // Set FIFO reception threshold to 8 bits

    // Configure DMA channels - DMA1 CH2 DMA Rx / DMA1 CH3 DMA Tx

    // Select channels - Link DMA function to channel
    DMA1_CSELR->CSELR &= ~(DMA_CSELR_C2S_Msk | DMA_CSELR_C3S_Msk);
    DMA1_CSELR->CSELR |= (01 << DMA_CSELR_C2S_Pos);
    DMA1_CSELR->CSELR |= (01 << DMA_CSELR_C3S_Pos);

    // Channel 2 RX
    DMA1_Channel2->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel2->CCR = 0;
    DMA1_Channel2->CCR &= ~(DMA_CCR_PSIZE_Msk);
    DMA1_Channel2->CCR |= DMA_CCR_MINC;
    DMA1_Channel2->CCR &= ~(DMA_CCR_DIR);
    DMA1_Channel2->CCR |= DMA_CCR_TCIE;

    // Channel 3 TX
    DMA1_Channel3->CPAR = (uint32_t)&SPI1->DR;
    DMA1_Channel3->CCR = 0;
    DMA1_Channel3->CCR &= ~(DMA_CCR_PSIZE_Msk);
    DMA1_Channel3->CCR |= DMA_CCR_MINC;
    DMA1_Channel3->CCR |= DMA_CCR_DIR;
    //DMA1_Channel3->CCR |= DMA_CCR_TCIE; -> not used, we just want to receive data


    SPI1->CR1 |= SPI_CR1_SPE; // Enable SPI interface

    NVIC_SetPriority(DMA1_Channel2_IRQn, 5);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);


};

// Macros for CS

#define DEVICE_CS_ON() (GPIOA->BSRR |= GPIO_BSRR_BR8)
#define DEVICE_CS_OFF() (GPIOA->BSRR |= GPIO_BSRR_BS8)


// Transfer SPI data
uint8_t TransferSpiDataPolling(uint8_t data)
{

    while (SPI1->SR & SPI_SR_BSY)
    {
    };
    while (!(SPI1->SR & SPI_SR_TXE)) // Wait for send buffer empty
    {
    };

    *(__IO uint8_t*)&SPI1->DR = data; // Force to send 8 bit

    while (!(SPI1->SR & SPI_SR_RXNE)) // Wait until receive buff is not empty
    {
    };
           return (*(__IO uint8_t*)&SPI1->DR); // Force 8 bit read from register
}

void TransferSpiDataDMA(uint8_t *rx_data, uint8_t *tx_data, uint8_t reg)
{

    uint16_t dummyData = 0;

    // Set register address to transmit data
    tx_data[0] = reg |= 0x80; // Set bit 7 always to true

    dummyData = SPI1->DR;

    SPI1->CR1 &= ~SPI_CR1_SPE; // Disable SPI interface
    // Configure DMA for SPI1
    SPI1->CR2 |= SPI_CR2_TXDMAEN;  // Activate DMA for transmit
    SPI1->CR2 |= SPI_CR2_RXDMAEN; // Activate DMA for receive
    SPI1->CR1 |= SPI_CR1_SPE; // Enable SPI interface


    // Prepare DMA rx channel 2
    DMA1_Channel2->CCR &= ~DMA_CCR_EN; // Disable channel 2
    DMA1_Channel2->CMAR = (uint32_t)rx_data; // Set memory address
    DMA1_Channel2->CNDTR = 7; // Set number of bytes o transfer
    DMA1_Channel2->CCR |= DMA_CCR_EN; // Enable channel 2

    // Prepare DMA rx channel 3
    DMA1_Channel3->CCR &= ~DMA_CCR_EN; // Disable channel 3
    DMA1_Channel3->CMAR = (uint32_t)tx_data; // Set memory address
    DMA1_Channel3->CNDTR = 7; // Set number of bytes o transfer

    DEVICE_CS_ON(); // Enable sensor

    DMA1_Channel3->CCR |= DMA_CCR_EN; // Enable channel 3 -> start transmission

    uint32_t flags = osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);

    DEVICE_CS_OFF(); // Disable sensor

    DMA1->IFCR = DMA_IFCR_CTCIF3;

};

void WriteSpiData(uint8_t reg, uint8_t data)
{
    reg &= ~0x80; // Set bit 7 always false for write operation
    DEVICE_CS_ON();
    TransferSpiDataPolling(reg);
    TransferSpiDataPolling(data);
    DEVICE_CS_OFF();
}

void ReadSpiData(uint8_t reg, uint8_t count, uint8_t* data)
{
    reg |= 0x80; // Set bit 7 always true for read operation
    DEVICE_CS_ON();
        TransferSpiDataPolling(reg);
    for (uint8_t i = 0; i < count; i++)
    {
        *data = TransferSpiDataPolling(0x00);
        data++;
    }
    DEVICE_CS_OFF();
}


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

#define LCD_WAIT_TICKS 1

#define LCD_WAIT_TIME 5000

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
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN;
}

void WaitTimeLcd(uint8_t time)
{
    // time --> ms
    // Initialize timer

    TIM6->PSC = (4-1);
    TIM6->ARR = (time-1);

    // Update flag active

    TIM6->EGR |= TIM_EGR_UG;
    TIM6->SR &= ~TIM_SR_UIF;


    // Start timer
    TIM6->CNT = 0;
    TIM6->CR1 |= TIM_CR1_CEN;


    while(!(TIM6->SR & TIM_SR_UIF))
    {

    };

    TIM6->SR &= ~ TIM_SR_UIF;
    TIM6->CR1 &= ~TIM_CR1_CEN;
}

void SendLcdInstruction4Bit(uint8_t instruction)
{
    SetLcdPin(LCD_DB4_PORT, LCD_DB4_PIN, 0b0001 & (instruction >> 4));
    SetLcdPin(LCD_DB5_PORT, LCD_DB5_PIN, 0b0010 & (instruction >> 4));
    SetLcdPin(LCD_DB6_PORT, LCD_DB6_PIN, 0b0100 & (instruction >> 4));
    SetLcdPin(LCD_DB7_PORT, LCD_DB7_PIN, 0b1000 & (instruction >> 4));
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, true);
    osDelay(LCD_WAIT_TICKS);
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, false);
    ResetLcdPins();
    SetLcdPin(LCD_DB4_PORT, LCD_DB4_PIN, 0b00010000 & (instruction << 4));
    SetLcdPin(LCD_DB5_PORT, LCD_DB5_PIN, 0b00100000 & (instruction << 4));
    SetLcdPin(LCD_DB6_PORT, LCD_DB6_PIN, 0b01000000 & (instruction << 4));
    SetLcdPin(LCD_DB7_PORT, LCD_DB7_PIN, 0b10000000 & (instruction << 4));
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, true);
    osDelay(LCD_WAIT_TICKS);
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, false);
    ResetLcdPins();
    osDelay(LCD_WAIT_TICKS);
};

void SendLcdInstruction8Bit(uint8_t instruction)
{

    SetLcdPin(LCD_DB4_PORT, LCD_DB4_PIN, 0b0001 & (instruction >> 4));
    SetLcdPin(LCD_DB5_PORT, LCD_DB5_PIN, 0b0010 & (instruction >> 4));
    SetLcdPin(LCD_DB6_PORT, LCD_DB6_PIN, 0b0100 & (instruction >> 4));
    SetLcdPin(LCD_DB7_PORT, LCD_DB7_PIN, 0b1000 & (instruction >> 4));
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, true);
    osDelay(LCD_WAIT_TICKS);
    SetLcdPin(LCD_E_PORT, LCD_E_PIN, false);
    ResetLcdPins();
    osDelay(LCD_WAIT_TICKS);
};

void InitializeLcdScreen()
{
    osDelay(200);

    SendLcdInstruction8Bit(LCD_INITIALIZE); // Initialize
    SendLcdInstruction8Bit(LCD_INITIALIZE); // Initialize
    SendLcdInstruction8Bit(LCD_INITIALIZE); // Initialize
    SendLcdInstruction8Bit(LCD_4_PIN_OPERATION); // Set to 4 bis operation

    SendLcdInstruction4Bit(LCD_4BIT_2COL_5X8CHARS); // 4 Bit, 2 columns, 5×8 chars
    SendLcdInstruction4Bit(LCD_DISPLAY_ON_CURSOR_OFF); // Display on, cursor off
    SendLcdInstruction4Bit(LCD_DISPLAY_CLEAR); // Display clear
    SendLcdInstruction4Bit(LCD_CURSOR_AUTO); // Cursor auto
}

void SendLcdChar(char data)
{
    SetLcdPin(LCD_RS_PORT, LCD_RS_PIN, true);
    SendLcdInstruction4Bit(data);
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


    SendLcdInstruction4Bit(0b10000000 + position);

    ResetLcdPins();
};
