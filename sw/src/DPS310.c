

// DPS310 barometer device is used to measure pressure in Hectopascal and temperature in celsius. 
// altitude is in meters.
// Adapted from Adafruit Drivers

#include <stdint.h>
#include "./inc/tm4c123gh6pm.h"
#include "DPS310.h"

#define PD1       (*((volatile uint32_t *)0x40007008))		// PD1
#include "../inc/UART.h"

static int32_t oversample_scalefactor[] = {524288, 1572864, 3670016, 7864320,
                                           253952, 516096,  1040384, 2088960}; // used for configuring pressure and temp
static int _c0, _c1, _c00, _c10, _c01, _c11, _c20, _c21, _c30;

void Wait10ms(uint32_t n);
uint8_t sendAfterWaiting(uint8_t code);
int twosComplement(int val, int bit);
void DPS310_Write(uint8_t addr, uint8_t data);
int DPS310_Read(uint8_t addr, int numBytes);
																					 
void DPS310_Init(uint32_t period, uint32_t priority){
	SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period-1;    // 4) reload value
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x00000001;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0xFFFF00FF)|(priority << 13); // priority level 4, I think having lower priority
	// here could be better altitude data changes more slowly. 
// interrupts enabled in the main program after all devices initialized
// vector number 37, interrupt number 21
  NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}




int32_t raw_pressure, raw_temperature;
float _temperature, _pressure, _scaled_rawtemp;
int32_t temp_scale, pressure_scale;

int temp, alt, pres;

//int pressPa, tempCh;
void _Read(){
	raw_temperature = twosComplement(DPS310_Read(DPS310_TMPB2, 3), 24);
  raw_pressure = twosComplement(DPS310_Read(DPS310_PRSB2, 3), 24);

	_scaled_rawtemp = (float)raw_temperature / temp_scale;
  _temperature = _scaled_rawtemp * _c1 + _c0 / 2.0;
  
  _pressure = (float)raw_pressure / pressure_scale;
  _pressure =
      (int32_t)_c00 +
      _pressure * ((int32_t)_c10 +
                   _pressure * ((int32_t)_c20 + _pressure * (int32_t)_c30)) +
      _scaled_rawtemp *
          ((int32_t)_c01 +
           _pressure * ((int32_t)_c11 + _pressure * (int32_t)_c21));

	
			// INTEGER IMPLEMENTATION
//  tempCh = (10*raw_temperature*_c1)/temp_scale + 5 * _c0;

//	pressPa =
//      _c00 +
//      (raw_pressure * (_c10 +
//                   (raw_pressure * (_c20 + (raw_pressure*_c30))/ pressure_scale )/ pressure_scale) ) / pressure_scale +
//      (raw_temperature *
//          ((int32_t)_c01 +
//           (raw_pressure * (_c11 + (raw_pressure / pressure_scale) * _c21) ) / pressure_scale) ) / temp_scale;
	
	//tempCh = (int)(_temperature * 100);
	//pressPa = (int)(_pressure);
  //UART_OutSDec(tempCh); UART_OutString(" * .01 C\r\n");
	//UART_OutSDec(pressPa); 		UART_OutString(" * Pa\r\n");
	return;
}
#define ALT_FILTER_SIZE 8
int altf[ALT_FILTER_SIZE];
int alt_idx;


uint32_t DPS310_getTemp(void){	// .01 units C
	return temp;
}
uint32_t DPS310_getPressure(void){		// units Pa
	return pres;
}
uint32_t DPS310_altitudeApprox(void){		// units cm
	int sum = 0;
	for (int i =0; i < ALT_FILTER_SIZE; i++) {
		sum += altf[i];
	}
	return sum/ALT_FILTER_SIZE;
	
	//return alt;
}

void Wait10ms(uint32_t n) {
    uint32_t volatile time;
    while (n){
        time = 727240 * 2 / 91;  // 10msec
        while (time){
            --time;
        }
        --n;
    }
}



