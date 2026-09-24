//
// Created by yan on 8/28/26.
//

#include "../Inc/Spi.h"



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
    //GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8; // No pull-up / no pull-down
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD8_1; // Set du pull-up

    // Configure SPI

    SPI1->CR1 &= ~SPI_CR1_SPE; // Disable SPI interface

    SPI1->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0; // Set baudrate to clock/16
    SPI1->CR1 |= SPI_CR1_MSTR; // Set to master
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1->CR1 &= ~SPI_CR1_CPOL_Msk; // Clock polarity -> Set clock to 0 when idle
    SPI1->CR1 &= ~SPI_CR1_CPHA_Msk; // Clock phase -> The first clock transition is the first data capture edge

    SPI1->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2); // Set data size to 8 bit
    SPI1->CR2 |= SPI_CR2_FRXTH; // Set FIFO reception threshold to 8 bits
    SPI1->CR2 |= SPI_CR2_ERRIE; // Enable errors

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
    DMA1_Channel2->CCR |= DMA_CCR_TCIE; // Enable transfer complete interrupt
    DMA1_Channel2->CCR |= DMA_CCR_TEIE; // Enable transfer error interrupt

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

    NVIC_SetPriority(SPI1_IRQn, 5);
    NVIC_EnableIRQ(SPI1_IRQn);
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

void TransferSpiDataDMA(uint8_t* rx_data, uint8_t* tx_data, uint8_t reg)
{
    uint16_t dummyData = 0;

    // Set register address to transmit data
    tx_data[0] = reg |= 0x80; // Set bit 7 always to true

    dummyData = SPI1->DR;

    SPI1->CR1 &= ~SPI_CR1_SPE; // Disable SPI interface
    // Configure DMA for SPI1
    SPI1->CR2 |= SPI_CR2_TXDMAEN; // Activate DMA for transmit
    SPI1->CR2 |= SPI_CR2_RXDMAEN; // Activate DMA for receive
    SPI1->CR1 |= SPI_CR1_SPE; // Enable SPI interface


    // Prepare DMA rx channel 2
    DMA1_Channel2->CCR &= ~DMA_CCR_EN; // Disable channel 2
    DMA1_Channel2->CMAR = (uint32_t)rx_data; // Set memory address
    DMA1_Channel2->CNDTR = 7; // Set number of bytes to transfer
    DMA1_Channel2->CCR |= DMA_CCR_EN; // Enable channel 2

    // Prepare DMA rx channel 3
    DMA1_Channel3->CCR &= ~DMA_CCR_EN; // Disable channel 3
    DMA1_Channel3->CMAR = (uint32_t)tx_data; // Set memory address
    DMA1_Channel3->CNDTR = 7; // Set number of bytes to transfer

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
    uint8_t dummy_data = 0;
    reg |= 0x80; // Set bit 7 always true for read operation
    DEVICE_CS_ON();
    dummy_data = TransferSpiDataPolling(reg);
    for (uint8_t i = 0; i < count; i++)
    {
        *data = TransferSpiDataPolling(0x00);
        data++;
    }
    DEVICE_CS_OFF();
}



