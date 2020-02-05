// Lab04_SoftwareDesignmain.c
// Runs on MSP432
// Solution to Software Design lab
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
enum scenario {
    Error = 0,
    LeftTooClose = 1,
    RightTooClose = 2,
    LeftAndRight = 3,
    CenterTooClose = 4,
    LeftAndCenter = 5,
    RightAndCenter = 6,
    LeftRightCenter = 7,
    Straight = 8,
    LeftTurn = 9,
    RightTurn = 10,
    TeeJoint = 11,
    LeftJoint = 12,
    RightJoint = 13,
    CrossRoad = 14,
    Blocked = 15
};
typedef enum scenario scenario_t;

#define SIDEMAX 354    // largest side distance to wall in mm
#define SIDEMIN 212    // smallest side distance to wall in mm
#define CENTEROPEN 600 // distance to wall between open/blocked
#define CENTERMIN 150  // min distance to wall in the front

//WRITE THIS CODE (down below so i can use convert)
/**
 *
scenario_t Classify(int32_t Left, int32_t Center, int32_t Right){
  scenario_t result=Error;
  int32_t iLeft = Convert(Left);
  int32_t iRight = Convert(Right);
  int32_t iCenter = Convert(Center);
  if()
  return result;
}
*/
#define IRSlope 1195172
#define IROffset -1058
#define IRMax 2552

int32_t Convert(int32_t n){
    // write this code
   //in case of the n value exceeding the max, just indicate you're at max distance.
  int32_t d = ((int32_t)IRSlope)/(n + (int32_t)IROffset); // this takes the input and converts using the named constants and previous equation
  if(d > 800)
          return 800;
  return d;
}

#define NOTURN_MIN 212
#define NOTURN_MAX 354
#define CENTER_NOT_BLOCKED_MIN 600
#define CENTER_MAX 800

scenario_t Classify(int32_t Left, int32_t Center, int32_t Right){
  scenario_t result=Error;
  int32_t iLeft = Left;
  int32_t iRight = Right;
  int32_t iCenter = Center;
  int32_t RTP, LTP, CNB;
  RTP = 0;
  LTP = 0;
  CNB = 0;

  if(Center > CENTER_MAX || Center < 50 || Right < 50 || Right >800 || Left < 50 || Right > 800)
      return Error;
  // danger scenarios
  if(iLeft < 212 ){
      result += LeftTooClose;}
  if(iRight < 212){
      result += RightTooClose;}
  if(iCenter < 212){
      result += CenterTooClose;}

  if(result != Error ){ // return if there are danger conditions
      return result;
  }

  //right turn possible int
  RTP = ( iRight >= NOTURN_MIN && iRight <= NOTURN_MAX ) ? 0 : 1;
  RTP =  ( iRight <= NOTURN_MIN ) ? 0 : RTP; // this line is redundant but why not leave it in amirite
  //left turn possible int
  LTP = ((NOTURN_MIN <= iLeft && iLeft <= NOTURN_MAX)) ? 0 : 1;
  LTP = (NOTURN_MIN <= iLeft ) ? 0 : LTP; // also redundant
  //center not blocked int
  CNB = ( iCenter >= CENTER_NOT_BLOCKED_MIN && iCenter <= CENTER_MAX ) ? 1 : 0;
  //scenarios where center > 600 mm {straight, right/left joint, crossroads}
/*
 * enum scenario {
    Error = 0,
    LeftTooClose = 1,
    RightTooClose = 2,
    CenterTooClose = 4,
    Straight = 8,
    LeftTurn = 9,
    RightTurn = 10,
    TeeJoint = 11,
    LeftJoint = 12,
    RightJoint = 13,
    CrossRoad = 14,
    Blocked = 15
};
 */
  if(CNB == 1){
      // right and left turns not possible, forward not blocked - straight ahead
      if(RTP == 0 && LTP == 0){
          result = Straight;}
      //right turn possible, left turn not, right joint
      else if(RTP == 1 && LTP == 0){
          result = RightJoint;}
      //left turn possible, right turn not, left joint
      else if(RTP == 0 && LTP == 1){
          result = LeftJoint;}
      //both left and right turn possible, crossroad
      else if(RTP == 1 && LTP == 1){
          result = CrossRoad;}
      else {
          result  = Error; //error
      }
  }//close move forward possibilities

  //center is blocked options { right turn, left turn, blocked, tee joint}
  if(CNB == 0){
      if(RTP == 1 && LTP == 1){
          result = TeeJoint;}//tee joint
      else if(RTP == 1 && LTP == 0){
          result = RightTurn;} //right turn
      else if(RTP == 0 && LTP == 1){
          result = LeftTurn;} //left turn
      else if(RTP == 0 && LTP == 0){
          result = Blocked;} // blocked
      else{
          result = Error; //error
      }
  }// close center is block options

  return result;
}
// ***********testing of Convert*********
int32_t const ADCBuffer[16]={2000, 2733, 3466, 4199, 4932, 5665, 6398, 7131, 7864, 8597, 9330, 10063, 10796, 11529, 12262, 12995};
int32_t const DistanceBuffer[16]={800, 713, 496, 380, 308, 259, 223, 196, 175, 158, 144, 132, 122, 114, 106, 100};
void Program4_1(void){
  int i;
  int32_t adc,distance,errors,diff;
  errors = 0;
  for(i=0; i<16; i++){
    adc = ADCBuffer[i];
    distance = Convert(adc); // call to your function
    diff = distance-DistanceBuffer[i];
    if((diff<-1)||(diff>1)){
      errors++;
    }
  }
  while(1){};
}
// ***********end of testing of Convert*********



// ***********testing of classify
scenario_t Solution(int32_t Left, int32_t Center, int32_t Right);
int32_t const CornerCases[18]={49,50,51,149,150,151,211,212,213,353,354,355,599,600,601,799,800,801};

void Program4_2(void){
  enum scenario result,truth;
  int i,j,k;
  int32_t left, right, center; // sensor readings
  int32_t errors = 0;
  for(i=0; i<18; i++){
    left = CornerCases[i];
    for(j=0; j<18; j++){
      center = CornerCases[j];
      for(k=0; k<18; k++){
        right = CornerCases[k];
        result = Classify(left,center,right); // your solution
        truth = Solution(left,center,right);  // correct answer
        if(result != truth){
           errors++;
        }
      }
    }
  }
  while(1){
  }
}

void Program4_3(void){ // will take over 16 hours to complete
  enum scenario result,truth;
  int32_t left, right, center; // sensor readings
  int32_t errors = 0;
  for(left=0; left<1000; left++){
    for(center=0; center<1000;  center++){
      for(right=0; right<1000; right++){
        result = Classify(left,center,right); // your solution
        truth = Solution(left,center,right);  // correct answer
        if(result != truth){
           errors++;
        }
      }
    }
  }
  while(1){
  }
}
enum scenario result,truth;
     volatile int left, right, center; // sensor readings
     volatile int errors = 0;
void main(void){
  // run one of these
//  Program4_1();
//  Program4_2();
//  Program4_3();
     for(left=0; left<1000; left++){
       for(center=0; center<1000;  center++){
         for(right=0; right<1000; right++){
           result = Classify(left,center,right); // your solution
           truth = Solution(left,center,right);  // correct answer
           if(result != truth){
              errors++;
           }
         }
       }
     }
     while(1){
     }
}
