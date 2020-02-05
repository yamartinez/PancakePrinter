// Motor.c
// Runs on MSP432
// Provide mid-level functions that initialize ports and
// set motor speeds to move the robot. Lab 13 solution
// Daniel Valvano
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

// Left motor direction connected to P5.4 (J3.29)
// Left motor PWM connected to P2.7/TA0CCP4 (J4.40)
// Left motor enable connected to P3.7 (J4.31)
// Right motor direction connected to P5.5 (J3.30)
// Right motor PWM connected to P2.6/TA0CCP3 (J4.39)
// Right motor enable connected to P3.6 (J2.11)

#include <stdint.h>
#include "msp.h"
#include "../inc/CortexM.h"
#include "../inc/PWM.h"

// *******Lab 13 solution*******

// ------------Motor_Init------------
// Initialize GPIO pins for output, which will be
// used to control the direction of the motors and
// to enable or disable the drivers.
// The motors are initially stopped, the drivers
// are initially powered down, and the PWM speed
// control is uninitialized.
// Input: none
// Output: none
int leftDrive = 0;
int rightDrive = 0;

/*
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
    if(rightDrive)
    Cycle_Right();
    if(leftDrive)
    Cycle_Left();
}*/

/*void PWM_Init_custom(uint16_t period, uint16_t duty1, uint16_t duty2){
P2->DIR |= 0xC0;          // P2.6, P2.7 output
P2->SEL0 |= 0xC0;       // P2.6, P2.7 TimerA0 function w/ gpio
P2->SEL1 &= ~0xC0;  // P2.6, P2.7 TimerA0 function w/gpio

leftDuty = duty1; //change global variables that track duty cycles for the motors.
rightDuty = duty2;

TIMER_A0->CCTL[0] = 0x0080;   // CCI1 toggle mode
TIMER_A0->CCR[0] = period;       // Period is 2*period*8*83.33ns is 1.333*period
TIMER_A0->EX0 = 0x0000;      //    divide by 1
TIMER_A0->CCTL[3] = 0x0040;   // CCR1 toggle/reset
TIMER_A0->CCR[3] = duty1;        // CCR1 duty cycle is duty1/period
TIMER_A0->CCTL[4] = 0x0040;  // CCR2 toggle/reset
TIMER_A0->CCR[4] = duty2;     // CCR2 duty cycle is duty2/period
TIMER_A0->CTL = 0x02F0;
}*/

void Motor_Init(void){
  // write this as part of Lab 13
    P5->SEL0 &= ~0x30;
    P5->SEL1 &= ~0x30;    //  P5.4 & P5.5 as GPIO
    P5->DIR |=  0x30;    //  make P5.4 & P5.5 out
    P5->OUT &= ~0x30;   //low - default direction forward
    P2->DIR |= 0xC0;          // P2.6, P2.7 output
    P2->SEL0 |= 0xC0;       // P2.6, P2.7 TimerA0 function w/ gpio
    P2->SEL1 &= ~0xC0;  // P2.6, P2.7 TimerA0 function w/gpio
    /*
    P2->SEL0 &= ~0xC0;    //  make p2.7 & P2.6 gpio
    P2->SEL1 &= ~0xC0;  //THESE ARE THE ENABLE BITS FOR THE MOTORS
    P2->DIR |= 0xC0;    //output
    */
    //what do we want timer A0 to do? it should toggle one or
    //more motors on or off depending on which
    //function it is called from.

    PWM_Init34(15000, 0, 0);
    P2->OUT &= ~0xC0;   //enable LOW for motors
    P3->SEL0 &= ~0xC0; //p3.7 & P3.6 gpio
    P3->SEL1 &= ~0xC0;
    P3->DIR |= 0xC0;   //output
    P3->OUT &= ~0xC0;  //nsleep  3.6 and 3.7 LOW

    return;
}

/*
 *@param 1 for activate left wheel
 *@param 2 for activate right wheel
 *@param 3 for activate BOTH wheel
 */

