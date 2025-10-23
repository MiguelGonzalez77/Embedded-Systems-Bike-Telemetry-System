/**
 * @file display.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-11-09
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once



#include "/../inc/ST7735.h"

void formatNumber(int32_t n, int32_t numberLength, int decimalSpot, char* decimalString);
void displayInit(int period, int priority);

void displayUpdateTask(void);
void updateDisplay(void);

void ST7735_DrawGraph(int16_t vertical, uint16_t color);

void Output_Value(int value, int y_cord, int x_cord, char* String);




