// This will be the module that will handle the switches Uses Systick
// Lots of optimization possible but it works... (refactor to set set bitvector so only one function is needed)

#include <stdint.h>
#include "./inc/tm4c123gh6pm.h"
#include "./inc/CortexM.h"

#define PC4 (*((volatile uint32_t *)0x40006040))
#define PC5 (*((volatile uint32_t *)0x40006080))
#define PC6 (*((volatile uint32_t *)0x40006100))
#define PC7 (*((volatile uint32_t *)0x40006200))


#define IDLE_TIME 200 // This is because given in main that it is 40Hz sampling (25ms/interrupt), therefore
// 5 / 25ms = 200 intervals


static uint32_t pc4Flag, pc5Flag, pc6Flag, pc7Flag, pc4Old, pc5Old, pc6Old, pc7Old, idleFlag;

static uint32_t idleCounter = 0;

void sampleSwitches(void);

void Switches_Init(uint32_t period, uint32_t priority){
		NVIC_ST_CTRL_R = 0;       				// Turn off SysTick during initialization
  	NVIC_ST_RELOAD_R = period;     		// Set SysTick reload
  	NVIC_ST_CURRENT_R = 0;    				// Clear current register to load new reload value
  	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | priority<<29;  // Priority 
  	NVIC_ST_CTRL_R = 0x7;     // Turn on the Timer with external clock and interrupts (bit2 ClkSrc, bit1 IntEn, bit0 Enable)
}



void SysTick_Handler(void){
	if(pc4Old == 0 && PC4){
		pc4Flag = 1;
		idleCounter = 0; // added idleCounter = 0 to each if showing that it resets idleCounter when button is pressed
	}
	if(pc5Old == 0 && PC5){
		pc5Flag = 1;
		idleCounter = 0;
	}
	if(pc6Old == 0 && PC6){
		pc6Flag = 1;
		idleCounter = 0;
	}
	if(pc7Old == 0 && PC7){
		pc7Flag = 1;
		idleCounter = 0;
	}
	pc4Old = PC4;
  	pc5Old = PC5;
	pc6Old = PC6;
	pc7Old = PC7;
	
	if(!(pc4Flag || pc5Flag || pc6Flag || pc7Flag)){ // this if statement checks the flags, if one of the flags is 1
		// then a button has been pressed in one of them, so it negates and doesn't execute.
		// but if none are pressed, it will increment idleCounter until it reaches IDLE_TIME.
		if (idleFlag == 0 && idleCounter == IDLE_TIME){
			idleFlag = 1;
    }	
		idleCounter++;
	}
}

// These functions are called by main
uint32_t Switches_PC4Pressed(void) { 
	uint32_t pressedFlagCopy = pc4Flag;
	pc4Flag = 0;
	
	return pressedFlagCopy;
	
}
uint32_t Switches_PC5Pressed(void) {
	uint32_t pressedFlagCopy = pc5Flag;
	pc5Flag = 0;
	
	return pressedFlagCopy;
}
uint32_t Switches_PC6Pressed(void) { 
	uint32_t pressedFlagCopy = pc6Flag;
	pc6Flag = 0;
	
	return pressedFlagCopy;
	
}
uint32_t Switches_PC7Pressed(void) { 
	uint32_t pressedFlagCopy = pc7Flag;
	pc7Flag = 0;
	
	return pressedFlagCopy;
}

uint32_t Switches_Idle(void){
	uint32_t idleFlagCopy = idleFlag;
	idleFlag = 0;
	
	return idleFlagCopy;
}




