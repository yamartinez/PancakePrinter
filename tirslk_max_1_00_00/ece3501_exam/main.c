#include "../inc/Clock.h"
#include "../inc/CortexM.h"
#include "../inc/PWM.h"
#include "../inc/LaunchPad.h"
#include "../inc/TExaS.h"
#include "../inc/AP.h"
#include "../inc/UART0.h"
#include "../inc/Bump.h"
#include "../inc/Reflectance.h"
#include "../inc/Motor.h"
#include "../inc/TimerA1.h"
#include "../inc/SysTickInts.h"
#include "../inc/Nokia5110.h"
/*(Left,Right) Motors, call LaunchPad_Output (positive logic)
3   1,1     both motors, yellow means go straight
2   1,0     left motor,  green  means turns right
1   0,1     right motor, red    means turn left
0   0,0     both off,    dark   means stop
(Left,Right) Sensors, call LaunchPad_Input (positive logic)
3   1,1     both buttons pushed means on line,
2   1,0     SW2 pushed          means off to right - oscillate for 1,0 between 11 and 01
1   0,1     SW1 pushed          means off to left - oscillate for 0,1 between 11 and 10
0   0,0     neither button      means lost - if it was 11 before, stop, if it was 10 or 01, enter lost mode
 */
/* start of testcode */
struct State {
  uint32_t light_out;                // 2-bit output
  void (*motorAction)(uint16_t, uint16_t) ; //a motor function for this to call
  uint16_t right_out; //right PWM output
  uint16_t left_out; //left PWM output
  uint32_t delay;              // time to delay in 1ms (only will be used for the "straight" commands
  const struct State *next[4]; // Next if 2-bit input is 0-3
};

typedef const struct State State_t;
volatile uint32_t INPUT; //global variable used to track the necessary input to this FSM
uint32_t OUTPUT; //global variable used to track the desired output from this FSM
volatile uint32_t INTERRUPT_COUNTER; //global pointer to track triggers of our periodic interrupt
volatile uint16_t LEFT_TRACKER;
volatile uint16_t RIGHT_TRACKER;
volatile uint8_t RAW_INPUT;
volatile int32_t signed_input;
//#define notOff 3
//#define offLeft 2
//#define offRight 1
//#define lost 0
//
//#define Center &fsm[0]
//
//#define Right1 &fsm[1]
//#define Right2 &fsm[2]
//
//#define HardL &fsm[3]
//
//#define Rstraight &fsm[4]
//
//#define Stop &fsm[5]
//
//#define Left1 &fsm[6]
//#define Left2 &fsm[7]
//
//#define HardR &fsm[8]
//
//#define Lstraight &fsm[9]
 // the nested array is indexed 0-3, which is what reflectance can return if we only use 2 bits
//problem 1- we want to use all 8 bits, which would necessitate a great deal of struggle in creating many possible "next states"
//solution 1- make most of them invalid inputs and have them return to the current state
//problem 2- what states do we actually need? what can we ignore?
//solution 2- I believe we can make a few left side states and a few right side states so that in the case of the bot looking like this
/**
 *         ___________________
 *       /   o o o o o o o o   \
 *       |                     |
 *       |                     |
 *        \____________________/
 *  we can hopefully always assume that only one wing of these should be lit at the same time. so i can assume the inputs will range from
 *
 *          1 1 1 1 1 0 0 0
 *       to 0 0 0 1 1 0 0 0
 *       to 0 0 0 1 1 1 1 1
 *       and possibly one state where we look like 1 1 1 1 1 1 1 1
 *  perhaps a readLeft() and readRight() set of methods are called for, so we can base our logic off 5 bits of input instead of 8 bits
 *  alternatively -- one possibility is that we instead choose to simplify our decision making process -- mapping the 8 bit dataset we have to
 *  the 4 existing possibilities - with a few special cases that trigger specialized out of state behavior before resuming regular behavior
 * or we could use the preexisting Reflectance_Position() function and make decisions based on that
 */
