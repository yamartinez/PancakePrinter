/*
 * Steppers.h
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 *
 * v2.0 - for msp432p401r
 * most pin and port definitions have been moved to PortPins.h
 *
 */

#ifndef STEPPERS_H_
#define STEPPERS_H_

#include "PortPins.h"

void InitStepperMotors(uint16_t, uint16_t);
void CalibrateSteppers();
void move_relative(int16_t x, int16_t y);
void move_absolute(int16_t x, int16_t y);
void move_image(int16_t x, int16_t y);
typedef enum direction{
    none,
    backward,
    forward
} direction;

#endif /* STEPPERS_H_ */
