/*
 * Carriage.h
 *
 *  Created on: Dec 6, 2021
 *      Author: yudel
 */

#ifndef CARRIAGE_H_
#define CARRIAGE_H_

#include <stdint.h>
#include "Steppers.h"
#include "Pump.h"

void CarriageMove(uint16_t x,uint16_t y,uint8_t p);
void CarriageWait(uint16_t t);
void CarriageDone();


#endif /* CARRIAGE_H_ */
