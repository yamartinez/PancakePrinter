//eb5yf
//bilge batsukh
//2/3/2020
//lab 14 quiz
//main.c

#include "msp.h"
#include "..\inc\CortexM.h"
#include "..\inc\Clock.h"

#define PORT P4
#define PIN BIT3

//preprocessor directives for edge bounce counting
#define SEL0_AS_GPIO PORT->SEL0 &= ~PIN
#define SEL1_AS_GPIO PORT->SEL1 &= ~PIN
#define DEFINE_AS_INPUT PORT->DIR &= ~PIN
#define ENABLE_INTERNAL_RESISTORS PORT->REN |= PIN
#define INTERNAL_RESISTORS_AS_PULLUP PORT->OUT |= PIN
#define INTERRUPT_ON_FALLING_EDGE PORT->IES |= PIN
#define ARM_INTERRUPT PORT->IE |= PIN

#define CLEAR_INTERRUPT_FLAG PORT->IFG &= ~PIN

//to set the nested vector interrupt controller settings, keep in mind this interrupts on P3 are interrupt 37
#define COUNTER_NVIC_ENABLE NVIC->ISER[1] |= 0x00000020 //enable interrupt 37 in the NVIC
//setting the priority is difficult in one line but trivial in two so i will use two lines
#define PHASE1_PRIORITY_SET_4 NVIC->IP[9] &= ~0x0000e000 //this line clears the top three bits of PRI_37
#define PHASE2_PRIORITY_SET_4 NVIC->IP[9] |=  0x00008000 //this line sets the top three bits of PRI_37 to 100 -> 4 in binary
//define global variables
uint32_t Count;

void Input_Init(void){
    Count = 0;
    SEL0_AS_GPIO;
    SEL1_AS_GPIO;
    DEFINE_AS_INPUT;
    ENABLE_INTERNAL_RESISTORS;
    INTERNAL_RESISTORS_AS_PULLUP;
    INTERRUPT_ON_FALLING_EDGE;
    ARM_INTERRUPT;

    COUNTER_NVIC_ENABLE;
    PHASE1_PRIORITY_SET_4;PHASE2_PRIORITY_SET_4;

    CLEAR_INTERRUPT_FLAG;
}

void PORT3_IRQHandler(void){
    CLEAR_INTERRUPT_FLAG;
    Count++;
}
/**
 * main.c
 */
void main(void)
{
    Clock_Init48MHz();
    Input_Init();
    while(1){

    }

}