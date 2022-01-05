/*
 * Commands.h
 *
 *  Created on: Dec 6, 2021
 *      Author: yudel
 */

#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <msp430.h>
#include <stdint.h>
#include "Error.h"
#include "Carriage.h"

typedef enum CommandType {
    move_dry = 0,
    move_wet = 1,
    time_delay = 2,
    done = 3
} CommandType;

typedef struct Command {
    CommandType command:2;
    uint16_t    arg0:16;
    uint16_t    arg1:16;
} Command;

int8_t EnqueueCommand(Command*c);
Command* DequeueCommand();
uint16_t QueueSize();
void ExecuteCommand(Command *c);

#endif /* COMMANDS_H_ */
