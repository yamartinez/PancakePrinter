// Lab07_FSMmain.c
// Runs on MSP432
// Student version of FSM lab, FSM with 2 inputs and 2 outputs.
// Rather than real sensors and motors, it uses LaunchPad I/O
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

#include <stdint.h>
#include "msp.h"
#include "../inc/clock.h"
#include "../inc/LaunchPad.h"
#include "../inc/Texas.h"

/*(Left,Right) Motors, call LaunchPad_Output (positive logic)
3   1,1     both motors, yellow means go straight
2   1,0     left motor,  green  means turns right
1   0,1     right motor, red    means turn left
0   0,0     both off,    dark   means stop
(Left,Right) Sensors, call LaunchPad_Input (positive logic)
3   1,1     both buttons pushed means on line,
2   1,0     SW2 pushed          means off to right - oscillate for 1,0 between 11 and 01
1   0,1     SW1 pushed          means off to left - oscillate for 0,1 between 11 and 10
0   0,0     neither button      means lost - if it was 11 before, stop, if it was 10 or 01, enter lost mode
 */
/* start of testcode */
struct State {
  uint32_t out;                // 2-bit output
  uint32_t delay;              // time to delay in 1ms
  const struct State *next[4]; // Next if 2-bit input is 0-3
};

typedef const struct State State_t;
#define notOff 3
#define offLeft 2
#define offRight 1
#define lost 0

#define Center &fsm[0]

#define Right1 &fsm[1]
#define Right2 &fsm[2]

#define HardL &fsm[3]

#define Rstraight &fsm[4]

#define Stop &fsm[5]

#define Left1 &fsm[6]
#define Left2 &fsm[7]

#define HardR &fsm[8]

#define Lstraight &fsm[9]
 // the nested array is indexed 0-3, which is what reflectance can return if we only use 2 bits
//problem 1- we want to use all 8 bits, which would necessitate a great deal of struggle in creating many possible "next states"
//solution 1- make most of them invalid inputs and have them return to the current state
//problem 2- what states do we actually need? what can we ignore?
//solution 2- I believe we can make a few left side states and a few right side states so that in the case of the bot looking like this
/**
 *         ___________________
 *       /   o o o o o o o o   \
 *       |                     |
 *       |                     |
 *        \____________________/
 *  we can hopefully always assume that only one wing of these should be lit at the same time. so i can assume the inputs will range from
 *
 *          1 1 1 1 1 0 0 0
 *       to 0 0 0 1 1 0 0 0
 *       to 0 0 0 1 1 1 1 1
 *       and possibly one state where we look like 1 1 1 1 1 1 1 1
 *  perhaps a readLeft() and readRight() set of methods are called for, so we can base our logic off 5 bits of input instead of 8 bits
 *  alternatively -- one possibility is that we instead choose to simplify our decision making process -- mapping the 8 bit dataset we have to
 *  the 4 existing possibilities - with a few special cases that trigger specialized out of state behavior before resuming regular behavior
 *
 */
State_t fsm[10]={ //the outputs need to correspond to the colors
  {0x07, 500, { Stop,Left1,Right1,Center}},  //Center - white
  {0x01, 500, { HardL,Left1,Right2,Center}},  //Right1 - red (we need to oscillate between these two right states)
  {0x05, 500, { HardL,Left1,Right1,Center}},  //Right2 - pink
  {0x03, 5000, { Lstraight,Lstraight,Lstraight,Lstraight}},  //HardL - yellow
  {0x02, 5000, { Stop,Right1,Right1,Center}},  //Rstraight - green & baseLED is on
  {0x00, 5000, { Stop,Stop,Stop,Stop}},  //Stop - dark
  {0x04, 500, { HardR,Left2,Right1,Center}},  //Left1 - blue
  {0x06, 500, { HardR,Left1,Right1,Center}},  //Left2 - cyan
  {0x02, 5000, { Rstraight,Rstraight,Rstraight,Rstraight}},  //HardR - green
  {0x03, 5000, { Stop,Left1,Right1,Center}}   //Lstraight - yellow & baseLED is on
};
// built-in LED1 connected to P1.0
// negative logic built-in Button 1 connected to P1.1
// negative logic built-in Button 2 connected to P1.4
// built-in red LED connected to P2.0
// built-in green LED connected to P2.1
// built-in blue LED connected to P2.2
// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// cyan     -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05