void forward(int i){
    //in order to go forward, nsleep must be 1, enable must be 1, and PH must be 0
    //nsleep is p3, enable is p2, ph is p1
    if(i == 1)
    {
        P3->OUT |=  0x80; //nsleep = 1
        P2->OUT |=  0x80; //enable  = 1
        P5->OUT &= ~0x10; //direction = 0
        leftDrive = 1;
    }
    if(i == 2)
        {
            P3->OUT |=  0x40; //nsleep = 1
            P2->OUT |=  0x40; //enable = 1
            P5->OUT &= ~0x20; //direction = 0
            rightDrive = 1;
        }
    if(i == 3)
    {
        P3->OUT |=  0xC0; //nsleep = 1
        P2->OUT |=  0xC0; //enable = 1
        P5->OUT &= ~0x30; //direction = 0
        leftDrive = 1;
        rightDrive = 1;
    }
}
void backward(int i){
    //in order to go backward, nsleep must be 1, enable must be 1, and PH must be 1
    //nsleep is p3, enable is p2, ph is p1
    if(i == 1)
    {
        P3->OUT |= 0x80; //nsleep = 1
        P2->OUT |= 0x80; //enable  = 1
        P5->OUT |= 0x10; //direction = 1
        leftDrive = 1;
    }
    if(i == 2)
        {
            P3->OUT |= 0x40; //nsleep = 1
            P2->OUT |= 0x40; //enable = 1
            P5->OUT |= 0x20; //direction = 1
            rightDrive = 1;
        }
    if(i == 3)
    {
        P3->OUT |= 0xC0; //nsleep = 1
        P2->OUT |= 0xC0; //enable = 1
        P5->OUT |= 0x30; //direction = 1
        leftDrive = 1;
        rightDrive = 1;
    }
}
// ------------Motor_Stop------------
// Stop the motors, power down the drivers, and
// set the PWM speed control to 0% duty cycle.
// Input: none
// Output: none
// as a reminder, nsleep and enable determine whether or not the motors will move - nsleep turns off to save power
// p5 will determine the direction of the motion, forward is LOW
// p3 is nsleep
// p2 is enable
void Motor_Stop(uint16_t leftDuty, uint16_t rightDuty){
  // write this as part of Lab 13
  P3 -> OUT &= ~0xC0;
  P2 -> OUT &= ~0xC0;
  PWM_Duty3(0);
  PWM_Duty4(0);
  //direction is not altered, as when it starts moving again it will likely receive new directional commands.
}

// ------------Motor_Forward------------
// Drive the robot forward by running left and
// right wheels forward with the given duty
// cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Forward(uint16_t leftDuty, uint16_t rightDuty){ 
  // write this as part of Lab 13
    PWM_Duty3(rightDuty);
    PWM_Duty4(leftDuty);
    forward(3);

}

// ------------Motor_Right------------
// Turn the robot to the right by running the
// left wheel forward and the right wheel
// backward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Right(uint16_t leftDuty, uint16_t rightDuty){ 
  // write this as part of Lab 13
    PWM_Duty3(rightDuty);
    PWM_Duty4(leftDuty);
    forward(2);
    backward(1);

}

// ------------Motor_Left------------
// Turn the robot to the left by running the
// left wheel backward and the right wheel
// forward with the given duty cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Left(uint16_t leftDuty, uint16_t rightDuty){ 
  // write this as part of Lab 13
    PWM_Duty3(rightDuty);
    PWM_Duty4(leftDuty);
    forward(1);
    backward(2);
}

// ------------Motor_Backward------------
// Drive the robot backward by running left and
// right wheels backward with the given duty
// cycles.
// Input: leftDuty  duty cycle of left wheel (0 to 14,998)
//        rightDuty duty cycle of right wheel (0 to 14,998)
// Output: none
// Assumes: Motor_Init() has been called
void Motor_Backward(uint16_t leftDuty, uint16_t rightDuty){ 
  // write this as part of Lab 13
    PWM_Duty3(rightDuty);
    PWM_Duty4(leftDuty);
    backward(3);
}
