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

void BumpInterruptInit();

void InitStepperMotors(uint16_t X_dim, uint16_t Y_dim){

    X_DIM = X_dim;
    Y_DIM = Y_dim;

        // Configure IO Pins for Control Pins
    MX_ENABLE_PORT_SEL0 &= ~ MX_ENABLE_PIN;
    MX_ENABLE_PORT_SEL1 &= ~ MX_ENABLE_PIN;
    MX_ENABLE_PORT_DIR  |=   MX_ENABLE_PIN;
    MX_ENABLE_PORT_OUT  |=   MX_ENABLE_PIN; //set enables high to avoid locking motors/generating heat

    MY_ENABLE_PORT_SEL0 &= ~ MY_ENABLE_PIN;
    MY_ENABLE_PORT_SEL1 &= ~ MY_ENABLE_PIN;
    MY_ENABLE_PORT_DIR  |=   MY_ENABLE_PIN;
    MY_ENABLE_PORT_OUT  |=   MY_ENABLE_PIN;

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

    MX0_LIMIT_PORT_SEL0  &= ~ (MX_LIMIT_0_PIN);
    MX0_LIMIT_PORT_SEL1  &= ~ (MX_LIMIT_0_PIN);
    MX0_LIMIT_PORT_DIR   &= ~ (MX_LIMIT_0_PIN);
    MX0_LIMIT_PORT_REN   |=   (MX_LIMIT_0_PIN);
    MX0_LIMIT_PORT_OUT   |=   (MX_LIMIT_0_PIN);
    MX0_LIMIT_PORT_IE    &= ~ (MX_LIMIT_0_PIN);
    MX0_LIMIT_PORT_IES   &= ~ (MX_LIMIT_0_PIN); // Check if high or low for High->Low Edge
    MX0_LIMIT_PORT_IFG   &= ~ (MX_LIMIT_0_PIN);


    MX1_LIMIT_PORT_SEL0  &= ~ (MX_LIMIT_1_PIN);
    MX1_LIMIT_PORT_SEL1  &= ~ (MX_LIMIT_1_PIN);
    MX1_LIMIT_PORT_DIR   &= ~ (MX_LIMIT_1_PIN);
    MX1_LIMIT_PORT_REN   |=   (MX_LIMIT_1_PIN);
    MX1_LIMIT_PORT_OUT   |=   (MX_LIMIT_1_PIN);
    MX1_LIMIT_PORT_IE    &= ~ (MX_LIMIT_1_PIN);
    MX1_LIMIT_PORT_IES   &= ~ (MX_LIMIT_1_PIN); // Check if high or low for High->Low Edge
    MX1_LIMIT_PORT_IFG   &= ~ (MX_LIMIT_1_PIN);

    MY_LIMIT_PORT_SEL0  &= ~ (MY_LIMIT_PINS);
    MY_LIMIT_PORT_SEL1  &= ~ (MY_LIMIT_PINS);
    MY_LIMIT_PORT_DIR   &= ~ (MY_LIMIT_PINS);
    MY_LIMIT_PORT_REN   |=   (MY_LIMIT_PINS);
    MY_LIMIT_PORT_OUT   |=   (MY_LIMIT_PINS);
    MY_LIMIT_PORT_IE    &= ~ (MY_LIMIT_PINS);
    MY_LIMIT_PORT_IES   &= ~ (MY_LIMIT_PINS); // Check if high or low for High->Low Edge
    MY_LIMIT_PORT_IFG   &= ~ (MY_LIMIT_PINS);



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

    MX_STEP_PORT_OUT |=  MX_STEP_PIN;
    if (id == 0)
        wait(7);
    else if (count < 13)
        wait(20-count);
    else if (len-count < 13)
        wait(7+(len-count));
    else
        wait(7);
    MX_STEP_PORT_OUT &=  ~MX_STEP_PIN;
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
//    MX_ENABLE;
    MOTOR_X_FORWARD;
    X_Step(0,0);
//    MX_DISABLE;
}

void X_Step_Backward(){
//    MX_ENABLE;
    MOTOR_X_BACKWARD;
    X_Step(0,0);
//    MX_DISABLE;
}

void Y_Step_Forward(){
//    MY_ENABLE;
    MOTOR_Y_FORWARD;
    Y_Step(0,0);
//    MY_DISABLE;
}

void Y_Step_Backward(){
//    MY_ENABLE;
    MOTOR_Y_BACKWARD;
    Y_Step(0,0);
//    MY_DISABLE;
}

uint8_t CheckLimitSwitches(){

    uint8_t x0 = (MX_LIM0) ? 0 : 1; // x0 returns as 0 if P2.0 == 0
    uint8_t x1 = (MX_LIM1) ? 0 : 1; // x1 returns as 0 if P4.3 == 0
    uint8_t y0 = (MY_LIM0) ? 0 : 1; // y0 returns as 0 if P3.3 == 0
    uint8_t y1 = (MY_LIM1) ? 0 : 1; // y1 returns as 0 if P3.3 == 0
    uint8_t ret =  (x0 | (x1 << 1) | (y0 << 2) | (y1 << 3));
    return ret;
}

bool XLim0(){
    return CheckLimitSwitches() & 0x01;
}

bool XLim1(){
    return CheckLimitSwitches() & 0x02;
}

bool YLim0(){
    return CheckLimitSwitches() & 0x04;
}

bool YLim1(){
    return CheckLimitSwitches() & 0x08;
}

