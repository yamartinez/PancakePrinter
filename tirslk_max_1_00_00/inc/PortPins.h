/*
 * PortPins.h
 *
 *  Created on: Jul 9, 2019
 *      Author: tadd2x1
 */

#ifndef PORTPINS_H_
#define PORTPINS_H_

#include "msp.h"

#define NVIC_INTERRUPT_ENABLE(REG,BIT)      NVIC->ISER[REG] = (0x00000001 << BIT)
#define PRIORITY_REGISTER(REG,MASK,PR)      NVIC->IP[REG] = (NVIC->IP[REG]&MASK)|PR
#define ADC14_CONVERSION_START_ADDR(REG)    (uint32_t)((REG<<ADC14_CTL1_CSTARTADD_OFS) & ADC14_CTL1_CSTARTADD_MASK)

// LEDs
#define LED1_PORT               P1
#define LED1_BIT                0x01
#define SET_LED1_AS_AN_OUTPUT   LED1_PORT->DIR |= LED1_BIT
#define TOGGLE_LED1             LED1_PORT->OUT ^= LED1_BIT

#define REFLECTANCE_IR_PORT                 P5
#define REFLECTANCE_IR_BIT                  0x08
#define REFLECTANCE_IR_DIR                  REFLECTANCE_IR_PORT->DIR
#define REFLECTANCE_IR_OUT                  REFLECTANCE_IR_PORT->OUT
#define SET_REFLECTANCE_IR_AS_OUTPUT        REFLECTANCE_IR_DIR |= REFLECTANCE_IR_BIT
#define TURN_ON_REFLECTANCE_IR              REFLECTANCE_IR_OUT |= REFLECTANCE_IR_BIT
#define TURN_OFF_REFLECTANCE_IR             REFLECTANCE_IR_OUT &= ~REFLECTANCE_IR_BIT

#define REFLECTANCE_SENSOR_PORT             P7
#define REFLECTANCE_SENSOR_DIR              REFLECTANCE_SENSOR_PORT->DIR
#define REFLECTANCE_SENSOR_OUT              REFLECTANCE_SENSOR_PORT->OUT
#define REFLECTANCE_SENSOR_IN               REFLECTANCE_SENSOR_PORT->IN
#define SET_REFLECTANCE_SENSOR_AS_INPUT     REFLECTANCE_SENSOR_DIR = 0x00
#define SET_REFLECTANCE_SENSOR_AS_OUTPUT    REFLECTANCE_SENSOR_DIR = 0xFF

// Definitions for bump switches
#define BUMP_PORT   P4
#define BUMP5_BIT   0x80
#define BUMP4_BIT   0x40
#define BUMP3_BIT   0x20
#define BUMP2_BIT   0x08
#define BUMP1_BIT   0x04
#define BUMP0_BIT   0x01
#define BUMP_BITS   (BUMP5_BIT|BUMP4_BIT|BUMP3_BIT|BUMP2_BIT|BUMP1_BIT|BUMP0_BIT)
#define SET_BUMP_SWITCHES_AS_INPUTS     BUMP_PORT->DIR &= ~BUMP_BITS
#define ENABLE_PULL_RESISTORS           BUMP_PORT->REN |= BUMP_BITS
#define PULL_UP_RESISTORS               BUMP_PORT->OUT |= BUMP_BITS
#define FALLING_EDGE_INTERRUPTS         BUMP_PORT->IES |= BUMP_BITS
#define RISING_EDGE_INTERRUPTS          BUMP_PORT->IES &= ~BUMP_BITS
#define ENABLE_BUMP_SWITCHES_INTERRUPTS BUMP_PORT->IE |= BUMP_BITS

// PORT4 Interrupt input = 38 (Table 6-39 of datasheet)
// ISER1 Irqs 32 to 63 (Table 2-25 of the Technical Reference Manual)
// Interrupt priority register 9 (Table 2-25 of the Technical Reference Manual),
// bits 23-16 (Table 2-45 of the Technical Reference Manual)
//#define BUMP_PORT_NVIC_INTERRUPT_ENABLE NVIC->ISER[1] = (0x00000001 << (38-32))
//#define BUMP_PORT_PRIORITY_REGISTER(PR) NVIC->IP[9] = (NVIC->IP[9]&0xFF00FFFF)|PR
#define BUMP_PORT_NVIC_INTERRUPT_ENABLE     NVIC_INTERRUPT_ENABLE(1,(38-32))
#define BUMP_PORT_PRIORITY_REGISTER(PR)     PRIORITY_REGISTER(9,0xFF00FFFF,(PR << (5+16)))

// Definitions for motors
#define DIRR_PORT                       P5      // Right Motor Direction
#define DIRR_BIT                        0x20
#define RIGHT_MOTOR_DIRECTION_FORWARD   DIRR_PORT->OUT &= ~DIRR_BIT
#define RIGHT_MOTOR_DIRECTION_REVERSE   DIRR_PORT->OUT |= DIRR_BIT

