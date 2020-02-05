// Lab09_SysTickmain.c
// Runs on MSP432
// Student version of SysTick, lab9
// Daniel and Jonathan Valvano
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


// built-in LED1 connected to P1.0
// negative logic built-in Button 1 connected to P1.1
// negative logic built-in Button 2 connected to P1.4
// built-in red LED connected to P2.0
// built-in green LED connected to P2.1
// built-in blue LED connected to P2.2
// RC circuit connected to P2.6, to create DAC
#include <stdint.h>
#include "msp.h"
#include "..\inc\TExaS.h"
#include "..\inc\Clock.h"
#include "..\inc\CortexM.h"
#include "..\inc\SysTick.h"
#include "..\inc\LaunchPad.h"
#define active &fsm[0]
#define inactive &fsm[1]
#define on 1
#define off 0
#define period 10000
#define SW1_MASK = 0x01
#define SW2_MASK = 0x10
void nextState(void);



uint32_t const DutyBuf[100]={
    240000, 255063, 270067, 284953, 299661, 314133, 328313, 342144, 355573, 368545,
    381010, 392918, 404223, 414880, 424846, 434083, 442554, 450226, 457068, 463053,
    468158, 472363, 475651, 478008, 479427, 479900, 479427, 478008, 475651, 472363,
    468158, 463053, 457068, 450226, 442554, 434083, 424846, 414880, 404223, 392918,
    381010, 368545, 355573, 342144, 328313, 314133, 299661, 284953, 270067, 255063,
    240000, 224937, 209933, 195047, 180339, 165867, 151687, 137856, 124427, 111455,
    98990, 87082, 75777, 65120, 55154, 45917, 37446, 29774, 22932, 16947,
    11842, 7637, 4349, 1992, 573, 100, 573, 1992, 4349, 7637,
    11842, 16947, 22932, 29774, 37446, 45917, 55154, 65120, 75777, 87082,
    98990, 111455, 124427, 137856, 151687, 165867, 180339, 195047, 209933, 224937
};
const uint32_t PulseBuf[100]={
    5000, 5308, 5614, 5918, 6219, 6514, 6804, 7086, 7361, 7626,
    7880, 8123, 8354, 8572, 8776, 8964, 9137, 9294, 9434, 9556,
    9660, 9746, 9813, 9861, 9890, 9900, 9890, 9861, 9813, 9746,
    9660, 9556, 9434, 9294, 9137, 8964, 8776, 8572, 8354, 8123,
    7880, 7626, 7361, 7086, 6804, 6514, 6219, 5918, 5614, 5308,
    5000, 4692, 4386, 4082, 3781, 3486, 3196, 2914, 2639, 2374,
    2120, 1877, 1646, 1428, 1224, 1036,  863,  706,  566,  444,
     340,  254,  187,  139,  110,  100,  110,  139,  187,  254,
     340,  444,  566,  706,  863, 1036, 1224, 1428, 1646, 1877,
    2120, 2374, 2639, 2914, 3196, 3486, 3781, 4082, 4386, 4692};

struct State {
  void(*task)(void);     // function that will carry out the output and change to the next state
  uint32_t delay;
  const struct State *next[4]; // Next if 2-bit input is 0-3, can ignore repeated attempts to stop or to start.
}; typedef const struct State State_t;


uint32_t PULSE_COUNT;
uint32_t LP_INPUT = 0;

void SysTick_Wait1us(uint32_t delay){
    // write this code
      SysTick_Wait(48*delay);  // wait 1us*the delay value (assumes 48 MHz clock)
}

void outputStart(void){
    uint32_t DUTY_TIME = PulseBuf[PULSE_COUNT];
    LaunchPad_Output(on);
    SysTick_Wait1us(DUTY_TIME);
    LaunchPad_Output(off);
    SysTick_Wait1us(period - DUTY_TIME);
    PULSE_COUNT++;
    if(PULSE_COUNT == 100)
        PULSE_COUNT =0 ;
    nextState();
}
void outputStop(void){
    LaunchPad_Output(off);
    nextState();
}

State_t fsm[2]={
   { &outputStart , 0 , {active, active, inactive, active}},  //active state
   { &outputStop  , 0 , {inactive, active, inactive, inactive}} };//inactive state}
//I've chosen to set mine up such that it will always ignore actions that are not explicitly
//turning on or off the light - ie both switches on or both switches off will cause no change.

State_t *Spt;

void nextState(void){
    Spt = Spt->next[LP_INPUT];
}

/*
 *
 * intake current values
 * generate outputs based on current state using function pointers (difficult)
 * generate the next state using the current values (also in said function pointer
 *
 */




//let's solve this with a pointermap FSM to create the heartbeat pattern


int Program9_1(void){
  Clock_Init48MHz();  // makes bus clock 48 MHz
  SysTick_Init();
  LaunchPad_Init();   // buttons and LEDs
  TExaS_Init(LOGICANALYZER_P1);
  while(1){
    P1->OUT |= 0x01;   // red LED on
    SysTick_Wait1us(7500);
    P1->OUT &= ~0x01;  // red LED off
    SysTick_Wait1us(2500);
  }
}

int Program9_2(void){uint32_t H,L;
  Clock_Init48MHz();  // makes bus clock 48 MHz
  SysTick_Init();
  TExaS_Init(SCOPE);
  P2->SEL0 &= ~0x40;
  P2->SEL1 &= ~0x40; // 1) configure P2.6 as GPIO
  P2->DIR |= 0x40;   // P2.6 output
  H = 7500;
  L = 10000-H;
  while(1){
    P2->OUT |= 0x40;   // on
    SysTick_Wait1us(H);
    P2->OUT &= ~0x40;  // off
    SysTick_Wait1us(L);
  }
}




// Operation
// The heartbeat starts when the operator pushes Button 1
// The heartbeat stops when the operator pushes Button 2
// When beating, the P1.0 LED oscillates at 100 Hz (too fast to see with the eye)
//  and the duty cycle is varied sinuosoidally once a second
int main(void){
  Clock_Init48MHz(); // makes it 48 MHz
  TExaS_Init(LOGICANALYZER_P1);
  LaunchPad_Init();   // buttons and LEDs
  SysTick_Init();
  // write this code
  Spt = inactive;
  void (*fpointer)(void);
  fpointer = Spt -> task;
  LP_INPUT = 0;
  PULSE_COUNT = 0;
  EnableInterrupts();
  while(1){
        LP_INPUT = LaunchPad_Input();
        fpointer = Spt -> task;
        (*fpointer)();
//     Input = LaunchPad_Input();
//      if(Input == 0x01) { on = 1; }
//      on = (Input == 0x02) ? 0 : on;
//      if( !on ){}
//      else{
//      H = PulseBuf[H_count];
//      L = 10000 - H;
//      LaunchPad_LED(1);
//      SysTick_Wait1us(H);
//      LaunchPad_LED(0);
//      SysTick_Wait1us(L);
//      H_count++;
//      if(H_count == 100){ H_count = 0;}
      }
  }






