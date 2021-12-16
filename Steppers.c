/*
 * Steppers.c
 *
 *  Created on: Oct 18, 2021
 *      Author: yudel
 * version 2.0 - for msp432p401r
 */

#include "./Steppers.h"

uint16_t X_Steps, Y_Steps = 0;
int16_t  GlobalX_Position, GlobalY_Position  = 0;
uint16_t X_DIM, Y_DIM, Y_Steps_per_unit, X_Steps_per_unit, X_Steps_per_unit_diff = 0;
bool Steppers_Initialized = false;
bool Steppers_Calibrated = false;



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

    M_LIMIT_PORT_SEL0  &= ~ (M_LIMIT_PINS);
    M_LIMIT_PORT_SEL1  &= ~ (M_LIMIT_PINS);
    M_LIMIT_PORT_DIR   &= ~ (M_LIMIT_PINS);
    M_LIMIT_PORT_REN   |=   (M_LIMIT_PINS);
    M_LIMIT_PORT_OUT   &= ~ (M_LIMIT_PINS);
    M_LIMIT_PORT_IE    |=   (M_LIMIT_PINS);
    M_LIMIT_PORT_IES   &= ~ (M_LIMIT_PINS); // Check if high or low for High->Low Edge
    M_LIMIT_PORT_IFG   &= ~ (M_LIMIT_PINS);

    Steppers_Initialized = true;
    
}

void X_Step(){
    M_STEP_PORT_OUT |=  MX_STEP_PIN;
    wait();
    M_STEP_PORT_OUT &= ~MX_STEP_PIN;
    wait();
}

void Y_Step(){
    M_STEP_PORT_OUT |=  MY_STEP_PIN;
    wait();
    M_STEP_PORT_OUT &= ~MY_STEP_PIN;
    wait();
}

void X_Step_Forward(){
    MOTOR_X_FORWARD;
    X_Step();
}

void X_Step_Backward(){
    MOTOR_X_BACKWARD;
    X_Step();
}

void Y_Step_Forward(){
    MOTOR_Y_FORWARD;
    Y_Step();
}

void Y_Step_Backward(){
    MOTOR_Y_BACKWARD;
    Y_Step();
}

uint8_t CheckLimitSwitches(){
    uint8_t val = M_LIMIT_PORT_IN;
    uint8_t ret =  (val&(M_LIMIT_PINS));
    return ret;
}

bool XLim0(){
    return CheckLimitSwitches() & M_LIMIT_0_PIN;
}

bool XLim1(){
    return CheckLimitSwitches() & M_LIMIT_1_PIN;
}

bool YLim0(){
    return CheckLimitSwitches() & M_LIMIT_0_PIN;
}

bool YLim1(){
    return CheckLimitSwitches() & M_LIMIT_1_PIN;
}

void CalibrateSteppers(){
    M_LIMIT_PORT_IE  &= ~M_LIMIT_PINS; // Disable interrupts

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
    //now that space calibration is complete, enable interrupts on the bump switches. write this ISR.
    // M_LIMIT_PORT_IFG &= ~M_LIMIT_PINS; //clear flag, enable interrupts
    // M_LIMIT_PORT_IE  |=  M_LIMIT_PINS;
    BumpInterruptInit(); //activate interrupts on the bump sensors

    uint16_t diff = X_Steps - Y_Steps; //

    for(uint16_t i = 0;i<Y_Steps;i++){ //move back together to the 0,0 coordinate
        X_Step_Backward();
        Y_Step_Forward();
    }
    for(uint16_t i = 0;i<diff;i++){ // since x is longer than y, keep moving the x back to the 0,0 coordinate by itself
        X_Step_Backward();
    }

    Y_Steps_per_unit = Y_DIM / Y_Steps;
    X_Steps_per_unit = X_DIM / X_Steps;
    X_Steps_per_unit_diff = X_Steps_per_unit - Y_Steps_per_unit;

    Steppers_Calibrated = true;
}

