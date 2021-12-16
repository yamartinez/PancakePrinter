/*
 * Steppers.c
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 * version 2.0 - for msp432p401r
 */
#define STEP_ACCEL 1
#include "./Steppers.h"
#include "Pump.h"

#define STEP_ACCEL 1

uint32_t X_Steps, Y_Steps = 0;
int16_t  GlobalX_Position, GlobalY_Position, X_Image_Offset, Y_Image_Offset  = 0;
uint16_t X_DIM, Y_DIM;
bool Steppers_Initialized = false;
bool Steppers_Calibrated = false;

void wait(uint32_t count);

void BumpInterruptInit();

void InitStepperMotors(uint16_t X_dim, uint16_t Y_dim){

    X_DIM = X_dim;
    Y_DIM = Y_dim;

        // Configure IO Pins for Control Pins
    MX_ENABLE_PORT_SEL0 &= ~ MX_ENABLE_PIN;
    MX_ENABLE_PORT_SEL1 &= ~ MX_ENABLE_PIN;
    MX_ENABLE_PORT_DIR  |=   MX_ENABLE_PIN;
    MX_ENABLE_PORT_OUT  &= ~ MX_ENABLE_PIN;
    MY_ENABLE_PORT_SEL0 &= ~ MY_ENABLE_PIN;
    MY_ENABLE_PORT_SEL1 &= ~ MY_ENABLE_PIN;
    MY_ENABLE_PORT_DIR  |=   MY_ENABLE_PIN;
    MY_ENABLE_PORT_OUT  &= ~ MY_ENABLE_PIN;

    MX_DIR_PORT_SEL0 &= ~ MX_DIR_PIN;
    MX_DIR_PORT_SEL1 &= ~ MX_DIR_PIN;
    MX_DIR_PORT_DIR  |=   MX_DIR_PIN;
    MX_DIR_PORT_OUT  &= ~ MX_DIR_PIN;
    MY_DIR_PORT_SEL0 &= ~ MY_DIR_PIN;
    MY_DIR_PORT_SEL1 &= ~ MY_DIR_PIN;
    MY_DIR_PORT_DIR  |=   MY_DIR_PIN;
    MY_DIR_PORT_OUT  &= ~ MY_DIR_PIN;

    MX_uS0_PORT_SEL0 &= ~ MX_uS0_PIN;
    MX_uS0_PORT_SEL1 &= ~ MX_uS0_PIN;
    MX_uS0_PORT_DIR  |=   MX_uS0_PIN;
    MX_uS0_PORT_OUT  &= ~ MX_uS0_PIN;
    MY_uS0_PORT_SEL0 &= ~ MY_uS0_PIN;
    MY_uS0_PORT_SEL1 &= ~ MY_uS0_PIN;
    MY_uS0_PORT_DIR  |=   MY_uS0_PIN;
    MY_uS0_PORT_OUT  &= ~ MY_uS0_PIN;

    MX_uS1_PORT_SEL0 &= ~ MX_uS1_PIN;
    MX_uS1_PORT_SEL1 &= ~ MX_uS1_PIN;
    MX_uS1_PORT_DIR  |=   MX_uS1_PIN;
    MX_uS1_PORT_OUT  &= ~ MX_uS1_PIN;
    MY_uS1_PORT_SEL0 &= ~ MY_uS1_PIN;
    MY_uS1_PORT_SEL1 &= ~ MY_uS1_PIN;
    MY_uS1_PORT_DIR  |=   MY_uS1_PIN;
    MY_uS1_PORT_OUT  &= ~ MY_uS1_PIN;

    MX_RESET_PORT_SEL0 &= ~ MX_RESET_PIN;
    MX_RESET_PORT_SEL1 &= ~ MX_RESET_PIN;
    MX_RESET_PORT_DIR  |=   MX_RESET_PIN;
    MX_RESET_PORT_OUT  |=   MX_RESET_PIN;
    MY_RESET_PORT_SEL0 &= ~ MY_RESET_PIN;
    MY_RESET_PORT_SEL1 &= ~ MY_RESET_PIN;
    MY_RESET_PORT_DIR  |=   MY_RESET_PIN;
    MY_RESET_PORT_OUT  |=   MY_RESET_PIN;

    MX_STEP_PORT_SEL0 &= ~ MX_STEP_PIN;
    MX_STEP_PORT_SEL1 &= ~ MX_STEP_PIN;
    MX_STEP_PORT_DIR  |=   MX_STEP_PIN;
    MX_STEP_PORT_OUT  &= ~ MX_STEP_PIN;
    MY_STEP_PORT_SEL0 &= ~ MY_STEP_PIN;
    MY_STEP_PORT_SEL1 &= ~ MY_STEP_PIN;
    MY_STEP_PORT_DIR  |=   MY_STEP_PIN;
    MY_STEP_PORT_OUT  &= ~ MY_STEP_PIN;

    MX_LIMIT_PORT_SEL0  &= ~ (MX_LIMIT_PINS);
    MX_LIMIT_PORT_SEL1  &= ~ (MX_LIMIT_PINS);
    MX_LIMIT_PORT_DIR   &= ~ (MX_LIMIT_PINS);
    MX_LIMIT_PORT_REN   |=   (MX_LIMIT_PINS);
    MX_LIMIT_PORT_OUT   |=   (MX_LIMIT_PINS);
    MX_LIMIT_PORT_IE    |=   (MX_LIMIT_PINS);
    MX_LIMIT_PORT_IES   &= ~ (MX_LIMIT_PINS); // Check if high or low for High->Low Edge
    MX_LIMIT_PORT_IFG   &= ~ (MX_LIMIT_PINS);

    MY_LIMIT_PORT_SEL0  &= ~ (MY_LIMIT_PINS);
    MY_LIMIT_PORT_SEL1  &= ~ (MY_LIMIT_PINS);
    MY_LIMIT_PORT_DIR   &= ~ (MY_LIMIT_PINS);
    MY_LIMIT_PORT_REN   |=   (MY_LIMIT_PINS);
    MY_LIMIT_PORT_OUT   |=   (MY_LIMIT_PINS);
    MY_LIMIT_PORT_IE    |=   (MY_LIMIT_PINS);
    MY_LIMIT_PORT_IES   &= ~ (MY_LIMIT_PINS); // Check if high or low for High->Low Edge
    MY_LIMIT_PORT_IFG   &= ~ (MY_LIMIT_PINS);



    Steppers_Initialized = true;

}