/*Run FSM continuously
1) Output depends on State (LaunchPad LED)
2) Wait depends on State
3) Input (LaunchPad buttons)
4) Next depends on (Input,State)
 */

State_t *Spt;  // pointer to the current state
uint32_t Input;
uint32_t Output;
int main(void){ //uint32_t heart=0;

  Clock_Init48MHz();
  LaunchPad_Init();
  //TExaS_Init(LOGICANALYZER);  // optional
  Spt = Center;
  while(1){
    Output = Spt->out;            // set output from FSM
    LaunchPad_Output(Output);     // do output to two motors

    if((Output == 0x03 && Spt -> next[Input] == Stop) || (Output == 0x02 && Spt-> next[Input] == Stop) ){
        LaunchPad_LED(1);}
    else
    { LaunchPad_LED(0);}
   // TExaS_Set(Input<<2|Output);   // optional, send data to logic analyzer
    Clock_Delay1ms(Spt->delay);   // wait
    Input = LaunchPad_Input();//reflectance_center?    // read sensors - how to take input, reflectance???
    Spt = Spt->next[Input];       // next depends on input and state
    //heart = heart^Spt -> out;
    //LaunchPad_LED(heart);         // optional, debugging heartbeat
  }
}

// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05
/**/ //end of testcode



/* this is real
// Linked data structure
struct State {
  uint32_t out;                // 2-bit output
  uint32_t delay;              // time to delay in 1ms
  const struct State *next[4]; // Next if 2-bit input is 0-3
};
typedef const struct State State_t;

#define Center    &fsm[0]
#define Left      &fsm[1]
#define Right     &fsm[2]
#define Left2     &fsm[3]
#define Right2    &fsm[4]
#define LostLeft  &fsm[5]
#define LostLeft2 &fsm[6]
#define LostRight &fsm[7]
#define LostRight2 &fsm[8]
#define Stop      &fsm[9]
// with all these states defined, we can now simulate the behavior we wanted to be simulating

State_t fsm[10]={
  {0x03, 500, { Stop, Left,   Right,  Center }},  // Center
  {0x02, 500, { LostLeft,  Left2, Right,  Center }},  // Left
  {0x01, 500, { LostRight, Left,   Right2, Center }},  // Right
  {0x03, 500, { LostLeft, Left, Right, Center }},  // Left2
  {0x03, 500, { LostRight, Left,Right, Center }},  // Right2
  {0x02, 5000, { LostLeft2, Left,Right, Center }},  // LostLeft
  {0x03, 5000, { Stop, Left, Right, Center}},  // LostLeft2
  {0x01, 5000, { LostRight2, Left, Right, Center}},  // LostRight
  {0x03, 5000, { Stop, Left, Right, Center}},  // LostRight2
  {0x00, 5000, { Stop, Stop, Stop, Stop }},  // Stop
};

State_t *Spt;  // pointer to the current state
uint32_t Input;
uint32_t Output;
//Run FSM continuously
//1) Output depends on State (LaunchPad LED)
//2) Wait depends on State
//3) Input (LaunchPad buttons)
//4) Next depends on (Input,State)

int main(void){ uint32_t heart=0;
  Clock_Init48MHz();
  LaunchPad_Init();
  TExaS_Init(LOGICANALYZER);  // optional
  Spt = Center;
  while(1){
    Output = Spt->out;            // set output from FSM
    LaunchPad_Output(Output);     // do output to two motors
    TExaS_Set(Input<<2|Output);   // optional, send data to logic analyzer
    Clock_Delay1ms(Spt->delay);   // wait
    Input = LaunchPad_Input();//reflectance_center?    // read sensors - how to take input, reflectance???
    Spt = Spt->next[Input];       // next depends on input and state
    heart = heart^1;
    LaunchPad_LED(heart);         // optional, debugging heartbeat
  }
}

// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// sky blue -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05
*/ //end of realcode
