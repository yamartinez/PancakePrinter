/*
 * MotorEncoderInt.c
 *
 *  Created on: Feb 20, 2020
 *      Author: tadd2x1
 */

#include <stdint.h>
#include "msp.h"
#include "../inc/PortPins.h"

// P5.2 Left Encoder B
// P5.0 Right Encoder B

void (*CaptureTask2)(uint16_t time);
void (*CaptureTask3)(uint16_t time);

// Initialize port pins associated with motor encoder B
// Make two Port 5 pins inputs
// Activate interface pullup
// pins 5, 0
// Interrupt on rising edge
void MotorEncoderInt_Init(void(*task0)(uint16_t time), void(*task1)(uint16_t time))
{
    // write this as part of Lab 14
    CaptureTask2 = task0;
    CaptureTask3 = task1;

    ENCODER_LEFT_B_PORT->SEL0 &= ~ENCODER_LEFT_B_BIT; // Configure port pins as GPIO.
    ENCODER_LEFT_B_PORT->SEL1 &= ~ENCODER_LEFT_B_BIT;
    ENCODER_RIGHT_B_PORT->SEL0 &= ~ENCODER_RIGHT_B_BIT;
    ENCODER_RIGHT_B_PORT->SEL1 &= ~ENCODER_RIGHT_B_BIT;

    SET_ELB_AS_INPUT; SET_ERB_AS_INPUT;
    ENABLE_ELB_PULL_RESISTORS; ENABLE_ERB_PULL_RESISTORS;
    PULL_DOWN_RESISTORS_ELB; PULL_DOWN_RESISTORS_ERB;
    RISING_EDGE_INTERRUPTS_ELB; RISING_EDGE_INTERRUPTS_ERB;
    ENCODER_LEFT_B_PORT->IFG &= ~ENCODER_LEFT_B_BIT; // Clear interrupt flags initially
    ENCODER_RIGHT_B_PORT->IFG &= ~ENCODER_RIGHT_B_BIT;
    ENCODER_RIGHT_B_PORT->IFG = 0x00;
    ENABLE_ELB_INTERRUPTS; ENABLE_ERB_INTERRUPTS;

    ENCODER_B_PORT_NVIC_INTERRUPT_ENABLE;
    ENCODER_B_PORT_PRIORITY_REGISTER(2); // priority 2
}

void PORT5_IRQHandler(void)
{
    uint16_t EventTime = 0;

    EventTime = TA3R;
    if ((ENCODER_LEFT_B_PORT->IFG & ENCODER_LEFT_B_BIT) != 0) {
        ENCODER_LEFT_B_PORT->IFG &= ~ENCODER_LEFT_B_BIT;    // acknowledge capture/compare interrupt 1
        (*CaptureTask2)(EventTime);                         // execute user task
    }
    else if ((ENCODER_RIGHT_B_PORT->IFG & ENCODER_RIGHT_B_BIT) != 0) {
        ENCODER_RIGHT_B_PORT->IFG &= ~ENCODER_RIGHT_B_BIT;  // acknowledge capture/compare interrupt 1
        (*CaptureTask3)(EventTime);                         // execute user task
    }
}






