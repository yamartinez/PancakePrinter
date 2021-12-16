/*
 * Steppers.c
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 */

#include "./Steppers.h"
#include "Pump.h"
#include "DebugIO.h"

//#define STEPPER_DEBUG 1

#define STEP_ACCEL

uint32_t X_Steps, Y_Steps = 0;
int16_t  GlobalX_Position, GlobalY_Position, X_Image_Offset, Y_Image_Offset  = 0;
uint16_t X_DIM, Y_DIM;
bool Steppers_Initialized = false;
bool Steppers_Calibrated = false;

void wait();
void wait_(uint32_t count);

void InitStepperMotors(uint16_t X_dim, uint16_t Y_dim){
    X_DIM = X_dim;
    Y_DIM = Y_dim;

    // Configure IO Pins for Control Pins
    MX_PORT_SEL0 &= ~(MCPINS);
    MX_PORT_SEL1 &= ~(MCPINS);
    MX_PORT_DIR  |=  (MCPINS);
    MX_PORT_OUT  &= ~(MCPINS);

    MY_PORT_SEL0 &= ~(MCPINS);
    MY_PORT_SEL1 &= ~(MCPINS);
    MY_PORT_DIR  |=  (MCPINS);
    MY_PORT_OUT  &= ~(MCPINS);

    // Configure I/O for Motor Reset
    M_RESET_PORT_SEL0 &= ~(MX_RESET | MY_RESET);
    M_RESET_PORT_SEL1 &= ~(MX_RESET | MY_RESET);
    M_RESET_PORT_DIR  |=  (MX_RESET | MY_RESET);
    M_RESET_PORT_OUT  |=  (MX_RESET | MY_RESET); // Check if Reset is active high or low

    // Configure I/O for Motor Output
    M_STEP_PORT_SEL0 &= ~(MX_STEP | MY_STEP);
    M_STEP_PORT_SEL1 &= ~(MX_STEP | MY_STEP);
    M_STEP_PORT_DIR  |=  (MX_STEP | MY_STEP);
    M_STEP_PORT_OUT  &= ~(MX_STEP | MY_STEP);

    // Configure I/O for Limit Switches
    M_LIMIT_PORT_SEL0 &= ~MLIMPINS;
    M_LIMIT_PORT_SEL1 &= ~MLIMPINS;
    M_LIMIT_PORT_DIR  &= ~MLIMPINS;
    M_LIMIT_PORT_REN  |=  MLIMPINS;
    M_LIMIT_PORT_OUT  &= ~MLIMPINS;
#ifndef STEPPER_DEBUG
    M_LIMIT_PORT_IES  &= ~MLIMPINS; // Edge select
    M_LIMIT_PORT_IE  &= ~MLIMPINS; // Diable interrupts
    M_LIMIT_PORT_IFG  &= ~MLIMPINS; // Clear Interrupt Flag
#endif

    //Disable Motors
    MX_DISABLE;
    MY_DISABLE;

    Steppers_Initialized = true;
}

#ifndef STEP_ACCEL
void X_Step(uint32_t id_, uint32_t len_){
    M_STEP_PORT_OUT |=  MX_STEP;
    wait();
    M_STEP_PORT_OUT &= ~MX_STEP;
    wait();
}

void Y_Step(uint32_t id_, uint32_t len_){
    M_STEP_PORT_OUT |=  MY_STEP;
    wait();
    M_STEP_PORT_OUT &= ~MY_STEP;
    wait();
}
#else
void X_Step(uint32_t id_, uint32_t len_){
    static uint32_t id    = 0;
    static uint32_t len   = 0;
    static uint32_t count = 0;

    if(id_ != id){
        count = 0;
        len = len_;
        id = id_;
    }

    M_STEP_PORT_OUT |=  MX_STEP;
    if (id == 0)
        wait(7);
    else if (count < 13)
        wait(20-count);
    else if (len-count < 13)
        wait(7+(len-count));
    else
        wait(7);
    M_STEP_PORT_OUT &= ~MX_STEP;
    if (id == 0)
        wait(7);
    else if (count < 13)
        wait(20-count);
    else if (len-count < 13)
        wait(7+(len-count));
    else
        wait(7);

    count ++;
}