#define SLPR_PORT                       P3      // Right Motor Sleep
#define SLPR_BIT                        0x40
#define RIGHT_MOTOR_SLEEP_ENABLE        SLPR_PORT->OUT &= ~SLPR_BIT
#define RIGHT_MOTOR_SLEEP_DISABLE       SLPR_PORT->OUT |= SLPR_BIT

#define PWMR_PORT                       P2      // Right Motor PWM
#define PWMR_BIT                        0x40
#define RIGHT_MOTOR_PWM_ENABLE          PWMR_PORT->OUT |= PWMR_BIT
#define RIGHT_MOTOR_PWM_DISABLE         PWMR_PORT->OUT &= ~PWMR_BIT

#define DIRL_PORT                       P5      // Left Motor Direction
#define DIRL_BIT                        0x10
#define LEFT_MOTOR_DIRECTION_FORWARD    DIRL_PORT->OUT &= ~DIRL_BIT
#define LEFT_MOTOR_DIRECTION_REVERSE    DIRL_PORT->OUT |= DIRL_BIT

#define SLPL_PORT                       P3      // Left Motor Sleep
#define SLPL_BIT                        0x80
#define LEFT_MOTOR_SLEEP_ENABLE         SLPL_PORT->OUT &= ~SLPL_BIT
#define LEFT_MOTOR_SLEEP_DISABLE        SLPL_PORT->OUT |= SLPL_BIT

#define PWML_PORT                       P2      // Left Motor PWM
#define PWML_BIT                        0x80
#define LEFT_MOTOR_PWM_ENABLE           PWML_PORT->OUT |= PWML_BIT
#define LEFT_MOTOR_PWM_DISABLE          PWML_PORT->OUT &= ~PWML_BIT

// Definitions associated with IR distance sensors.
#define ANALOG_CHANNEL6_PORT P4
#define ANALOG_CHANNEL6_BIT 0x80
#define ANALOG_CHANNEL7_PORT P4
#define ANALOG_CHANNEL7_BIT 0x40
#define ANALOG_CHANNEL12_PORT P4
#define ANALOG_CHANNEL12_BIT 0x02
#define ANALOG_CHANNEL14_PORT P6
#define ANALOG_CHANNEL14_BIT 0x02
#define ANALOG_CHANNEL16_PORT P9
#define ANALOG_CHANNEL16_BIT 0x02
#define ANALOG_CHANNEL17_PORT P9
#define ANALOG_CHANNEL17_BIT 0x01

// Definitions associated with tachometers
#define ENCODER_LEFT_A_PORT    P10
#define ENCODER_LEFT_A_BIT     0x20
#define ENCODER_RIGHT_A_PORT    P10
#define ENCODER_RIGHT_A_BIT     0x10

#define ENCODER_LEFT_B_PORT    P5
#define ENCODER_LEFT_B_BIT     0x04
#define ENCODER_RIGHT_B_PORT    P5
#define ENCODER_RIGHT_B_BIT     0x01

#define SET_ELB_AS_INPUT                ENCODER_LEFT_B_PORT->DIR &= ~ENCODER_LEFT_B_BIT
#define SET_ERB_AS_INPUT                ENCODER_RIGHT_B_PORT->DIR &= ~ENCODER_RIGHT_B_BIT
#define ENABLE_ELB_PULL_RESISTORS       ENCODER_LEFT_B_PORT->REN |= ENCODER_LEFT_B_BIT
#define ENABLE_ERB_PULL_RESISTORS       ENCODER_RIGHT_B_PORT->REN |= ENCODER_RIGHT_B_BIT
#define PULL_DOWN_RESISTORS_ELB         ENCODER_LEFT_B_PORT->OUT &= ~ENCODER_LEFT_B_BIT
#define PULL_DOWN_RESISTORS_ERB         ENCODER_RIGHT_B_PORT->OUT &= ~ENCODER_RIGHT_B_BIT
#define RISING_EDGE_INTERRUPTS_ELB      ENCODER_LEFT_B_PORT->IES &= ~ENCODER_LEFT_B_BIT
#define RISING_EDGE_INTERRUPTS_ERB      ENCODER_RIGHT_B_PORT->IES &= ~ENCODER_RIGHT_B_BIT
#define ENABLE_ELB_INTERRUPTS           ENCODER_LEFT_B_PORT->IE |= ENCODER_LEFT_B_BIT
#define ENABLE_ERB_INTERRUPTS           ENCODER_RIGHT_B_PORT->IE |= ENCODER_RIGHT_B_BIT

