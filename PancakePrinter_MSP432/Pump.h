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

#include "PortPins.h"

void InitPump();
void PumpDrive();
void PumpSetSpeed(float Duty);
void PumpStop();

void PumpDir(uint16_t direction);

#endif /* PUMP_H_ */
