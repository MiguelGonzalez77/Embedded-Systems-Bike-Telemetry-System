#include "ReedSW.h"


/*****
	ReedSW.c
	Description: 
	//1000 Hz.
	// Delta between current rising edge and previous rising edge.

// Use Timer0A to continuously sample Switch.
// If it is active above a certain counter,
// Allow value to be sent.
// Returns voltage values.
//
*****/


#define PE3       (*((volatile uint32_t *)0x40024020))    // Heartbeat LED
	
#include "inc/tm4c123gh6pm.h"
#include "./inc/ST7735.h"
#include "inc/Timer0A.h"



static int flagPE3;
static int PE3old;
static int counter;
static int period;

void ReedSW_Init(uint32_t period, uint32_t priority){
		Timer0A_Init(&ReedSW_TimeTask, period, priority);
}
void ReedSW_TimeTask(void){
	if(PE3old == 0 && PE3){		// Rising edge
		flagPE3 = 1;						
		
		period = counter;				
		counter = 0;
	}
	
	PE3old = PE3;
	counter++;
}

uint32_t ReedSW_hasPressed(int* retPeriod){
	*retPeriod = period;								// pass period
	int pressedFlagCopy = flagPE3;	
	flagPE3 = 0;												// clear flag
	
	return pressedFlagCopy;
}