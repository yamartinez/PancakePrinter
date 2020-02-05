// PeriodicSysTickIntsMain.c
// Runs on MSP432
// Use the SysTick timer to request interrupts at a particular period.
// Jonathan Valvano
// July 1, 2017

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


// built-in LED1 connected to P1.0
// P1.0, P2.0 are an output to profiling scope/logic analyzer

#include <stdint.h>
#include "msp.h"
#include "..\inc\CortexM.h"
#include "..\inc\SysTickInts.h"
#include "..\inc\LaunchPad.h"
#include "..\inc\Clock.h"

volatile uint32_t Time,MainCount;
#define LEDOUT (*((volatile uint8_t *)(0x42098040)))
void SysTick_Handler(void){
  LEDOUT ^= 0x01;       // toggle P1.0
  LEDOUT ^= 0x01;       // toggle P1.0
  Time = Time + 1;
  LEDOUT ^= 0x01;       // toggle P1.0
}


int main(void){
  Clock_Init48MHz();      // running on crystal
  Time = MainCount = 0;
  SysTick_Init(6000000,2);  // set up SysTick for 8 Hz interrupts
  LaunchPad_Init();       // P1.0 is red LED on LaunchPad
  EnableInterrupts();
  while(1){
    WaitForInterrupt();
    P2->OUT ^= 0x01; // foreground thread
    MainCount++;
  }
}

uint32_t High=120000,Low=360000;
void SysTick_Handler2(void){ // PWM
  LEDOUT ^= 0x01;   // toggle red LED
  if(LEDOUT){  // if high
     SysTick->LOAD = High; // time while high
  }else{
     SysTick->LOAD = Low;  // time while low
  }
  SysTick->VAL = 0;        // any value written to counter clears
  Time = Time + 1;
}
