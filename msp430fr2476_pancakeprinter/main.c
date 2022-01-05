#include <msp430.h> 
#include <stdint.h>
#include <stdbool.h>

#include "./Steppers.h"
#include "./Pump.h"
#include "./UART.h"
#include "./DebugIO.h"
#include "Commands.h"

void whack();
void uart_task(uint16_t);
void mainloop();
/**
 * main.c
 */
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	InitDebugIO();
	InitStepperMotors(1000,1000);
	InitPump();
	uart_main();
	PM5CTL0 &= ~LOCKLPM5;
	__bis_SR_register(GIE);
	__no_operation();
	DebugLEDOn();
	while(!(DebugReadButton())){
	    __delay_cycles(100);
	}
//	TestSteppers();
	PumpStop();
	DebugLEDOff();
	__delay_cycles(1000000);  // .5s
	CalibrateSteppers();

	mainloop();
//	whack();

//	return 0;
}

void uart_task(uint16_t value){
    DebugLEDOff();
    _no_operation();
}

void mainloop(){
    static uint8_t notified = 0;
    while(1){
        notified = 0;
        while(QueueSize()>0){
            // Execute Command
            Command* c = DequeueCommand();
            if(c != 0)
                ExecuteCommand(c);
        }
        while(QueueSize()==0){
            // Twiddle Thumbs
//            if(!notified){
                UART_Write(0);
//                notified = 1;
//            }
            __no_operation();
        }
    }
}

void whack(){
//    move_image(500, 500);
//    PumpDrive();
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    move_image(0,500);
//    move_image(0,0);
//    move_image(500,0);
//    move_image(500,500);
//    PumpStop();
//    __delay_cycles(1000000);  // .5s
//    move_image(0,0);
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    move_image(500,500);
//    PumpDrive();
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    move_image(0,0);
//    PumpStop();
//    move_image(500,0);
//    PumpDrive();
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    move_image(0,500);
//    PumpStop();
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    move_image(0,0);
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s
//    __delay_cycles(1000000);  // .5s

    fill_image_square();
}
