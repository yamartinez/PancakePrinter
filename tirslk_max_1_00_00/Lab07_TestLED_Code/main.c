/*****************************************************************************
*
* Copyright (C) 2013 - 2017 Texas Instruments Incorporated - http://www.ti.com/
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* * Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
* * Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the
*   distribution.
*
* * Neither the name of Texas Instruments Incorporated nor the names of
*   its contributors may be used to endorse or promote products derived
*   from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
******************************************************************************
*
* MSP432
*
******************************************************************************/
//the
#include "msp.h"



struct State {
  uint32_t out;                // 2-bit output
  uint32_t delay;              // time to delay in 1ms
  const struct State *next[4]; // Next if 2-bit input is 0-3
};

typedef const struct State State_t;

#define Center    &fsm[0]
#

#define Right1 &fsm[1]
#define Right2 &fsm[2]
#define HardL &fsm[3]
#define Rstraight &fsm[4]
#define Stop &fsm[5]
#define Left1 &fsm[6]
#define Left2 &fsm[7]
#define HardR &fsm[8]
#define Lstraight &fsm[9]

State_t fsm[10]={ //the outputs need to correspond to the colors
  {0x07, 500, { Stop,Left1,Right1,Center}},  //Center - white
  {0x01, 500, { HardL,Left1,Right1,Center}},  //Right1 - red
  {0x05, 500, { HardL,Left1,Right1,Center}},  //Right2 - pink
  {0x03, 5000, { Rstraight,Rstraight,Rstraight,Rstraight}},  //HardL - yellow
  {0x02, 5000, { Stop,Right1,Right1,Center}},  //Rstraight - green, blinking
  {0x00, 5000, { Stop,Stop,Stop,Stop}},  //Stop - dark
  {0x04, 500, { HardR,Left2,Right1,Center}},  //Left1 - blue
  {0x06, 500, { HardR,Left1,Right1,Center}},  //Left2 - cyan
  {0x02, 5000, { Lstraight,Lstraight,Lstraight,Lstraight}},  //HardR - green
  {0x03, 5000, { Stop,Left1,Right1,Center}}   //Lstraight - blinking yellow
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
int main(void){ uint32_t heart=0;

  Clock_Init48MHz();
  LaunchPad_Init();
  //TExaS_Init(LOGICANALYZER);  // optional
  Spt = Center;
  while(1){
    Output = Spt->out;            // set output from FSM
    LaunchPad_Output(Output);     // do output to two motors
   // TExaS_Set(Input<<2|Output);   // optional, send data to logic analyzer
    Clock_Delay1ms(Spt->delay);   // wait
    Input = LaunchPad_Input();//reflectance_center?    // read sensors - how to take input, reflectance???
    Spt = Spt->next[Input];       // next depends on input and state
    heart = heart^Spt -> out;
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
