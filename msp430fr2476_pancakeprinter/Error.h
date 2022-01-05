/*
 * Error.h
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#ifndef ERROR_H_
#define ERROR_H_

#include <stdint.h>

typedef enum error_code{        //  STATUS0 | STATUS1
    unknown,                    //   on         on
    steppers_not_initialized,   //  blink       on
    steppers_not_calibrated,    //   on        blink
    direction_error_x,          //   off       blink
    direction_error_y,          //   on         off
    bounds_error_x,             //  nblink     blink
    bounds_error_y              //   blink     blink
} error_code;

void error(error_code err, char*file, int line);

#endif /* ERROR_H_ */
