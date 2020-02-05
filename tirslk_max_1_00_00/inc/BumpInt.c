// BumpInt.c
// Runs on MSP432, interrupt version
// Provide low-level functions that interface bump switches on the robot.
// Daniel Valvano and Jonathan Valvano
// July 11, 2019

/* This example accompanies the book
   "Embedded Systems: Introduction to Robotics,
   Jonathan W. Valvano, ISBN: 9781074544300, copyright (c) 2019
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/

Simplified BSD License (FreeBSD License)
Copyright (c) 2019, Jonathan Valvano, All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are
those of the authors and should not be interpreted as representing official
policies, either expressed or implied, of the FreeBSD Project.
*/

// Negative logic bump sensors
// P4.7 Bump5, left side of robot
// P4.6 Bump4
// P4.5 Bump3
// P4.3 Bump2
// P4.2 Bump1
// P4.0 Bump0, right side of robot

//edge triggered interrupts on P4 are bound to ISER1, P4 interrupts are on interrupt 38
#include <stdint.h>
#include "msp.h"
#define BUMP_PORT P4
#define BUMP_PIN_0 BIT0
#define BUMP_PIN_1 BIT2
#define BUMP_PIN_2 BIT3
#define BUMP_PIN_3 BIT5
#define BUMP_PIN_4 BIT6
#define BUMP_PIN_5 BIT7
#define ALL_BUMP_PINS (BUMP_PIN_0 | BUMP_PIN_1 | BUMP_PIN_2 | BUMP_PIN_3 | BUMP_PIN_4 | BUMP_PIN_5)

#define BUMP_INITIALIZE_PIN0_SEL0 BUMP_PORT->SEL0 &= ~BUMP_PIN_0
#define BUMP_INITIALIZE_PIN0_SEL1 BUMP_PORT->SEL1 &= ~BUMP_PIN_0
#define BUMP_INITIALIZE_PIN1_SEL0 BUMP_PORT->SEL0 &= ~BUMP_PIN_1
#define BUMP_INITIALIZE_PIN1_SEL1 BUMP_PORT->SEL1 &= ~BUMP_PIN_1
#define BUMP_INITIALIZE_PIN2_SEL0 BUMP_PORT->SEL0 &= ~BUMP_PIN_2
#define BUMP_INITIALIZE_PIN2_SEL1 BUMP_PORT->SEL1 &= ~BUMP_PIN_2
#define BUMP_INITIALIZE_PIN3_SEL0 BUMP_PORT->SEL0 &= ~BUMP_PIN_3
#define BUMP_INITIALIZE_PIN3_SEL1 BUMP_PORT->SEL1 &= ~BUMP_PIN_3
#define BUMP_INITIALIZE_PIN4_SEL0 BUMP_PORT->SEL0 &= ~BUMP_PIN_4
#define BUMP_INITIALIZE_PIN4_SEL1 BUMP_PORT->SEL1 &= ~BUMP_PIN_4
#define BUMP_INITIALIZE_PIN5_SEL0 BUMP_PORT->SEL0 &= ~BUMP_PIN_5
#define BUMP_INITIALIZE_PIN5_SEL1 BUMP_PORT->SEL1 &= ~BUMP_PIN_5

#define BUMP_INTERRUPT_INIT_HTOL  BUMP_PORT->IES  |= ALL_BUMP_PINS //high to low transition trigger
#define BUMP_INTERNAL_RESISTORS BUMP_PORT->REN |= ALL_BUMP_PINS //internal resistors enabled
#define BUMP_SET_R_PULLUP BUMP_PORT->OUT |= ALL_BUMP_PINS //internal resistors set to pull-up

#define BUMP_NVIC_ENABLE NVIC->ISER[1] |= 0x00000040 //enable interrupt 38 in the NVIC
//to set priority 0, just zero out the 3rd and 4th bytes from the left
#define BUMP_PRIORITY_SET NVIC->IP[9] &= ~0x00ff0000 //priority 0
#define ENABLE_BUMP_PORT_INTERRUPT BUMP_PORT->IE |= ALL_BUMP_PINS //interrupts associated with each bump pin enabled
#define CONFIGURE_BUMP_DIR_INPUT BUMP_PORT->DIR &= ~ALL_BUMP_PINS
#define CLEAR_BUMP_INTERRUPT_FLAG BUMP_PORT->IFG &= ~ALL_BUMP_PINS

void (*BumpReact)(uint8_t);

// Initialize Bump sensors
// Make six Port 4 pins inputs
// Activate interface pullup
// pins 7,6,5,3,2,0
// Interrupt on falling edge (on touch)
//INTISR[38] handles i/o port P4
//P4 -> REN |= 0xED;  //enable resistors
//P4 -> OUT |= 0xED;  //resistors are pullup
void BumpInt_Init(void(*task)(uint8_t)){
    // write this as part of Lab 14

    BUMP_INITIALIZE_PIN0_SEL0;
    BUMP_INITIALIZE_PIN0_SEL1;
    BUMP_INITIALIZE_PIN1_SEL0;
    BUMP_INITIALIZE_PIN1_SEL1;
    BUMP_INITIALIZE_PIN2_SEL0;
    BUMP_INITIALIZE_PIN2_SEL1;
    BUMP_INITIALIZE_PIN3_SEL0;
    BUMP_INITIALIZE_PIN3_SEL1;
    BUMP_INITIALIZE_PIN4_SEL0;
    BUMP_INITIALIZE_PIN4_SEL1;
    BUMP_INITIALIZE_PIN5_SEL0;
    BUMP_INITIALIZE_PIN5_SEL1;
    CONFIGURE_BUMP_DIR_INPUT;

    BUMP_INTERNAL_RESISTORS;
    BUMP_SET_R_PULLUP;

    ENABLE_BUMP_PORT_INTERRUPT;
    BUMP_INTERRUPT_INIT_HTOL;
    BUMP_NVIC_ENABLE;
    BUMP_PRIORITY_SET;

    CLEAR_BUMP_INTERRUPT_FLAG;

    BumpReact = task;

}
// Read current state of 6 switches
// Returns a 6-bit positive logic result (0 to 63)
// bit 5 Bump5
// bit 4 Bump4
// bit 3 Bump3
// bit 2 Bump2
// bit 1 Bump1
// bit 0 Bump0
uint8_t Bump_Read(void){
    // write this as part of Lab 14
    uint8_t data = (P4 -> IN & 0xED);
    uint8_t result = 0;
    // 0t 0t 0t 0 0t 0t 0 0t
    // 0x80t  0x40t  0x20t  0x10  0x08t 0x04t  0x02  0x01t
    result = 1*(data&0x01) + 2*((data&0x04) >> 2) + 4*((data&0x08) >> 3) + 8*((data&0x20) >> 5) + 16*((data&0x40) >> 6) + 32*((data&0x80 )>> 7);
    return result; // replace this line p replaced
}
// we do not care about critical section/race conditions
// triggered on touch, falling edge
void PORT4_IRQHandler(void){
    // write this as part of Lab 14
    CLEAR_BUMP_INTERRUPT_FLAG;
    BumpReact(Bump_Read());


}
