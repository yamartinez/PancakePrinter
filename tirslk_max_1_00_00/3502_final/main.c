#include <stdint.h>
#include "msp.h"
#include "..\inc\Clock.h"
#include "..\inc\LaunchPad.h"
#include "Nokia5110.h"
/**
 * main.c
 */
void main(void)
{
    Clock_Init48MHz();
    LaunchPad_Init();
    Nokia5110_Init();
    while(1)
    {
        Nokia5110_Clear();
        Nokia5110_OutChar('B');
        Nokia5110_OutChar('I');
        Nokia5110_OutChar('L');
        Nokia5110_OutChar('G');
        Nokia5110_OutChar('E');
    }
}
