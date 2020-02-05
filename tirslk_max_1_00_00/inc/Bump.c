// Bump.c
// Runs on MSP432
// Provide low-level functions that interface bump switches the robot.
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

#include <stdint.h>
#include "msp.h"
volatile uint8_t bumpVal; //tracks most recent bump value

// Initialize Bump sensors
// Make six Port 4 pins inputs
// Activate interface pullup
// pins 7,6,5,3,2,0
void Bump_Init(void){
    // write this as part of Lab 10
    //0xED = 1110 1101
    P4 -> SEL0 &= ~0xED; //P4 0,2,3,5,6,7 AS GPIO
    P4 -> SEL1 &= ~0xED;
    P4 -> DIR &= ~0xED;      //P4 0,2,3,5,6,7 as inputs
    P4 -> REN |= 0xED;  //enable resistors
    P4 -> OUT |= 0xED;  //resistors are pullup
    //pin init complete, what's next?
}

//// Read current state of 6 switches
//// Returns a 6-bit positive logic result (0 to 63)
//// bit 5 Bump5
//// bit 4 Bump4
//// bit 3 Bump3
//// bit 2 Bump2
//// bit 1 Bump1
//// bit 0 Bump0
uint8_t Bump_Read(void){
    // write this as part of Lab 10
    uint8_t data = (P4 -> IN & 0xED);
    uint8_t result = 0;
    // 0t 0t 0t 0 0t 0t 0 0t
    // 0x80t  0x40t  0x20t  0x10  0x08t 0x04t  0x02  0x01t
    result = 1*(data&0x01) + 2*((data&0x04) >> 2) + 4*((data&0x08) >> 3) + 8*((data&0x20) >> 5) + 16*((data&0x40) >> 6) + 32*((data&0x80 )>> 7);

    return result; // replace this line p replaced
}

//// Read current state of 6 switches
//// Returns a 6-bit positive logic result (0 to 63)
//// bit 5 Bump5
//// bit 4 Bump4
//// bit 3 Bump3
//// bit 2 Bump2
//// bit 1 Bump1
//// bit 0 Bump0
//void Bump_Read(){ // will run periodically on TimerA1 interrupts
//    uint8_t data =  (P4 -> IN & 0xED);
//    bumpVal = 1*(data&0x01) + 2*((data&0x04) >> 2) + 4*((data&0x08) >> 3) + 8*((data&0x20) >> 5) + 16*((data&0x40) >> 6) + 32*((data&0x80 )>> 7);
//    if(bumpVal != 63){
//        bumpSemaphore = 1;
//    }
//    else
//        bumpSemaphore = 0;
//}




