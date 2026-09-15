//
// Created by yan on 7/26/26.
//

#include "readSensorData_task.h"

#include "cmsis_os2.h"
#include "queues.h"
#include "stm32l476xx.h"
#include "../../Interfaces/Inc/Peripherals.h"
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

static uint8_t tx_data[7];
static uint8_t rx_data[7];

static BMP280Values BMP280ValueData;


// Task loop
void startReadSensorData(void* argument)
{


    /* Infinite loop */
    for (;;)
    {



        TransferSpiDataDMA(rx_data, tx_data, 0xF7);

        ProcessSensorData(&BMP280ValueData, rx_data);

        osMessageQueuePut(sensorDataHandle, &BMP280ValueData, 0, osWaitForever);


        osDelay(100);
    }
}

void DMA1_Channel2_IRQHandler(void)
{
    // Check if transfer complete is set for DMA 2 Channel
    if (DMA1->ISR & DMA_ISR_TCIF2)
    {
        DMA1->IFCR = DMA_IFCR_CTCIF2; // delete interrupt flag

        if (readSensorDataHandle != NULL)
        {
            osThreadFlagsSet(readSensorDataHandle, 0x01);
        }
    }
}
