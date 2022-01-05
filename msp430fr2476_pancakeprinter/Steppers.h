/*
 * Steppers.h
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#ifndef STEPPERS_H_
#define STEPPERS_H_

#include <PortPins.h>
#include "./Error.h"
#include "./DebugIO.h"


void InitStepperMotors(uint16_t, uint16_t);
void CalibrateSteppers();
void move_absolute(int16_t x, int16_t y);
void move_image(int16_t x, int16_t y);
void fill_image_square();
void TestSteppers();

typedef enum direction{
    none,
    backward,
    forward
} direction;

#endif /* STEPPERS_H_ */
