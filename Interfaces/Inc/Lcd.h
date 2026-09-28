//
// Created by yan on 9/22/26.
//

#ifndef NUCLEO_RTOS_PROJECT_LCD_H
#define NUCLEO_RTOS_PROJECT_LCD_H
#include <stdint.h>

// LCD screen

void ConfigLcdScreen();



void SendLcdInstructionByte(uint8_t instruction);
void SendLcdInstructionNibble(uint8_t instruction);

void InitializeLcdScreen();

void ClearLcdScreen();

void SendLcdChar(char data);
void SendLcdString(char *data);

void SendLcdInteger(uint32_t data);

void SetLcdCursorPosition(uint8_t x, uint8_t y);
#endif //NUCLEO_RTOS_PROJECT_LCD_H
