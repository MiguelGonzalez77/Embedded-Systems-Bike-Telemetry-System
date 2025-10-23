/**
 * @file DAC.h
 * @author your name (you@domain.com)
 * @brief Low level driver for the DAC. Documentation comments are specifically
 *        for the TLV5618 12-bit SPI DAC. Modify for your particular DAC.
 * @version 0.3.0
 * @date 2023-02-14
 * 
 * @copyright Copyright (c) 2023
 * @note TLV5618 Reference datasheet: 
 *       https://www.ti.com/lit/ds/symlink/tlv5618a.pdf?ts=1676400608127&ref_url=https%253A%252F%252Fwww.google.com%252F
 */
#pragma once
#include <stdint.h>

/**
 * TODO: THIS
 * @brief Design your driver here! There are two things your driver needs to do:
 *
 * 1. Initialize SPI for your specific hardware (see page 965 on the TM4C
 *    datasheet).
 * 2. Accept a digital value (from 0 to (2^12 - 1)) and output a 16-bit data
 *    word via SPI that will be sent to the chip (see page 12 on the TLV5618
 *    datasheet).
 * 
 * Provided are two possible function declarations. You may modify this based on
 * your implementation and API design.
 */


/**
 * @brief dac_init initializes SPI at pins <TODO: STATE PINS HERE> to
 *        communicate with the TLV5618 12-bit SPI DAC.
 * 
 * @return int 0 if initialization was successful or a positive integer if an
 *         error occurred. <TODO: LIST ERROR CODES AND POSSIBLE ERROR EVENTS>
 */
int dac_init();

/**
 * @brief dac_output tells the DAC to output a specified voltage.
 * 
 * @param data 12-bit data value representing the voltage that should be output
 *        to the TLV5618.
 * @return int 0 if initialization was successful or a positive integer if an
 *         error occurred. <TODO: LIST ERROR CODES AND POSSIBLE ERROR EVENTS>
 */
int dac_output(uint16_t data);
