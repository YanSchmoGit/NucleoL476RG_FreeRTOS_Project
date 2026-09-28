//
// Created by yan on 7/26/26.
//

#include "readSensorData_task.h"

#include "cmsis_os2.h"
#include "queues.h"
#include "stm32l476xx.h"
#include "showErrorData_task.h"
#include "../../Interfaces/Inc/Spi.h"
#include "../../Interfaces/Inc/BMP280.h"


/* Definitions for readSensorData */
osThreadId_t readSensorDataHandle;
const osThreadAttr_t readSensorData_attributes = {
    .name = "readSensorData",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

void startReadSensorData(void* argument);

void createTaskReadSensorData(void)
{
    /* creation of readSensorData */
    readSensorDataHandle = osThreadNew(startReadSensorData, NULL, &readSensorData_attributes);
}


// Task loop
void startReadSensorData(void* argument)
{
    static volatile uint8_t tx_data[7];
    static volatile uint8_t rx_data[7];

    static BMP280Values BMP280ValueData;
    /* Infinite loop */
    for (;;)
    {
        TransferSpiDataDMA(rx_data, tx_data, BMP280_REGISTER_PRESS_MSB);
        ProcessSensorData(&BMP280ValueData, rx_data);

        // check sensor data
        if (CheckSensorData(&BMP280ValueData, 4000, 110000, 0, 0) == 1)
        {
            // Bad values
            osThreadFlagsSet(showErrorDataHandle, ERROR_HANDLE_VALUES_OUT_OF_BOUNDS);
        }
        else
        {
            // Good values
            osMessageQueuePut(sensorDataHandle, &BMP280ValueData, 0, osWaitForever);
        }


        osDelay(10);
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    // Check if transfer complete is set for DMA 2 channel
    if (DMA1->ISR & DMA_ISR_TCIF2)
    {
        DMA1->IFCR = DMA_IFCR_CTCIF2; // delete interrupt flag

        if (readSensorDataHandle != NULL)
        {
            osThreadFlagsSet(readSensorDataHandle, 0x01);
        }
    }

    // Check if error ist set for DMA 2 channel
    if (DMA1->ISR & DMA_ISR_TEIF2)
    {
        DMA1->IFCR = DMA_IFCR_CTEIF2;

        osThreadFlagsSet(showErrorDataHandle, ERROR_HANDLE_DMA1_TRANSFER_ERROR);
    }
}


void SPI1_IRQHandler(void)
{
    if (SPI1->SR & SPI_SR_CRCERR) // CRC error flag
    {
        SPI1->SR &= ~SPI_SR_CRCERR;
        osThreadFlagsSet(showErrorDataHandle, ERROR_HANDLE_SPI1_CRC_ERROR);
    }

    if (SPI1->SR & SPI_SR_OVR) // Overrun flag
    {
        volatile uint32_t tmp;
        tmp = SPI1->DR;
        tmp = SPI1->SR;
        (void)tmp;
        osThreadFlagsSet(showErrorDataHandle, ERROR_HANDLE_SPI1_OVERRUN_ERROR);
    }

    if (SPI1->SR & SPI_SR_MODF) // Mode fault
    {
        volatile uint32_t tmp = SPI1->SR;
        (void)tmp;
        SPI1->CR1 |= SPI_CR1_SPE;
        osThreadFlagsSet(showErrorDataHandle, ERROR_HANDLE_SPI1_MODE_FAULT_ERROR);
    }
}
