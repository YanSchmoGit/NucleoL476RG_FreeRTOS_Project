//
// Created by yan on 8/28/26.
//

#ifndef NUCLEO_RTOS_PROJECT_PERIPHERALS_H
#define NUCLEO_RTOS_PROJECT_PERIPHERALS_H
#include <stdint.h>

#include "stm32l476xx.h"

// SPI interface, SPI functions

void ConfigSpiInterface();

uint8_t TransferSpiDataPolling(uint8_t data);

void TransferSpiDataDMA(uint8_t *rx_data, uint8_t *tx_data, uint8_t reg);

void WriteSpiData(uint8_t reg, uint8_t data);
void ReadSpiData(uint8_t reg, uint8_t count, uint8_t *data);



// LCD screen

void ConfigLcdScreen();

void EnableLcdTimer();
void WaitTimeLcd(uint8_t time);

void SendLcdInstruction4Bit(uint8_t instruction);
void SendLcdInstruction8Bit(uint8_t instruction);

void InitializeLcdScreen();

void SendLcdChar(char data);
void SendLcdString(char *data);

void SendLcdInteger(uint32_t data);

void SetLcdCursorPosition(uint8_t x, uint8_t y);

#endif //NUCLEO_RTOS_PROJECT_PERIPHERALS_H
