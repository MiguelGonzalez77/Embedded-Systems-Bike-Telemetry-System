/***************************************************
  This handles switch inputs, uses Systick
	
	Switches are sampled at 40 Hz and the debounced output is available to view with SwitchPressedFunction.
	Recommend 2000000 period.
	
  Switches.h

	Written by Avinash Bhaskaran, Miguel Gonzalez
	
	Last Modified: 10/23/23
 ****************************************************/


#ifndef _SWITCHES_
#define _SWITCHES_

#include <stdint.h>

	
	void Switches_Init(uint32_t period, uint32_t priority);
	
	uint32_t Switches_PC4Pressed(void);
	uint32_t Switches_PC5Pressed(void);
	uint32_t Switches_PC6Pressed(void);
	uint32_t Switches_PC7Pressed(void);
	
	
	

#endif


