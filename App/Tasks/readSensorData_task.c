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

void createTaskReadSensorData(BMP280Handle* sensor)
{
    /* creation of readSensorData */
    readSensorDataHandle = osThreadNew(startReadSensorData, sensor, &readSensorData_attributes);
}


// Task loop
void startReadSensorData(void* argument)
{
    BMP280Handle* sensor = (BMP280Handle*)argument;

    static volatile uint8_t rx_data[7];


    /* Infinite loop */
    for (;;)
    {

        sensor->Device.readData(NULL, rx_data,BMP280_REGISTER_PRESS_MSB, 7);
        ProcessSensorData(sensor, rx_data);



        // check sensor data
        if (CheckSensorData(&sensor->SensorValues, 4000, 110000, 0, 0) == 1)
        {
            // Bad values
            osThreadFlagsSet(showErrorDataHandle, ERROR_HANDLE_VALUES_OUT_OF_BOUNDS);
        }
        else
        {
            // Good values
            osMessageQueuePut(sensorDataHandle, &sensor->SensorValues, 0, osWaitForever);
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