//State_t fsm[10]={ //the outputs need to correspond to the colors
//  //as well as motor functions to call and
//  //duty cycles for the wheels (in order light, function, right duty, left duty, next)
//  {0x07, &Motor_Forward,5000, 5000, 10 ,{ Stop,Left1,Right1,Center}},  //Center - white
//  {0x01, &Motor_Forward,2500, 5000, 10 ,{ HardL,Left1,Right2,Center}},  //Right1 - red (we need to oscillate between these two right states)
//  {0x05, &Motor_Forward, 4000,5000,10 ,{ HardL,Left1,Right1,Center}},  //Right2 - pink
//  {0x03, &Motor_Forward,5000 ,5000, 10,{ Lstraight,Lstraight,Lstraight,Lstraight}},  //HardL - yellow
//  {0x02, &Motor_Forward,500 ,500, 500,{ Stop,Right1,Right1,Center}},  //Rstraight - green & baseLED is on
//  {0x00, &Motor_Stop, 5000,5000, 10,{ Stop,Stop,Stop,Stop}},  //Stop - dark - replace with LOST stage
//  {0x00, &Motor_Forward, 3000, 3000, 10, {Lost, Left1, Right1, Center}},
//  {0x04, &Motor_Forward,5000 ,2500,  10,{ HardR,Left2,Right1,Center}},  //Left1 - blue
//  {0x06, &Motor_Forward,5000,4000, 10,{ HardR,Left1,Right1,Center}},  //Left2 - cyan
//  {0x02, &Motor_Forward, 5000, 5000, 10,{ Rstraight,Rstraight,Rstraight,Rstraight}},  //HardR - green
//  {0x03, &Motor_Forward, 500,500, 500,{ Stop,Left1,Right1,Center}}   //Lstraight - yellow & baseLED is on
//};

void Motor_search_left(uint16_t leftDuty, uint16_t rightDuty){
    if(INTERRUPT_COUNTER % 10 < 3)
        Motor_Forward(leftDuty, rightDuty);
    else if(INTERRUPT_COUNTER %10 > 2)
        Motor_Left(leftDuty, rightDuty);
}

void Motor_search_right(uint16_t leftDuty, uint16_t rightDuty){
    if(INTERRUPT_COUNTER % 10 < 3)
        Motor_Forward(leftDuty, rightDuty);
    else if(INTERRUPT_COUNTER %10 > 2)
        Motor_Right(leftDuty, rightDuty);
}






#define Center &fsm[0]
#define Right &fsm[1]
#define LostR &fsm[2]
#define LostC &fsm[3]
#define Left &fsm[4]
#define LostL &fsm[5]
#define Stop &fsm[6]
#define LostC_2 &fsm[7]
uint16_t repeatCounter = 0;
//we're drafting a new FSM
State_t fsm[8]={ //the outputs need to correspond to the colors
  //as well as motor functions to call and
  //duty cycles for the wheels (in order light, function, right duty, left duty, next
  {0x07, &Motor_Forward,7000, 7000, 10 ,{ LostC,Right,Left,Center}},  //Center - white fsm[0]
  {0x01, &Motor_Forward, 4500, 5000, 15 ,{ LostR,Right,Left,Center}},  //Right - red fsm[1]
  {0x03, &Motor_Left, 6500, 6500, 10,{ LostR,Right,Left,Center}},  //LostR - yellow fsm[2]
  {0x02, &Motor_Forward, 5000,5000, 10,{LostC_2,Right,Left,Center}},  //LostC - dark fsm[3]
  {0x04, &Motor_Forward, 5000, 4500, 15,{ LostL,Right,Left,Center}},  //Left - blue fsm[4]
  {0x06, &Motor_Right, 6500,6500, 14,{ LostL,Right,Left,Center}},  //LostL - cyan fsm [5]
  {0x00, &Motor_Stop, 4000, 4000, 20, {Stop,Stop,Stop,Stop}}, //Stop - green fsm[6]
  {0x02, &Motor_Left, 4000, 3000, 30, {LostC, Right, Left, Center}} //LostC_2
};

