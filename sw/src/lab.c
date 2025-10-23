/**
 * @file        lab.c
 * @author      your name (your_email@doman.com), Jonathan Valvano, Matthew Yu
 *              <TA NAME and LAB SECTION # HERE>
 * @brief       An empty main file for running your lab.
 * @version     0.1.0
 * @date        2022-10-08 <REPLACE WITH DATE OF LAST REVISION>
 * @copyright   Copyright (c) 2022
 * @note        Potential Pinouts:
 *                  Backlight (pin 10) connected to +3.3 V
 *                  MISO (pin 9) unconnected
 *                  SCK (pin 8) connected to PA2 (SSI0Clk)
 *                  MOSI (pin 7) connected to PA5 (SSI0Tx)
 *                  TFT_CS (pin 6) connected to PA3 (SSI0Fss)
 *                  CARD_CS (pin 5) unconnected
 *                  Data/Command (pin 4) connected to PA6 (GPIO)
 *                  RESET (pin 3) connected to PA7 (GPIO)
 *                  VCC (pin 2) connected to +3.3 V
 *                  Gnd (pin 1) connected to ground
 *
 *                  Center of 10k-ohm potentiometer connected to PE2/AIN1
 *                  Bottom of 10k-ohm potentiometer connected to ground
 *                  Top of 10k-ohm potentiometer connected to +3.3V
 *
 *              Warning. Initial code for the RGB driver creates bright flashing
 *              lights. Remove this code and do not run if you have epilepsy.
 */

/** File includes. */
#include <stdint.h>
#include <stdbool.h>
/* Register definitions. */
#include "./inc/tm4c123gh6pm.h"
/* Clocking. */
#include "./inc/PLL.h"
/* Clock delay and interrupt control. */
#include "./inc/CortexM.h"
/* Initialization of all the pins. */
#include "./inc/Unified_Port_Init.h"
/* Talking to PC via UART. */
#include "./inc/UART.h"
/* ST7735 display. */
#include "./inc/ST7735.h"

/* Add whatever else you need here! */
#include "./lib/RGB/RGB.h"
#include "DPS310.h"
#include "ReedSW.h"
#include "Heartbeat.h"
#include "Switches.h"
#include "display.h"


void Pause(void);
int Diameter = 24;

void IntToStr(int value, char *str) { // since we can't use sprintf, using this to convert int to string
	// for displaying HR in the next function
    char temp[20]; // Temporary buffer
    int i = 0;
    bool isNegative = false;

    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    if (value < 0) {
        isNegative = true;
        value = -value;
    }

    // Fill in the buffer backwards
    while (value != 0) {
        temp[i++] = (value % 10) + '0'; // Convert integer to character
        value /= 10;
    }

    if (isNegative) {
        temp[i++] = '-';
    }

    temp[i] = '\0';

    // Reverse the string
    int j;
    for (j = 0; j < i; j++) {
        str[j] = temp[i - j - 1];
    }
    str[j] = '\0'; // Null-terminate the string
}
void DisplayHeartRate(int BPM) { // function to help display BPM onto the display screen. 
    if (BPM >= 0) {
        char bpmString[20];
        IntToStr(BPM, bpmString);

        char displayString[30];
        int i = 0, j = 0;
        // Copy "HR: " to displayString
        const char prefix[] = "HR: ";
        while (prefix[j] != '\0') {
            displayString[i++] = prefix[j++];
        }
        // Copy bpmString to displayString
        j = 0;
        while (bpmString[j] != '\0') {
            displayString[i++] = bpmString[j++];
        }
        displayString[i++] = ' ';
        displayString[i++] = 'B';
        displayString[i++] = 'P';
        displayString[i++] = 'M';
        displayString[i] = '\0';

        ST7735_SetCursor(0, 0);
        ST7735_OutString(displayString);
    }
}

int main(void) {
	
    // Disable interrupts for initialization.
    DisableInterrupts();

    // Initialize clocking.
    PLL_Init(Bus80MHz);

    // Allow us to talk to the PC via PuTTy! Check device manager to see which
    // COM serial port we are on. The baud rate is 115200 chars/s.
    UART_Init();
	
		ST7735_InitR(INITR_REDTAB);

    // Initialize all ports.
    Unified_Port_Init();

//    // Start RGB flashing. WARNING! BRIGHT FLASHING COLORS. DO NOT RUN IF YOU HAVE EPILEPSY.
//    RGBInit();

    // Allows any enabled timers to begin running.
    EnableInterrupts();

//    // Print starting message to the PC and the ST7735.
//    ST7735_FillScreen(ST7735_BLACK);
//    ST7735_SetCursor(0, 0);
//    ST7735_OutString(
//        "ECE445L Final lab.\n"
//        "Press SW1 to start.\n");
//    UART_OutString(
//        "ECE445L Final lab.\r\n"
//        "Press SW1 to start.\r\n");
//    Pause();

//    // Stop RGB and turn off any on LEDs.
//    RGBStop();
//    PF1 = 0;
//    PF2 = 0;
//    PF3 = 0;

    // Reset screen.
    ST7735_FillScreen(ST7735_BLACK);
    ST7735_SetCursor(0, 0);
    ST7735_OutString("Starting...\n");
    UART_OutString("Starting...\r\n");
		
		// START /////////////
		DPS310_SSIInit();
		DPS310_Init(4000000, 6);			// 20Hz Priority 6 
		displayInit(2000000, 7);			// 20Hz
		Switches_Init(2000000, 5);		// 40Hz 	Priority 5
		ReedSW_Init(80000, 4);				// 1000Hz Priority 4
		Heartbeat_Init(160000, 3); // 500Hz Priority 1 
		// a max of 200 BPM for safety. equivalent to 3.33 BPS, through nyquist thm, we choose 6.66, but safer
		// side choose 10Hz
		ST7735_SetCursor(0, 0);
		ST7735_OutString("D: 24 in");
		
    while (1) {
        // TODO: Write your code here!
			
			if (Switches_PC4Pressed()){
				
				UART_OutString("PC4\r\n\n");
			}
			if (Switches_PC7Pressed()){
				UART_OutString("PC7\r\n\n");
				Diameter = (Diameter < 40) ? Diameter + 1 : Diameter;
				ST7735_SetCursor(3, 0);
				ST7735_OutUDec(Diameter);
			}
			if (Switches_PC6Pressed()){
				UART_OutString("PC6\r\n\n");
				Diameter = (Diameter > 11) ? Diameter - 1 : Diameter;
				ST7735_SetCursor(3, 0);
				ST7735_OutUDec(Diameter);
			}
			if (Switches_PC5Pressed()){
				UART_OutString("PC5\r\n\n");
			}
			
			
			int period;
			if (ReedSW_hasPressed(&period)) {
				UART_OutUDec(period);
				UART_OutString("ms REED\r\n\n");
			}
			
//			int currentBPM = Heartbeat_getBPM();
//        if (currentBPM > 0) {
//            DisplayHeartRate(currentBPM);
//        }
//			
//			
			WaitForInterrupt();
    }
		
		
    return 1;
}



/** Function Implementations. */
void DelayWait10ms(uint32_t n) {
    uint32_t volatile time;
    while (n){
        time = 727240 * 2 / 91;  // 10msec
        while (time){
            --time;
        }
        --n;
    }
}
void Pause(void) {
    while (PF4 == 0x00) {
        DelayWait10ms(10);
    }
    while (PF4 == 0x10) {
        DelayWait10ms(10);
    }
}
