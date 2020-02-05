// Reflectance.c
// Provide functions to take measurements using the kit's built-in
// QTRX reflectance sensor array.  Pololu part number 3672. This works by outputting to the
// sensor, waiting, then reading the digital value of each of the
// eight phototransistors.  The more reflective the target surface is,
// the faster the voltage decays.
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

// reflectance even LED illuminate connected to P5.3
// reflectance odd LED illuminate connected to P9.2
// reflectance sensor 1 connected to P7.0 (robot's right, robot off road to left)
// reflectance sensor 2 connected to P7.1
// reflectance sensor 3 connected to P7.2
// reflectance sensor 4 connected to P7.3 center
// reflectance sensor 5 connected to P7.4 center
// reflectance sensor 6 connected to P7.5
// reflectance sensor 7 connected to P7.6
// reflectance sensor 8 connected to P7.7 (robot's left, robot off road to right)

#include <stdint.h>
#include "msp432.h"
#include "..\inc\Clock.h"
#define INPUT 0
#define OUTPUT 1



// ------------Reflectance_Init------------
// Initialize the GPIO pins associated with the QTR-8RC
// reflectance sensor.  Infrared illumination LEDs are
// initially off.
// Input: none
// Output: none
void Reflectance_Init(void){
    // write this as part of Lab 6
    P5->SEL0 &= ~0x08;
    P5->SEL1 &= ~0x08;    //  P5.3 as GPIO
    P5-> DIR |=  0x08;      //  make P5.3 out

    P9->SEL0 &= ~0x04;
    P9->SEL1 &= ~0x04;    //  P9.2 as GPIO
    P9-> DIR |=  0x04;      //  make 9.2 out

    P7 -> SEL0 = 0x00;    //P7.x GPIO
    P7 -> SEL1 = 0x00;

    P7 -> DIR =  0x00; // 7.x high
}

void infraredON(){
    P5 -> OUT |= 0x08;
    P9 -> OUT |= 0x04;
}

void infraredOFF(){
    P5 -> OUT &= ~0x08;
    P9 -> OUT &= ~0x04;
}
// ------------Reflectance_Read------------
// Read the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Read(uint32_t time){
uint8_t result;
    // write this as part of Lab 6
    infraredON();

    P7 -> DIR = 0xFF; // 7.x output
    P7 -> OUT = 0xFF; //7.x high
    Clock_Delay1us(10);
    P7 -> DIR = 0x00;
    Clock_Delay1us(time);
  result = P7 -> IN; // replace this line - DONE - this is now hex for the inputs to P7

    infraredOFF();

  return result;
}

// ------------Reflectance_Center------------
// Read the two center sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// wait t us
// Read sensors
// Turn off the 8 IR LEDs
// Input: time to wait in usec
// Output: 0 (off road), 1 off to left, 2 off to right, 3 on road
// (Left,Right) Sensors
// 1,1          both sensors   on line
// 0,1          just right     off to left
// 1,0          left left      off to right
// 0,0          neither        lost
// Assumes: Reflectance_Init() has been called
uint8_t Reflectance_Center(uint32_t time){
    // write this as part of Lab 6
    uint8_t read = Reflectance_Read(time);

    read &= 0x18; // clear everything but 0x18 (7.3 & 7.4)
    read = read >> 3;
    return read; // replace this line - DONE
}


// Perform sensor integration
// Input: data is 8-bit result from line sensor
// Output: position in 0.1mm relative to center of line
#define pin7 0x80
#define pin6 0x40
#define pin5 0x20
#define pin4 0x10
#define pin3 0x08
#define pin2 0x04
#define pin1 0x02
#define pin0 0x01

int32_t Reflectance_Position(uint8_t data){
    // write this as part of Lab 6
    //int32_t dataTest = data;
    int32_t W[8] = {334, 238, 142, 48, -48, -142, -238, -334};
    //int32_t avg = 0;
    //int32_t count = 0;
    int32_t Z[8] = {pin0 , pin1, pin2, pin3, pin4, pin5, pin6, pin7};
    if (data == 0)
        return 0;

    else{
        float top = 0.0;
        float bot = 0.0;
        int count;
        for(count = 0; count < 8; count++){
            top += W[count]*(data & Z[count]);
        }
        int count2;
        for(count2 = 0; count2 < 8; count2++){
            bot += (data & Z[count2]);
        }

        return top/bot;
        }
    /*
    if(dataTest & pin0 == 0x01){
        avg += W[0];
        count++;
    }
    if(dataTest & pin1 == 0x02){
        avg += W[1];
        count++;
    }
    if(dataTest & pin2 == 0x04){
        avg += W[2];
        count++;
    }

    if(dataTest & pin3 == 0x08){
        avg += W[3];
        count++;
    }
    if(dataTest & pin4 == 0x10){
        avg += W[4];
        count++;
    }
    if(dataTest & pin5 == 0x20){
        avg += W[5];
        count++;
    }
    if(dataTest & pin6 == 0x40){
        avg += W[6];
        count++;
    }
    if(dataTest & pin7 == 0x80){
        avg += W[7];
        count++;
    }
    avg /= count;
 return avg; // replace this line*/
}


// ------------Reflectance_Start------------
// Begin the process of reading the eight sensors
// Turn on the 8 IR LEDs
// Pulse the 8 sensors high for 10 us
// Make the sensor pins input
// Input: none
// Output: none
// Assumes: Reflectance_Init() has been called
void Reflectance_Start(void){
    // write this as part of Lab 10
    infraredON();
    P7 -> DIR = 0xFF; // 7.x output
    P7 -> OUT = 0xFF; //7.x high
    Clock_Delay1us(10);//busy delay 10 us
    P7 -> DIR = 0x00;
}


// ------------Reflectance_End------------
// Finish reading the eight sensors
// Read sensors
// Turn off the 8 IR LEDs
// Input: none
// Output: sensor readings
// Assumes: Reflectance_Init() has been called
// Assumes: Reflectance_Start() was called 1 ms ago
uint8_t Reflectance_End(void){
    // write this as part of Lab 10
    volatile uint8_t result = P7 -> IN; // hex for the inputs to P7
    infraredOFF();
    return result; // replace this line
}

























