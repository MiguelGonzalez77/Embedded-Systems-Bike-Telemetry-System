/***************************************************
  This deals with the heartbeat sensor and converts into a digital pulse
	
	ADC is sampled at ?? Hz and hearbeat is abstracted to view with Heartbeat_hasPulse().

  Heartbeat.h

	Written by Avinash Bhaskaran, Miguel Gonzalez
	
	Last Modified: 10/23/23
 ****************************************************/


#ifndef _HEARTBEAT_
#define _HEARTBEAT_

#include <stdint.h>

	// Setup ADC1 and pin input if necessary? can take initialiations from UnifedPort init since we used standard pin layout
	void ADC_PE2(void);
	void Heartbeat_Init(uint32_t period, uint32_t priority);
	
	void Heartbeat_Task(void);
	
	// todo, find peaks
	uint32_t Heartbeat_hasPulse(void);	
	
	int Heartbeat_getBPM(void);
	
	uint32_t get_heartbeat(void);

#endif


