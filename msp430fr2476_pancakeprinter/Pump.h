/**
 * @file Pump.h
 * @author bilge batsukh
 * @brief Simple MOSFET driven pump driver for our pancake printer - header file
 * @version 0.1
 * @date 2021-11-30
 *
 * @copyright none, disclaimer: use at your own risk, provided as-is without warranty or assurance of function
 *
 */

#ifndef PUMP_H_
#define PUMP_H_

#include <stdint.h>
#include <msp430.h>
#include "PortPins.h"
/**
 * P4.6 - Output PWM using TB1.1 to drive MOSFET
 */

#define PUMP_PIN 0x01


void InitPump();
void PumpDrive();
void PumpSetSpeed(uint8_t Duty);
void PumpStop();

#endif /* PUMP_H_ */
