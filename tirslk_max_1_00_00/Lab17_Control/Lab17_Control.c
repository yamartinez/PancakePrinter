// Lab17_Control.c
// Runs on MSP432
// Implementation of the control system.
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
#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/PWM.h"
#include "../inc/LaunchPad.h"
#include "../inc/UART0.h"
#include "../inc/Motor.h"
#include "../inc/Bump.h"
#include "../inc/ADC14.h"
#include "../inc/TimerA1.h"
#include "../inc/IRDistance.h"
#include "../inc/Nokia5110.h"
#include "../inc/LPF.h"
#include "../inc/SysTickInts.h"
#include "../inc/Tachometer.h"
#include "../inc/Reflectance.h"
#include "../inc/BumpInt.h"

#define maxDuty 8800
#define minDuty 2
#define rpmConstant 2000000
uint16_t leftTach = 0;
int32_t leftSteps = 0;

uint16_t rightTach= 0;
int32_t rightSteps= 0;

enum TachDirection leftDir = STOPPED;
enum TachDirection rightDir = STOPPED;

int32_t desiredRightPeriod;
int32_t desiredLeftPeriod;
int32_t rightPeriod;
int32_t leftPeriod;
int32_t rightError;
int32_t leftError;

uint32_t rightDuty = 2000;
uint32_t leftDuty = 2000;
volatile uint32_t realleftDuty ;
volatile uint32_t realrightDuty ;

volatile uint32_t ADCval;
volatile uint32_t ADCsem;

//*****************************************************
void adjustSpeed(int32_t rightError, int32_t leftError){
//closed-loop speed controls incrementing at +- 1 duty cycle from dutyMin to dutyMax
    if(rightError == 0){
    }
    else if(rightError > 0){
        realrightDuty++;
    }
    else if(rightError < 0){
        realrightDuty--;
    }

    if(leftError == 0){}
    else if(leftError > 0){
        realleftDuty++;
    }
    else if(leftError < 0){
        realleftDuty--;
    }

    if( realleftDuty >= maxDuty){
        realleftDuty = maxDuty;}

    else if( realleftDuty <= minDuty){
        realleftDuty = minDuty;}

    if( realrightDuty >= maxDuty){
        realrightDuty = maxDuty;}

    else if( realrightDuty <= minDuty){
        realrightDuty = minDuty;}
}
//******************************************************
// Distance to wall proportional control
// Incremental speed control
// Integral control, Line follower
void Controller(void){
// write this as part of Lab 17
    //steps to execute - check tachometer & act
    rightPeriod = 0;
    leftPeriod = 0;
Tachometer_Get(&leftTach, &leftDir, &leftSteps, &rightTach, &rightDir, &rightSteps);
 rightPeriod = rpmConstant/rightTach;
 leftPeriod = rpmConstant/leftTach;
desiredRightPeriod = 30000;
desiredLeftPeriod = 30000;
rightError = desiredRightPeriod - rightPeriod;
leftError = desiredLeftPeriod - leftPeriod;
adjustSpeed(rightError, leftError);

}
volatile uint8_t dontgo;
volatile uint32_t rightDistance, leftDistance, centerDistance;
void distanceReact(uint32_t rightDistance, uint32_t centerDistance, uint32_t leftDistance){
    if(rightDistance <= 20 || centerDistance <= 20 || leftDistance <= 20){
        dontgo = 1;}
    else
    {
        dontgo = 0;
    }
}
//ISR to run at ~2 kHz
void distanceRead(void){
uint32_t sensorLeft; //currently on 9.1/A16
uint32_t sensorCenter; //currently on 6.1/A14
uint32_t sensorRight; //Currently on 9.0/A17
ADC_In17_14_16(&sensorRight, &sensorCenter, &sensorLeft);

rightDistance = RightConvert(LPF_Calc(sensorRight));
centerDistance = CenterConvert(LPF_Calc2(sensorCenter));
leftDistance = LeftConvert(LPF_Calc3(sensorLeft));
ADCsem = 1;
distanceReact(rightDistance, centerDistance, leftDistance);
}
#define TIMERA1_PERIOD 50
#define NUMBER_OF_TASKS 2
typedef struct{
    void (*Task)(void);
    uint32_t TaskCycleCounter;
    uint32_t TaskExecutionCycle;
} TaskType;

TaskType Tasks[NUMBER_OF_TASKS] = {
                                   {&Controller, 0, 100},
                                   {&distanceRead, 0 ,5},
};
void TaskSchedulerISR(void)
{
    unsigned int i;
    for(i = 0; i < NUMBER_OF_TASKS; i++) {
        Tasks[i].TaskCycleCounter++;
        if(Tasks[i].TaskCycleCounter == Tasks[i].TaskExecutionCycle)    {
            Tasks[i].TaskCycleCounter = 0;
            (*Tasks[i].Task)();
        }
    }
}
uint8_t CollisionData;
uint8_t CollisionFlag;
void HandleCollision(uint8_t bumpSensor){
   //Motor_Stop(realrightDuty, realleftDuty); //optional
   CollisionData = bumpSensor;
   CollisionFlag = 1;
}
void main(void){
  DisableInterrupts();
// initialization
  uint32_t raw17,raw14,raw16;
  uint32_t s;
  rightDuty = 3000;
  leftDuty = 3000;
  realrightDuty = rightDuty;
  realleftDuty = leftDuty;
  ADCsem = 0;
  dontgo = 0;
  s = 256;
  Clock_Init48MHz();
  LaunchPad_Init();
  Motor_Init();
  BumpInt_Init(&HandleCollision);
  ADC0_InitSWTriggerCh17_14_16();
  ADC_In17_14_16(&raw17, &raw14, &raw16);
  LPF_Init(raw17,s);
  LPF_Init2(raw14,s);
  LPF_Init3(raw16,s);
  Tachometer_Init();
  TimerA1_Init(&TaskSchedulerISR, TIMERA1_PERIOD);
// write this as part of Lab 17
  EnableInterrupts();

  while(1){
// write this as part of Lab 17
      if(CollisionFlag == 1)
      {
          CollisionFlag = 0;
          Motor_Stop(realrightDuty, realleftDuty);
      }
      else if(dontgo == 1){
          dontgo = 0;
          Motor_Stop(realrightDuty, realleftDuty);
      }
      else {
          Motor_Forward(realrightDuty, realleftDuty);
      }
  }
}

