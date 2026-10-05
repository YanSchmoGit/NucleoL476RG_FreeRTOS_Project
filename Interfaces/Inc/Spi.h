//
// Created by yan on 8/28/26.
//

#ifndef NUCLEO_RTOS_PROJECT_PERIPHERALS_H
#define NUCLEO_RTOS_PROJECT_PERIPHERALS_H
#include <stdint.h>

#include "stm32l476xx.h"
#include "SpiDevice.h"

// SPI interface, SPI functions

void ConfigSpiInterfaceHardware();
void ConfigSpiInterfaceSoftware(SPIDevice_t* SpiDevice);


uint8_t TransferSpiDataPolling(uint8_t data);
void WriteSpiDataPolling(uint8_t reg, uint8_t data);
void ReadSpiDataPolling(uint8_t reg, uint8_t count, uint8_t *data);

void TransferSpiDataDMA(uint8_t *rx_data, uint8_t *tx_data, uint8_t length);
void WriteSpiDataDMA(void* ctx, uint8_t* data_tx, uint8_t reg, uint8_t length);
void ReadSpiDataDMA(void* ctx, uint8_t* data_rx, uint8_t reg, uint8_t length);



#endif //NUCLEO_RTOS_PROJECT_PERIPHERALS_H