void DPS310_SSIInit(){
	PD1 = 0;									// Pull low to activate SPI interface on DPS
	
	SYSCTL_RCGCSSI_R |= 0x02;  // activate SSI1
	volatile int x = 0;
	x = 1; 										// delay
	PD1 = 2;									// Active low CS
	Wait10ms(1);
	
	SSI1_CR1_R = 0;	// Disable SSI, Master Mode
	SSI1_CPSR_R = 20;	// 4MHz 
	SSI1_CR0_R = (SSI1_CR0_R&(0xFFFF000F)) | (3<<6);     // SCR = 0, SPH = 1, SPO = 1 Freescale
  SSI1_CR0_R = (SSI1_CR0_R & 0xFFFFFFF0) | 0x7;              // DSS = 8-bit data
  SSI1_CR1_R |= 0x00000002;        // Enable SSI
	
	// REGISTER SETUP //
														// CHECK FOR DPS
	int prodID = DPS310_Read(DPS310_PRODREVID, 1);	
	UART_OutUHex(prodID);
	if (prodID == 0x10) {
		UART_OutString(" DPS!\r\n");
	}
	
														// SW RESET
	DPS310_Write(DPS310_RESET, 0x89);			// Flush FIFO, SOFTWARE RESET
	Wait10ms(10);											// WAIT FOR DEVICE RESET
	
	int SENSOR_RDY = (DPS310_Read(DPS310_MEASCFG, 1)) & 0x40;	// bit 6 is RDY Flag
	while (SENSOR_RDY == 0) {
		Wait10ms(1);
		SENSOR_RDY = (DPS310_Read(DPS310_MEASCFG, 1)) & 0x40;
	}
	UART_OutString("RESET DONE\r\n");
	
														// CALIBRATION COEF
	// Wait till we're ready to read calibration
  int CALIB_RDY = (DPS310_Read(DPS310_MEASCFG, 1)) & 0x80;	// bit 7 is RDY Flag
	while (CALIB_RDY == 0){
    Wait10ms(1);
		CALIB_RDY = (DPS310_Read(DPS310_MEASCFG, 1)) & 0x80;
  }
	UART_OutString("COEF RDY\r\n");
	
	uint8_t coeffs[18];
  for (uint8_t addr = 0; addr < 18; addr++) {
		coeffs[addr] = DPS310_Read(0x10 + addr, 1);			// COEF START AT 0x10
  }
  _c0 = ((uint16_t)coeffs[0] << 4) | (((uint16_t)coeffs[1] >> 4) & 0x0F);
  _c0 = twosComplement(_c0, 12);

  _c1 = twosComplement((((uint16_t)coeffs[1] & 0x0F) << 8) | coeffs[2], 12);

  _c00 = ((uint32_t)coeffs[3] << 12) | ((uint32_t)coeffs[4] << 4) |
         (((uint32_t)coeffs[5] >> 4) & 0x0F);
  _c00 = twosComplement(_c00, 20);

  _c10 = (((uint32_t)coeffs[5] & 0x0F) << 16) | ((uint32_t)coeffs[6] << 8) |
         (uint32_t)coeffs[7];
  _c10 = twosComplement(_c10, 20);

  _c01 = twosComplement(((uint16_t)coeffs[8] << 8) 	| (uint16_t)coeffs[9], 16);
  _c11 = twosComplement(((uint16_t)coeffs[10] << 8) | (uint16_t)coeffs[11], 16);
  _c20 = twosComplement(((uint16_t)coeffs[12] << 8) | (uint16_t)coeffs[13], 16);
  _c21 = twosComplement(((uint16_t)coeffs[14] << 8) | (uint16_t)coeffs[15], 16);
  _c30 = twosComplement(((uint16_t)coeffs[16] << 8) | (uint16_t)coeffs[17], 16);
  
  UART_OutString("c0 = "); 	UART_OutSDec(_c0); 	UART_OutString("\r\n");
	UART_OutString("c1 = ");  UART_OutSDec(_c1);	UART_OutString("\r\n");
  UART_OutString("c00 = "); UART_OutSDec(_c00);	UART_OutString("\r\n");
  UART_OutString("c10 = "); UART_OutSDec(_c10); UART_OutString("\r\n");
  UART_OutString("c01 = "); UART_OutSDec(_c01);	UART_OutString("\r\n");
  UART_OutString("c11 = "); UART_OutSDec(_c11); UART_OutString("\r\n");
  UART_OutString("c20 = "); UART_OutSDec(_c20);	UART_OutString("\r\n");
  UART_OutString("c21 = "); UART_OutSDec(_c21);	UART_OutString("\r\n");
  UART_OutString("c30 = "); UART_OutSDec(_c30);	UART_OutString("\r\n");
	
															// PRESSURE/TEMP  CONFIG
	DPS310_Write(DPS310_PRSCFG, ((DPS310_64HZ<<4) | (DPS310_64SAMPLES)) );		// PRESSURE 64 oversample 64Hz rate
  pressure_scale = oversample_scalefactor[DPS310_64SAMPLES];
																	
	int srcbit = DPS310_Read(DPS310_TMPCOEFSRCE, 1) & 0x80;														// FIND TMP COEF
	DPS310_Write(DPS310_TMPCFG, (srcbit | (DPS310_64HZ<<4) | (DPS310_64SAMPLES)) );		// set coef, TEMP 64 oversample 64Hz rate
  temp_scale = oversample_scalefactor[DPS310_64SAMPLES];
	
  DPS310_Write(DPS310_CFGREG, 0x0C);																			// SET SHIFT BITS SINCE SAMPLES > 8
	
															// MODE CONFIG
	DPS310_Write(DPS310_MEASCFG, DPS310_CONT_PRESTEMP);
	
	int temp_pres_RDY = DPS310_Read(DPS310_MEASCFG, 1) & 0x30;		// wait for valid temp/pressure reading
	while (temp_pres_RDY != 0x30) {
		Wait10ms(1);
		temp_pres_RDY = DPS310_Read(DPS310_MEASCFG, 1) & 0x30;
	}
	
	_Read();
  
}


