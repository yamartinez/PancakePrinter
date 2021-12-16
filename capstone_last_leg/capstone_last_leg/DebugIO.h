/*
 * DebugIo.h
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#ifndef DEBUGIO_H_
#define DEBUGIO_H_

#include "PortPins.h"

void InitLED1();
void InitSwitches();
void InitDebugIO();

bool DebugReadS1();
bool DebugReadS2();
void DebugLED1On();
void DebugLED1Off();
void DebugToggleLED1();

#endif
