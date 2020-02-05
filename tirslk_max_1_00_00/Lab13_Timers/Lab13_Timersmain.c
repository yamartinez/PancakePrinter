// Lab13_Timersmain.c
// Runs on MSP432
// Student starter code for Timers lab
// Daniel and Jonathan Valvano
// July 11, 2019
// PWM output to motor
// Second Periodic interrupt

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

// Left motor direction connected to P5.4 (J3.29)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P5.5 (J3.30)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

#include "msp.h"
#include "..\inc\bump.h"
#include "..\inc\Clock.h"
#include "..\inc\SysTick.h"
#include "..\inc\CortexM.h"
#include "..\inc\LaunchPad.h"
#include "..\inc\Motor.h"
#include "..\inc\TimerA1.h"
#include "..\inc\TExaS.h"



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
/* start of FSM testcode */
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
/*
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
*/


//pwm flipping functions
void Cycle_Left(void){
    if(!(P2 -> OUT | ~0x80)){
        //logic demonstrated in Cycle_Right()

        // this checks if left motor is on - since P2->OUT|~0x80 should have at least 1 non-zero bit if any motor is on
        //P2->out | ~0x80 should be all 1's unless the left motor is off - then it will have exactly 1 0 bit. The inverse then will be nonzero
        //since non-zero means true in logic, the contained actions in this body will activate if the left motor is OFF
        //so let's turn on the left motor here by setting p2->out to low
        P2->OUT |= 0x80;
    }

    else
        P2->OUT &= ~0x80; //if the motor IS off, turn it on.
}

void Cycle_Right(){
    if(!(P2 -> OUT | ~0x40)){  //if OFF
        // xxxx Rxxx
        //OR
        // 1111 0111
        // = 1111 R111
        // !1111 R111 = 0000 !R 000 == 0 unless R is HIGH
        //therefore this is true for R = 1,
        //but inverted, so false for R = 1, so it's if the right motor is off

        // this checks if right motor is on - since P2->OUT | ~0x40 should have at least 1 non-one bit if the right motor is on
        //P2->out | ~0x40 should be all 1's unless the right motor is off - then it will have exactly 1 0 bit. The inverse then will be nonzero
        //since non-zero means true in logic, the contained actions in this body will activate if the left motor is OFF
        //so let's turn on the left motor here by setting p2->out to low
        P2->OUT &= 0x40; //TURN ON
    }

    else //IF ON
        P2->OUT |= ~0x40; //if the motor IS ON, turn it OFF.
}
void Cycle_Motors(void){
    Cycle_Right();
    Cycle_Left();
}
void reverse(void){ //DEPRECATED
    LaunchPad_Output(0x01);
          Motor_Backward(3000,100);// reverse 1 sec
          LaunchPad_Output(0x03);
          Motor_Left(3000,200);   // right turn 2 sec
}
// Driver test
void TimedPause(uint32_t time){
  Clock_Delay1ms(time);          // run for a while and stop
  Motor_Stop();
  while(LaunchPad_Input()==0);  // wait for touch
  while(LaunchPad_Input());     // wait for release
}
int Program13_1(void){
  Clock_Init48MHz();
  LaunchPad_Init(); // built-in switches and LEDs
  Bump_Init();      // bump switches
  Motor_Init();     // your function
  while(1){
    TimedPause(4000);
    Motor_Forward(7500,7500);  // your function
    TimedPause(2000);
    Motor_Backward(7500,7500); // your function
    TimedPause(3000);
    Motor_Left(5000,5000);     // your function
    TimedPause(3000);
    Motor_Right(5000,5000);    // your function
  }
}

// Test of Periodic interrupt
#define REDLED (*((volatile uint8_t *)(0x42098060)))
#define BLUELED (*((volatile uint8_t *)(0x42098068)))
uint32_t Time;
void Task(void){
  REDLED ^= 0x01;       // toggle P2.0
  REDLED ^= 0x01;       // toggle P2.0
  Time = Time + 1;
  REDLED ^= 0x01;       // toggle P2.0
}
int Program13_2(void){
  Clock_Init48MHz();
  LaunchPad_Init();  // built-in switches and LEDs
  TimerA1_Init(&Task,10);  // 1000 Hz
  EnableInterrupts();
  while(1){
    BLUELED ^= 0x01; // toggle P2.1
  }
}
void BumpResponse(void){

    if(Bump_Read() != 63){
       DisableInterrupts();
        LaunchPad_Output(0x01);
        Motor_Backward(3000,3000);//reverse 1 sec
        Clock_Delay1ms(1000);
        LaunchPad_Output(0x03);
        Motor_Left(3000,3000); // right turn 2 sec
        Clock_Delay1ms(1500);
        EnableInterrupts();
        LaunchPad_Output(0);
    }
}





int main(void){
    // write a main program that uses PWM to move the robot
    // like Program13_1, but uses TimerA1 to periodically
    // check the bump switches and reflectance values, updating the FSM and

 Clock_Init48MHz();
 LaunchPad_Init();
 Bump_Init();

 //Reflectance_Init();
 TimerA1_Init(&BumpResponse, 1500);
 Motor_Init();
 EnableInterrupts();
  while(1){
    Motor_Forward(4000, 4000);
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