void Y_Step(uint32_t id_, uint32_t len_){
    static uint32_t id    = 0;
    static uint32_t len   = 0;
    static uint32_t count = 0;

    if(id_ != id){
        count = 0;
        len = len_;
        id = id_;
    }

    M_STEP_PORT_OUT |=  MY_STEP;
    if (id == 0)
        wait(7);
    else if (count < 13)
        wait(20-count);
    else if (len-count < 13)
        wait(7+(len-count));
    else
        wait(7);
    M_STEP_PORT_OUT &= ~MY_STEP;
    if (id == 0)
        wait(7);
    else if (count < 13)
        wait(20-count);
    else if (len-count < 13)
        wait(7+(len-count));
    else
        wait(7);

    count++;

}
#endif

void X_Step_Forward(){
    MOTOR_X_FORWARD;
    X_Step(0,0);
}

void X_Step_Backward(){
    MOTOR_X_BACKWARD;
    X_Step(0,0);
}

void Y_Step_Forward(){
    MOTOR_Y_FORWARD;
    Y_Step(0,0);
}

void Y_Step_Backward(){
    MOTOR_Y_BACKWARD;
    Y_Step(0,0);
}

uint8_t CheckLimitSwitches(){
    uint8_t val = ~M_LIMIT_PORT_IN;
    uint8_t ret = ((val&(M_LIM0|M_LIM1)) >> 6);
    return ret;
}

bool XLim0(){
    return CheckLimitSwitches() & 0x01;
}

bool XLim1(){
    return CheckLimitSwitches() & 0x02;
}

bool YLim0(){
    return CheckLimitSwitches() & 0x01;
}

bool YLim1(){
    return CheckLimitSwitches() & 0x02;
}

void TestSteppers(){
    M_LIMIT_PORT_IE  &= ~MLIMPINS; // Diable interrupts
    MX_ENABLE;
    MY_ENABLE;
    while(1){
        for(uint8_t i = 200; i>0; i--){
            X_Step_Forward();
            Y_Step_Forward();
        }
        for(uint8_t i = 200; i>0; i--){
            X_Step_Backward();
            Y_Step_Backward();
        }
    }
}

void CalibrateSteppers(){
    M_LIMIT_PORT_IE  &= ~MLIMPINS; // Diable interrupts
    MX_ENABLE;
    MY_ENABLE;
    // Do calibration
    X_Steps = 0;
    Y_Steps = 0;

    while(!XLim0() && !XLim1()){
        X_Step_Backward(); //step backwards until xlim0 is hit, hopefully
    }
    if(XLim1()){ //this happens if the front limit switch is hit (it should be stepping backwards)
        error(direction_error_x,__FILE__,__LINE__);
    }
    while(XLim0()){ //step forward a bit so we are not on the switch in the back
        X_Step_Forward();
    }
    while(!XLim1()){ //step forward until the front limit switch is hit, and count how many steps it takes to get there
        X_Step_Forward();
        X_Steps++;
    }
    while(XLim1()){ // step back enough to get off the front limit switch, and deduct those steps from the total of x steps
        X_Step_Backward();
        X_Steps--;
    }

    while(!YLim0() && !YLim1()){ // same process as with the x (literally the same read those comments)
        Y_Step_Backward();
    }
    if(YLim1()){
        error(direction_error_y,__FILE__,__LINE__);
    }
    while(YLim0()){
        Y_Step_Forward();
    }
    while(!YLim1()){
        Y_Step_Forward();
        Y_Steps++;
    }
    while(YLim1()){
        Y_Step_Backward();
        Y_Steps--;
    }

    X_Steps -= 100;
    Y_Steps -= 100;

    for(uint16_t i = X_Steps;i>0;i--){ //move back together to the 0,0 coordinate
        X_Step_Backward();
    }
    for(uint16_t i = Y_Steps;i>0;i--){ //move back together to the 0,0 coordinate
        Y_Step_Backward();
    }

    MX_DISABLE;
    MY_DISABLE;

    X_Steps-=100;
    Y_Steps-=100;

    Steppers_Calibrated = true;

    M_LIMIT_PORT_IFG &= ~MLIMPINS; //clear flag, enable interrupts
    M_LIMIT_PORT_IE  |=  MLIMPINS;

    GlobalX_Position = 0;
    GlobalY_Position = 0;

    X_Image_Offset = (X_Steps>>2) - (X_DIM>>2);
    Y_Image_Offset = (Y_Steps>>2) - (Y_DIM>>2);

//    move_image(0,0);
//    move_image(0,500);
//    move_image(500,500);
//    move_image(500,0);
//    move_image(0,0);
//    move_absolute(0,0);

//    error(steppers_not_initialized);
}

