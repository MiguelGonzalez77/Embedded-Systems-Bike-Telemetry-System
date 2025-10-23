/***************************************************
  This deals with the reed switch. Uses Timer0A.
	
  ReedSW.h

	Written by Avinash Bhaskaran, Miguel Gonzalez
	
	Last Modified: 10/23/23
 ****************************************************/


#ifndef _REED_
#define _REED_

#include <stdint.h>
#include "./inc/Timer0A.h"
#include "inc/tm4c123gh6pm.h"

	// Setup timer0a, pins if necessary, any variables
	void ReedSW_Init(uint32_t period, uint32_t priority);
	
	// similar to switches.c hasPressed functions
	uint32_t ReedSW_hasPressed(int* retPeriod);
	
	void ReedSW_TimeTask(void);
	
	

#endif


