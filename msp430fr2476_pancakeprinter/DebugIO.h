/*
 * DebugIo.h
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#ifndef DEBUGIO_H_
#define DEBUGIO_H_

#include <stdbool.h>
#include <msp430.h>
#include "PortPins.h"

void InitLED();
void InitButton();
void InitDebugIO();

bool DebugReadButton();
void DebugLEDOn();
void DebugLEDOff();
void ONLEDOn();
void ONLEDOff();
void DebugToggleLED();

#endif