void Timer1A_Handler(void) {
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
	
	int temp_pres_RDY = DPS310_Read(DPS310_MEASCFG, 1) & 0x30;		// wait for valid temp/pressure reading
	if (temp_pres_RDY == 0x30) {
		_Read();
		temp = (int)(_temperature*100);
		pres = (int)(_pressure);
		alt = (int)(-8.325683 * (_pressure - 101325));
		altf[alt_idx] = alt;
		alt_idx = (alt_idx + 1) % ALT_FILTER_SIZE;
	}
	
}


uint8_t sendAfterWaiting(uint8_t code) {	// bypasses fifo, ensure correct operation of CSB
	uint8_t dummy;
	while (SSI1_SR_R&SSI_SR_RNE){	// flush any remaining data
		dummy = SSI1_DR_R;
	}
	
	while ( (SSI1_SR_R&SSI_SR_TFE) == 0) {} // wait until all pending data is sent
	SSI1_DR_R = code;			// data output
	while ( (SSI1_SR_R&SSI_SR_RNE) == 0) {} // wait for response
	return SSI1_DR_R;												// return data
}

int twosComplement(int val, int bit) {
    if (val & (1<<(bit-1))) {	// NEGATIVE NUMBER
        val -= (1<<bit);	// TOGGLE OTHER BITS
    }
    return val;
}
void DPS310_Write(uint8_t addr, uint8_t data){
	PD1 = 0;																		// START TRANSACTION
	sendAfterWaiting(addr);											// WRITE START ADDR
	sendAfterWaiting(data);											// WRITE VALUE
	PD1 = 2;																		// END TRANSACTION	
}
int DPS310_Read(uint8_t addr, int numBytes){
	int data = 0;
	
	PD1 = 0;																		// START TRANSACTION
	sendAfterWaiting(0x80 | addr);							// READ START ADDR
	
	for(int i = 0; i < numBytes; i++) {	
		data = data << 8;
		data |= sendAfterWaiting(0);								// GET VALUE
	}
	
	PD1 = 2;																		// END TRANSACTION
	
	return data;
}