void move(direction x, direction y,uint16_t x_count, uint16_t y_count){
    static uint32_t id    = 0;
    id++;

    if(x == forward){
        MOTOR_X_FORWARD;
        GlobalX_Position += x_count;
    } else if (x==backward) {
        MOTOR_X_BACKWARD;
        GlobalX_Position -= x_count;
    }
    if(y == forward){
        MOTOR_Y_FORWARD;
        GlobalY_Position += y_count;
    } else if (y==backward){
        MOTOR_Y_BACKWARD;
        GlobalY_Position -= y_count;
    }

    if(GlobalX_Position < 0 || GlobalX_Position > X_Steps){
        __no_operation();
        error(bounds_error_x,__FILE__,__LINE__);
    }
    if(GlobalY_Position < 0 || GlobalY_Position > Y_Steps){
        __no_operation();
        error(bounds_error_y,__FILE__,__LINE__);
    }
    MX_ENABLE;
    MY_ENABLE;
    if (x!=none && y!=none){
        if(x_count > y_count){
            for(uint16_t i = y_count; i>0; i--){
                X_Step(id,x_count);
                Y_Step(id,y_count);
            }
            MY_DISABLE;
            for(uint16_t i = x_count-y_count; i>0;i--){
                X_Step(id,x_count);
            }
            MX_DISABLE;
        } else if (y_count > x_count){
            for(uint16_t i = x_count; i>0; i--){
                X_Step(id,x_count);
                Y_Step(id,y_count);
            }
            MX_DISABLE;
            for(uint16_t i = y_count-x_count; i>0;i--){
                Y_Step(id,y_count);
            }
            MY_DISABLE;
        } else {
            for(uint16_t i = x_count; i>0; i--){
                X_Step(id,x_count);
                Y_Step(id,y_count);
            }
            MX_DISABLE;
            MY_DISABLE;
        }
    }
    else if(x!=none){
        for(uint16_t i = x_count; i > 0; i--){
                X_Step(id,x_count);
        }
    } else if(y!=none){
        for(uint16_t i = y_count; i > 0; i--){
                Y_Step(id,y_count);
        }
    }
    MX_DISABLE;
    MY_DISABLE;
}

void move_relative(int16_t x, int16_t y){
    if(!Steppers_Initialized){
        error(steppers_not_initialized,__FILE__,__LINE__);
    }
    if(!Steppers_Calibrated){
        error(steppers_not_calibrated,__FILE__,__LINE__);
    }
    direction x_direction = none;
    direction y_direction = none;
    if(x > 0){
        x_direction = forward;
    }
    if(x < 0){
        x_direction = backward;
    }
    if(y > 0){
        y_direction = forward;
    }
    if(y < 0){
        y_direction = backward;
    }
    x = x<0 ? -1*x : x;
    y = y<0 ? -1*y : y;

    move(x_direction,y_direction,x,y);

}

void move_absolute(int16_t x, int16_t y){
    move_relative(x-GlobalX_Position, y-GlobalY_Position);
}

void move_image(int16_t x, int16_t y){
    move_absolute(X_Image_Offset+x,Y_Image_Offset+y);
}

void fill_image_square(){
    move_image(0,500);
    PumpDrive();
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    for(uint16_t i = (Y_DIM>>4); i > 0; i--){
        if(i&0x0001){
            move_image(0,i*16);
            move_image(X_DIM,i*16);
        } else {
            move_image(X_DIM,i*16);
            move_image(0,i*16);
        }
    }
    PumpStop();
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    __delay_cycles(1000000);  // .5s
    move_absolute(0,0);
}

#ifndef STEP_ACCEL
void wait(){
    __delay_cycles(750);
}
#else
void wait(uint32_t count){
    for(count;count>0;count--){
        _delay_cycles(1000);
    }
}
#endif

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT4_VECTOR))) Port_4 (void)
#else
#error Compiler not supported!
#endif
{
    error(unknown,__FILE__,__LINE__);
}
