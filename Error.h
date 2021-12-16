/*
 * Error.h
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#ifndef ERROR_H_
#define ERROR_H_


typedef enum error_code{ //TODO: rename error codes from x and y to high and low
    unknown,
    steppers_not_initialized,
    steppers_not_calibrated,
    direction_error_x,
    direction_error_y,
    bounds_error_x,
    bounds_error_y
} error_code;

void error(error_code);


#endif /* ERROR_H_ */