void move1(direction x, direction y){
    if(!Steppers_Initialized){
        error(steppers_not_initialized);
    }
    if(!Steppers_Calibrated){
        error(steppers_not_calibrated);
    }

    if(x == forward){
        MOTOR_X_FORWARD;
        GlobalX_Position ++;
    } else {
        MOTOR_X_BACKWARD;
        GlobalX_Position --;
    }
    if(y == forward){
        MOTOR_Y_FORWARD;
        GlobalY_Position ++;
    } else {
        MOTOR_Y_BACKWARD;
        GlobalY_Position --;
    }

    if(GlobalX_Position < 0 || GlobalX_Position > X_DIM){
        error(bounds_error_x);
    }
    if(GlobalY_Position < 0 || GlobalY_Position > Y_DIM){
        error(bounds_error_y);
    }

    uint16_t min_steps = X_Steps_per_unit <= Y_Steps_per_unit ? X_Steps_per_unit : Y_Steps_per_unit;
    uint16_t diff = X_Steps_per_unit <= Y_Steps_per_unit ? Y_Steps_per_unit-X_Steps_per_unit : X_Steps_per_unit-Y_Steps_per_unit;
    for(uint16_t i = 0; i < min_steps; i++){
        if(x){ //number of x steps to move 1 mm will always be more than y steps to move 1mm
            X_Step();
        }
        if(y){
            Y_Step();
        }
    }
    if(diff){
        if(x && X_Steps_per_unit < Y_Steps_per_unit){
            for(uint16_t i = 0; i < diff; i++){
                X_Step();
            }
        } else if (y && Y_Steps_per_unit < X_Steps_per_unit){
            for(uint16_t i = 0; i < diff; i++){
                Y_Step();
            }
        }
    }

}

void move_relative(int16_t x, int16_t y){
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

}

void move_absolute(int16_t x, int16_t y){
    move_relative(x-GlobalX_Position, y-GlobalY_Position);
}

void wait(){
    __delay_cycles(1000000);  // .5s/4
}


void BumpInterruptInit(){
    M_LIMIT_PORT_DIR &= ~M_LIMIT_PINS; //set limit switch pins to inputs
    M_LIMIT_PORT_REN |= M_LIMIT_PINS; //internal resistors enabled
    M_LIMIT_PORT_OUT |= M_LIMIT_PINS; //internal resistors set to PULL-UP

    M_LIMIT_PORT_IE |= M_LIMIT_PINS; //enabled interrupts on bump pins 
    M_LIMIT_PORT_IES |= M_LIMIT_PINS; //HIGH to LOW edge select trigger
    NVIC->ISER[1] |= 0x00000008; //enable interrupt 35 in the NVIC -  TRM 2.4.3.4
    NVIC->IP[8] &= ~0xff000000;  //maximum priority to interrupt 35 - TRM 2.4.3.19
    M_LIMIT_PORT_IFG &= ~M_LIMIT_PINS; //clear both flags, in case any appeared.
    
    //need to set up a PORT1_IRQHandler that includes:
    //clearing bump interrupt flags, setting an error condition,
    //and handling the two debug switches, if those are to be interrupt-driven.
    //consider the following
}

void PORT1_IRQHandler(void){
    uint16_t limit_flags = M_LIMIT_PORT_IFG & M_LIMIT_PINS; // record interrupt flags
    M_LIMIT_PORT_IFG &= ~M_LIMIT_PINS; //clear both bump interrupt flags
    if(limit_flags == BIT7){ //limit1, AKA high-side limit switch
        error(); //TODO: new error code
    }
    if(limit_flags == BIT6){ //limit0, AKA low-side limit switch
        error(); //TODO: new error code
    }
    if(limit_flags == BIT6 | BIT7){ //limit1 AND limit0, AKA both switches triggered? wacky but not unrealistic
        error(); //TODO: new error code
    }
}