#ifndef STEP_ACCEL
void wait();
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
void wait(uint32_t count);
void X_Step(uint32_t id_, uint32_t len_){
    static uint32_t id    = 0;
    static uint32_t len   = 0;
    static uint32_t count = 0;

    if(id_ != id){
        count = 0;
        len = len_;
        id = id_;
    }

    MX_STEP_PORT_OUT |=  MX_STEP_PIN;
    if (id == 0)
        wait(7);
    else if (count < 13)
        wait(20-count);
    else if (len-count < 13)
        wait(7+(len-count));
    else
        wait(7);
    MX_STEP_PORT_OUT &= ~MX_STEP_PIN;
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

    MY_STEP_PORT_OUT |=  MY_STEP_PIN;
    if (id == 0)
        wait(7);
    else if (count < 13)
        wait(20-count);
    else if (len-count < 13)
        wait(7+(len-count));
    else
        wait(7);
    MY_STEP_PORT_OUT &= ~MY_STEP_PIN;
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

uint8_t CheckXLimitSwitches(){
    uint8_t val = MX_LIMIT_PORT_IN;
    uint8_t ret = (val&(MX_LIMIT_PINS));
    return ret;
}

uint8_t CheckYLimitSwitches(){
    uint8_t val = MY_LIMIT_PORT_IN;
    uint8_t ret = (val&(MY_LIMIT_PINS));
    return ret;
}

bool XLim1(){
    return CheckXLimitSwitches() & MX_LIMIT_0_PIN;
}

bool XLim0(){
    return CheckXLimitSwitches() & MX_LIMIT_1_PIN;
}

bool YLim0(){
    return CheckYLimitSwitches() & MY_LIMIT_0_PIN;
}

bool YLim1(){
    return CheckYLimitSwitches() & MY_LIMIT_1_PIN;
}

void TestSteppers(){
    MX_LIMIT_PORT_IE  &= ~MX_LIMIT_PINS; // Diable interrupts
    MY_LIMIT_PORT_IE  &= ~MY_LIMIT_PINS;
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
    MX_LIMIT_PORT_IE  &= ~MX_LIMIT_PINS; // Diable interrupts
    MY_LIMIT_PORT_IE  &= ~MY_LIMIT_PINS;
    MX_ENABLE;
    MY_ENABLE;
    // Do calibration
    X_Steps = 0;
    Y_Steps = 0;

    while(!XLim0() && !XLim1()){
        X_Step_Backward(); //step backwards until xlim0 is hit, hopefully
    }
    if(XLim1()){ //this happens if the front limit switch is hit (it should be stepping backwards)
        error(direction_error_x);
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
        error(direction_error_y);
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

    MX_LIMIT_PORT_IFG &= ~MX_LIMIT_PINS; //clear flag, enable interrupts
    MX_LIMIT_PORT_IE  |=  MX_LIMIT_PINS;

    MY_LIMIT_PORT_IFG &= ~MY_LIMIT_PINS; //clear flag, enable interrupts
    MY_LIMIT_PORT_IE  |=  MY_LIMIT_PINS;
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
        error(bounds_error_x);
    }
    if(GlobalY_Position < 0 || GlobalY_Position > Y_Steps){
        __no_operation();
        error(bounds_error_y);
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
        error(steppers_not_initialized);
    }
    if(!Steppers_Calibrated){
        error(steppers_not_calibrated);
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
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;

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
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    WAIT_500MS;
    move_absolute(0,0);
}

#ifndef STEP_ACCEL
void wait(){
    __delay_cycles(24000);
}
#else
void wait(uint32_t count){
    for(count;count>0;count--){
        _delay_cycles(2400);
    }
}
#endif

void BumpInterruptInit(){
    MX_LIMIT_PORT_DIR &= ~MX_LIMIT_PINS; //set limit switch pins to inputs
    MX_LIMIT_PORT_REN |= MX_LIMIT_PINS; //internal resistors enabled
    MX_LIMIT_PORT_OUT |= MX_LIMIT_PINS; //internal resistors set to PULL-UP

    MX_LIMIT_PORT_IE |= MX_LIMIT_PINS; //enabled interrupts on bump pins
    MX_LIMIT_PORT_IES |= MX_LIMIT_PINS; //HIGH to LOW edge select trigger
    NVIC->ISER[1] |= 0x00000008; //enable interrupt 35 in the NVIC -  TRM 2.4.3.4
    NVIC->IP[8] &= ~0xff000000;  //maximum priority to interrupt 35 - TRM 2.4.3.19
    MX_LIMIT_PORT_IFG &= ~MX_LIMIT_PINS; //clear both flags, in case any appeared.

    //need to set up a PORT1_IRQHandler that includes:
    //clearing bump interrupt flags, setting an error condition,
    //and handling the two debug switches, if those are to be interrupt-driven.
    //consider the following
}


void PORT1_IRQHandler(void){
    uint16_t limit_flags = MX_LIMIT_PORT_IFG & MX_LIMIT_PINS; // record interrupt flags
    MX_LIMIT_PORT_IFG &= ~MX_LIMIT_PINS; //clear both bump interrupt flags
    if(limit_flags == BIT7){ //limit1, AKA high-side limit switch
        error(unknown); //TODO: new error code
    }
    if(limit_flags == BIT6){ //limit0, AKA low-side limit switch
        error(unknown); //TODO: new error code
    }
    if(limit_flags == BIT6 | BIT7){ //limit1 AND limit0, AKA both switches triggered? wacky but not unrealistic
        error(unknown); //TODO: new error code
    }
}