// TimerA3 CCR0 Interrupt input = 14 (Table 6-39 of datasheet)
// ISER0 Irqs 0 to 31 (Table 2-25 of the Technical Reference Manual)
// Interrupt priority register 3 (Table 2-25 of the Technical Reference Manual),
// bits 23-16 (Table 2-33 of the Technical Reference Manual)
#define ENCODER_A_PORT_NVIC_INTERRUPT_ENABLE_RIGHT     NVIC_INTERRUPT_ENABLE(0,14);
#define ENCODER_A_PORT_PRIORITY_REGISTER_RIGHT(PR)     PRIORITY_REGISTER(3,0xFF00FFFF,(PR << (5+16)))

// TimerA3 CCRn Interrupt input = 15 (Table 6-39 of datasheet)
// ISER0 Irqs 0 to 31 (Table 2-25 of the Technical Reference Manual)
// Interrupt priority register 3 (Table 2-25 of the Technical Reference Manual),
// bits 31-24 (Table 2-33 of the Technical Reference Manual)
#define ENCODER_A_PORT_NVIC_INTERRUPT_ENABLE_LEFT     NVIC_INTERRUPT_ENABLE(0,15);
#define ENCODER_A_PORT_PRIORITY_REGISTER_LEFT(PR)     PRIORITY_REGISTER(3,0x00FFFFFF,(PR << (5+24)))

// PORT5 Interrupt input = 39 (Table 6-39 of datasheet)
// ISER1 Irqs 32 to 63 (Table 2-25 of the Technical Reference Manual)
// Interrupt priority register 9 (Table 2-25 of the Technical Reference Manual),
// bits 31-24 (Table 2-45 of the Technical Reference Manual)
#define ENCODER_B_PORT_NVIC_INTERRUPT_ENABLE     NVIC_INTERRUPT_ENABLE(1,(39-32))
#define ENCODER_B_PORT_PRIORITY_REGISTER(PR)     PRIORITY_REGISTER(9,0x00FFFFFF,(PR << (5+24)))

// Port definitions for robot arm servos
#define SERVO_ARM_HEIGHT_PORT               P2
#define SERVO_ARM_HEIGHT_PORT_MAP_REGISTER  P2MAP->PMAP_REGISTER4
#define SERVO_ARM_HEIGHT_BIT                0x10

#define SERVO_ARM_TILT_PORT                 P3
#define SERVO_ARM_TILT_PORT_MAP_REGISTER    P3MAP->PMAP_REGISTER5
#define SERVO_ARM_TILT_BIT                  0x20

#define SERVO_GRIPPER_PORT                  P5
#define SERVO_GRIPPER_PORT_MAP_REGISTER     P5MAP->PMAP_REGISTER7
#define SERVO_GRIPPER_BIT                   0x80

#define SERVO_ARM_HEIGHT_FEEDBACK_PORT      P8
#define SERVO_ARM_HEIGHT_FEEDBACK_BIT       0x10
#define SERVO_ARM_TILT_FEEDBACK_PORT        P8
#define SERVO_ARM_TILT_FEEDBACK_BIT         0x08
#define SERVO_GRIPPER_FEEDBACK_PORT         P8
#define SERVO_GRIPPER_FEEDBACK_BIT          0x04

// Bit-banding for P5.3 and P7.0-P7.7
#define REFLECTANCE_IR  (*((volatile uint8_t *) (0x4209884c)))
#define REFLECTANCE_S1_IN  (*((volatile uint8_t *) (0x42098c00)))
#define REFLECTANCE_S2_IN  (*((volatile uint8_t *) (0x42098c04)))
#define REFLECTANCE_S3_IN  (*((volatile uint8_t *) (0x42098c08)))
#define REFLECTANCE_S4_IN  (*((volatile uint8_t *) (0x42098c0c)))
#define REFLECTANCE_S5_IN  (*((volatile uint8_t *) (0x42098c10)))
#define REFLECTANCE_S6_IN  (*((volatile uint8_t *) (0x42098c14)))
#define REFLECTANCE_S7_IN  (*((volatile uint8_t *) (0x42098c18)))
#define REFLECTANCE_S8_IN  (*((volatile uint8_t *) (0x42098c1c)))

#define REFLECTANCE_S1_OUT  (*((volatile uint8_t *) (0x42098c40)))
#define REFLECTANCE_S2_OUT  (*((volatile uint8_t *) (0x42098c44)))
#define REFLECTANCE_S3_OUT  (*((volatile uint8_t *) (0x42098c48)))
#define REFLECTANCE_S4_OUT  (*((volatile uint8_t *) (0x42098c4c)))
#define REFLECTANCE_S5_OUT  (*((volatile uint8_t *) (0x42098c50)))
#define REFLECTANCE_S6_OUT  (*((volatile uint8_t *) (0x42098c54)))
#define REFLECTANCE_S7_OUT  (*((volatile uint8_t *) (0x42098c58)))
#define REFLECTANCE_S8_OUT  (*((volatile uint8_t *) (0x42098c5c)))

// Function Prototypes
void PortRegisterMapping(void);

#endif /* PORTPINS_H_ */
