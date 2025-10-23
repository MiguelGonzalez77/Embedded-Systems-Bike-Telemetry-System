/***************************************************
  This deals with the SPI interface for the DPS310 barometer sensor and Adafruit drivers.
	
  DPS310.h

	Written by Avinash Bhaskaran, Miguel Gonzalez
	
	Last Modified: 10/23/23
 ****************************************************/


#ifndef _DPS310_
#define _DPS310_

#include <stdint.h>

#define DPS310_PRSB2 0x00       ///< Highest byte of pressure data
#define DPS310_TMPB2 0x03       ///< Highest byte of temperature data
#define DPS310_PRSCFG 0x06      ///< Pressure configuration
#define DPS310_TMPCFG 0x07      ///< Temperature configuration
#define DPS310_MEASCFG 0x08     ///< Sensor configuration
#define DPS310_CFGREG 0x09      ///< Interrupt/FIFO configuration
#define DPS310_RESET 0x0C       ///< Soft reset
#define DPS310_PRODREVID 0x0D   ///< Register that contains the part ID
#define DPS310_TMPCOEFSRCE 0x28 ///< Temperature calibration src
/** The measurement rate ranges */
typedef enum {
  DPS310_1HZ,   ///< 1 Hz
  DPS310_2HZ,   ///< 2 Hz
  DPS310_4HZ,   ///< 4 Hz
  DPS310_8HZ,   ///< 8 Hz
  DPS310_16HZ,  ///< 16 Hz
  DPS310_32HZ,  ///< 32 Hz
  DPS310_64HZ,  ///< 64 Hz
  DPS310_128HZ, ///< 128 Hz
} dps310_rate_t;

/** The  oversample rate ranges */
typedef enum {
  DPS310_1SAMPLE,    ///< 1 Hz
  DPS310_2SAMPLES,   ///< 2 Hz
  DPS310_4SAMPLES,   ///< 4 Hz
  DPS310_8SAMPLES,   ///< 8 Hz
  DPS310_16SAMPLES,  ///< 16 Hz
  DPS310_32SAMPLES,  ///< 32 Hz
  DPS310_64SAMPLES,  ///< 64 Hz
  DPS310_128SAMPLES, ///< 128 Hz
} dps310_oversample_t;

typedef enum {
	DPS310_IDLE = 0b000,            ///< Stopped/idle
	DPS310_CONT_PRESTEMP = 0b111,   ///< Continuous temp+pressure measurements
} dps310_mode_t;

	
	void DPS310_Init(uint32_t period, uint32_t priority);
	
	uint32_t DPS310_getTemp(void);
	uint32_t DPS310_getPressure(void);
	uint32_t DPS310_altitudeApprox(void);

	void DPS310_SSIInit(void);
	
	
	
	

#endif