// built-in LED1 connected to P1.0
// negative logic built-in Button 1 connected to P1.1
// negative logic built-in Button 2 connected to P1.4
// built-in red LED connected to P2.0
// built-in green LED connected to P2.1
// built-in blue LED connected to P2.2
// Color    LED(s) Port2
// dark     ---    0
// red      R--    0x01
// blue     --B    0x04
// green    -G-    0x02
// yellow   RG-    0x03
// cyan     -GB    0x06
// white    RGB    0x07
// pink     R-B    0x05

/*Run FSM continuously
1) Output depends on State (LaunchPad LED)
2) Wait depends on State
3) Input (LaunchPad buttons)
4) Next depends on (Input,State)
 */

State_t *Spt;  // global pointer to the current state


void updateScreen(void){
    Nokia5110_SetCursor(0,1);
    Nokia5110_OutUDec(RAW_INPUT);
}
void BumpResponse(void){
  //reflectance_read() uses busy waits, but

    if(Bump_Read() != 63){
        if(Spt == LostC || Spt == LostC_2 || Spt == Stop){
            Motor_Stop(1,1);
            Spt = Stop;
        }
       DisableInterrupts();
        LaunchPad_Output(0x01);
        Motor_Backward(3000,3000);//reverse 1 sec
        Clock_Delay1ms(1000);
        LaunchPad_Output(0x03);
        Motor_Left(3000,3000); // right turn 2 sec
        Clock_Delay1ms(1500);
        EnableInterrupts();
        LaunchPad_Output(0);
    }
}
void classify_input(uint8_t data){

    if(data == 255)
            INPUT = 1; //we have perpendicular start - go right
    else if(data == 0)
            INPUT = 0; //we're LOST!
    else{
          //we'll use this handy (and fast) function to help us decide our next state
      int32_t mod_data = Reflectance_Position(data);
      if(mod_data > -80 && mod_data < 80)
          INPUT = 3; //negative means off right
      else if(mod_data < -80)
          INPUT = 2; //less than center means left
      else
          INPUT = 1; //positive means off right;
        }
}


void SysTick_Handler(void){ // every 1ms (this one is for Reflectance)
    INTERRUPT_COUNTER++; // increment our global interrupt counter
    if(INTERRUPT_COUNTER % 10 == 1) { //once every 10 ms, start ref
    Reflectance_Start(); // begin reading
    }
    if(INTERRUPT_COUNTER % 10 == 2) { //once every 10 ms, 1 ms after starting ref, end ref
    RAW_INPUT = Reflectance_End(); //finish reading and give the input to our FSM.
    classify_input(RAW_INPUT);
    }
}


/**
 * main.c
 */

void main(void){
     Clock_Init48MHz();
     LaunchPad_Init();
     SysTick_Init(48000,2); // used for reflectance (priority 2)
     Bump_Init();
     Reflectance_Init();
     Nokia5110_Init();
     Motor_Init(); //covers PWM output
     TimerA1_Init(&BumpResponse, 1500); //used for bumps (priority 0)
     Spt = Center;
     Nokia5110_Clear();
     Nokia5110_OutString("reflect:    ");
     EnableInterrupts();
     repeatCounter = 0;
    while(1){
        OUTPUT = Spt->light_out;            // set output from FSM
        LaunchPad_Output(OUTPUT);     // do output to LED
        LEFT_TRACKER = Spt->left_out;
        RIGHT_TRACKER = Spt->right_out;
        (Spt->motorAction)(LEFT_TRACKER, RIGHT_TRACKER); //call this state's action
        updateScreen();
            if((OUTPUT == 0x03 && Spt -> next[INPUT] == Stop) || (OUTPUT == 0x02 && Spt-> next[INPUT] == Stop) ){
                LaunchPad_LED(1);}
            else
            { LaunchPad_LED(0);}
           // TExaS_Set(Input<<2|Output);   // optional, send data to logic analyzer
            Clock_Delay1ms(Spt->delay);   // wait (should be a low input unless it's in phase straightL or straightR
            //Input = LaunchPad_Input();//reflectance_center?    // read sensors - input is done by systick interrupt
            if(Spt->next[INPUT] == Spt)
                repeatCounter++;
            else
                repeatCounter = 0;
            if(repeatCounter > 100)
                Spt = LostC;
            Spt = Spt->next[INPUT];       // next depends on input and states
    }
}