void TestSteppers(){
    MX0_LIMIT_PORT_IE  &= ~MX_LIMIT_0_PIN; // Disable interrupts
    MX1_LIMIT_PORT_IE  &= ~MX_LIMIT_1_PIN;
    MY_LIMIT_PORT_IE &= ~(MY_LIMIT_0_PIN | MY_LIMIT_1_PIN);
    MX_ENABLE;
    MY_ENABLE;
    while(1){
        uint8_t i = 0;
        for(i = 200; i>0; i--){
            X_Step_Forward();
            Y_Step_Forward();
        }
        for(i = 200; i>0; i--){
            X_Step_Backward();
            Y_Step_Backward();
        }
    }
}

void CalibrateSteppers(){
    MX0_LIMIT_PORT_IE  &= ~MX_LIMIT_0_PIN; // Disable interrupts
    MX1_LIMIT_PORT_IE  &= ~MX_LIMIT_1_PIN;
    MY_LIMIT_PORT_IE &= ~(MY_LIMIT_0_PIN | MY_LIMIT_1_PIN);

    // Do calibration
    X_Steps = 0;
    Y_Steps = 0;
   // LEDBon(); // blue led - stage 1, x backwards calibration
    MX_ENABLE;
    MY_DISABLE;
    //reversed direction of the steppers, need to double check if this affects the calibration sequence.
    while(!XLim0() && !XLim1()){
//        MX_ENABLE;
        X_Step_Backward(); //step backwards until xlim0 is hit, hopefully
//        MX_DISABLE;
    }
    if(XLim0()){ //this happens if the front limit switch is hit (it should be stepping backwards)
        error(direction_error_x,__FILE__,__LINE__);
    }
    while(XLim1()){ //step forward a bit so we are not on the switch in the back
//        MX_ENABLE;
        X_Step_Forward();
//        MX_DISABLE;
    }
    // LEDBoff(); //stage 1 over, stage 2 now, green LED on
    // LEDGon();
    while(!XLim0()){ //step forward until the front limit switch is hit, and count how many steps it takes to get there
//        MX_ENABLE;
        X_Step_Forward();
        X_Steps++;
//        MX_DISABLE;

    }
    while(XLim1()){ // step back enough to get off the front limit switch, and deduct those steps from the total of x steps
//        LEDGoff();
//        MX_ENABLE;
        X_Step_Backward();
        X_Steps--;
//        MX_DISABLE;
    }

    MX_DISABLE;
    MY_ENABLE; //begin y-cal
    // LEDBon(); //stage 2 over, stage 3 start so turn the blue + green on as well
    while(!YLim0() && !YLim1()){ // same process as with the x (literally the same read those comments)
//        MY_ENABLE;
        Y_Step_Backward();
//        MY_DISABLE;
    }
    if(YLim0()){
        error(direction_error_y,__FILE__,__LINE__);
    }
    while(YLim1()){
        Y_Step_Forward();
    }
    while(!YLim0()){
        Y_Step_Forward();
        Y_Steps++;
    }
    while(YLim0()){
        Y_Step_Backward();
        Y_Steps--;
    }
//    stage 3 over, turn green off, turn blue on
    // LEDRon();
    X_Steps -= 100;
    Y_Steps -= 100;
    uint16_t i = 0;
    MX_ENABLE;
    MY_DISABLE;
    for(i = X_Steps;i>0;i--){ //move back together to the 0,0 coordinate
        X_Step_Backward();
    }
    MY_ENABLE;
    MX_DISABLE;

    for(i = Y_Steps;i>0;i--){ //move back together to the 0,0 coordinate
        Y_Step_Backward();
    }
    MX_DISABLE; // deactivate both steppers now.
    MY_DISABLE;

    X_Steps-=100;
    Y_Steps-=100;

    Steppers_Calibrated = true;

    MX0_LIMIT_PORT_IFG &= ~MX_LIMIT_0_PIN; //clear flag, enable interrupts
    MX1_LIMIT_PORT_IFG &= ~MX_LIMIT_1_PIN;
    MY_LIMIT_PORT_IFG &= ~(MY_LIMIT_0_PIN | MY_LIMIT_1_PIN);
    MX0_LIMIT_PORT_IE  |=  MX_LIMIT_0_PIN;
    MX1_LIMIT_PORT_IE  |=  MX_LIMIT_1_PIN;
    MY_LIMIT_PORT_IE  |= (MY_LIMIT_0_PIN | MY_LIMIT_1_PIN);

    GlobalX_Position = 0;
    GlobalY_Position = 0;

    X_Image_Offset = (X_Steps>>1) - (X_DIM>>1);
    Y_Image_Offset = (Y_Steps>>1) - (Y_DIM>>1);

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
    uint16_t i = 0;
    if (x!=none && y!=none){
        if(x_count > y_count){
            for(i = y_count; i>0; i--){
                X_Step(id,x_count);
                Y_Step(id,y_count);
            }
            MY_DISABLE;
            for(i = x_count-y_count; i>0;i--){
                X_Step(id,x_count);
            }
            MX_DISABLE;
        } else if (y_count > x_count){
            for(i = x_count; i>0; i--){
                X_Step(id,x_count);
                Y_Step(id,y_count);
            }
            MX_DISABLE;
            for(i = y_count-x_count; i>0;i--){
                Y_Step(id,y_count);
            }
            MY_DISABLE;
        } else {
            for(i = x_count; i>0; i--){
                X_Step(id,x_count);
                Y_Step(id,y_count);
            }
            MX_DISABLE;
            MY_DISABLE;
        }
    }
    else if(x!=none){
        for(i = x_count; i > 0; i--){
                X_Step(id,x_count);
        }
    } else if(y!=none){
        for(i = y_count; i > 0; i--){
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
    x = (x<0 ? -1*x : x);
    y = (y<0 ? -1*y : y);

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
    uint16_t i = 0;
    for(i = (Y_DIM>>4); i > 0; i--){
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
        _delay_cycles(500);
    }
}
#endif

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT4_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
    error(unknown,__FILE__,__LINE__);
}
