
#include <stdint.h>
#include <stdbool.h>
#include "./inc/tm4c123gh6pm.h"
#include "./inc/Timer2A.h"
#include "./inc/CortexM.h"
#include "Heartbeat.h"
#include "./inc//ADCSWTrigger.h"
#include "./inc//ST7735.h"
void ADC_PE2(void){ 
  SYSCTL_RCGCADC_R |= 0x0001;   // 7) activate ADC0 
                                  // 1) activate clock for Port E
  SYSCTL_RCGCGPIO_R |= 0x10;
  while((SYSCTL_PRGPIO_R&0x10) != 0x10){};
  GPIO_PORTE_DIR_R &= ~0x04;      // 2) make PE2 input
  GPIO_PORTE_AFSEL_R |= 0x04;     // 3) enable alternate function on PE2
  GPIO_PORTE_DEN_R &= ~0x04;      // 4) disable digital I/O on PE2
  GPIO_PORTE_AMSEL_R |= 0x04;     // 5) enable analog functionality on PE2
    
  while((SYSCTL_PRADC_R&0x0001) != 0x0001){};    // good code, but not yet implemented in simulator


  ADC0_PC_R &= ~0xF;              // 7) clear max sample rate field
  ADC0_PC_R |= 0x1;               //    maximum speed is 125K samples/sec
  ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
  ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
  ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
  ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
  ADC0_SSMUX3_R += 1;             //    set channel
  ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
  ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
  ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3
}

//void ADC_Init(void){ 
//SYSCTL_RCGCADC_R |= 0x0001;   // 1) activate ADC0 

//  ADC0_PC_R &= ~0xF;              // 2) clear max sample rate field
//  ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
//  ADC0_SSPRI_R = 0x0123;          // 3) Sequencer 3 is highest priority
//  ADC0_ACTSS_R &= ~0x0008;        // 4) disable sample sequencer 3
//  ADC0_EMUX_R &= ~0xF000;         // 5) seq3 is software trigger
//  ADC0_SSMUX3_R &= ~0x000F;       // 6) clear SS3 field
//  ADC0_SSMUX3_R += 5;             //    set channel
//  ADC0_SSCTL3_R = 0x0006;         // 7) no TS0 D0, yes IE0 END0
//  ADC0_IM_R &= ~0x0008;           // 8) disable SS3 interrupts
//  ADC0_SAC_R = 6;  
//  ADC0_ACTSS_R |= 0x0008;         // 9) enable sample sequencer 3
//}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5
//uint32_t ADC_In(void){  
//    // 1) initiate SS3
//  // 2) wait for conversion done
//  // 3) read result
//  // 4) acknowledge completion
//  uint32_t data;
//  
//  ADC0_PSSI_R = 0x8;
//  while((ADC0_RIS_R&0x8) == 0){};
//  data = ADC0_SSFIFO3_R&0xFFF;
//  ADC0_ISC_R = 0x8;
//  return data;
//}

int Signal, OldSignal;
int Threshold = 2100;			// which Signal count as a beat and which to ignore
int cnt, beatcnt;
int P = 512;               // Peak of the pulse wave
int T = 512;               // Trough of pulse wave
volatile int BPM;          // beats per minute
volatile int IBI = 600;    // holds the time between beats
volatile bool Pulse = false;
volatile bool QS = false;  // becomes true when a beat is detected
int amp = 100;             // Amplitude of the pulse wave
bool firstBeat = true;     // Flag for the first detected beat
bool secondBeat = false;   // Flag for the second detected beat
int rate[10];              // Array to hold the last ten IBI values
unsigned long sampleCounter = 0; // Used to determine pulse timing
unsigned long lastBeatTime = 0;  // Used to find the inter-beat interval

#define HBFILTERSIZE 8
int hbf[HBFILTERSIZE];
int hb_idx;


// Setup ADC1 and pin input if necessary? can take initialiations from UnifedPort init since we used standard pin layout	
void Heartbeat_Init(uint32_t period, uint32_t priority){
//	
//	//initalize timer2A
//	SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
//  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
//  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
//  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
//  TIMER2_TAILR_R = period-1;    // 4) reload value
//  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
//  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
//  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
//  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)| (priority << 29); // priority 2 // clears
//	// all top 8 bits and shifts bits to 29 positions. priority = 010.
//// interrupts enabled in the main program after all devices initialized
//// vector number 39, interrupt number 23
//  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
//  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
	//ADC_PE2();
	ADC0_InitSWTriggerSeq3(1);
	Timer2A_Init(&Heartbeat_Task, period, priority);
}
uint32_t heartbeat;
void Heartbeat_Task(void){
	Heartbeat_hasPulse();
}



#include "../inc/UART.h"
// find peaks
#define PF1                     (*((volatile uint32_t *)0x40025008))
uint32_t Heartbeat_hasPulse(void){	// assumes 500Hz sampling
	PF1 ^= 2;
	PF1 ^= 2;
	
	heartbeat = ADC0_InSeq3();
	
	if (cnt >= 1000) {
		BPM = 0;
	}
	if(OldSignal < Threshold && heartbeat > Threshold ){
		UART_OutUDec(cnt);
		UART_OutString("\r\n");
		BPM = (300000)/cnt; // cnt is in 2ms units, 60/2ms for bpm = 30000 .01units *10
		hbf[hb_idx] = BPM;
		hb_idx = (hb_idx + 1) % HBFILTERSIZE;
		cnt = 0;
	}
	
	cnt++;
	OldSignal = heartbeat;
	PF1 ^= 2;
	return 1;
}

uint32_t get_heartbeat(void){
	return heartbeat;
}

int Heartbeat_getBPM(void){
	int sum = 0;
	for (int i = 0; i < HBFILTERSIZE; i++) {
		sum+=hbf[i];
	}
	return sum/HBFILTERSIZE;
	return BPM;
}

