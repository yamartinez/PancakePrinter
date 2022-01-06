/*
 * Commands.c
 *
 *  Created on: Dec 5, 2021
 *      Author: yudel
 */

#include "./Commands.h"

Command queue[256*2];
uint16_t start, end, size = 0;

int8_t EnqueueCommand(Command*c){
    if(size == 256*2)
        return -1;
    queue[start] = *c;
    start++;
    if (start == 256*2)
        start = 0;
    size++;
    return 1;
}

Command* DequeueCommand(){
    if (size == 0)
        return 0;
    size --;
    Command * c = &queue[end];
    end++;
    if (end == 256*2)
        end = 0;
    return c;
}

uint16_t QueueSize(){
    return size;
}

void ExecuteCommand(Command *c){
    switch(c->command){
        case move_dry:
            CarriageMove(c->arg0,c->arg1,0);
            break;
        case move_wet:
            CarriageMove(c->arg0,c->arg1,1);
            break;
        case time_delay:
            CarriageWait(c->arg0);
            break;
        case done:
            CarriageDone();
            break;
    };
}

int8_t BuildCommand(uint8_t d){
    static Command tempCommand;
    static uint8_t count = 0;
    CommandType data = (CommandType) d;
    if (count == 0){
        if(data > 3){
            count = 0;
            return -1;
        }
        tempCommand.command = (CommandType) data;
        if(tempCommand.command != done){
            count ++;
        } else {
            count = 0;
            return EnqueueCommand(&tempCommand);
        }
    }
    else if (count == 1){
        tempCommand.arg0 = data;
        count ++;
    }
    else if (count == 2){
        tempCommand.arg0 += (data << 8);
        if(tempCommand.command != time_delay){
            count ++;
        } else {
            count = 0;
            return EnqueueCommand(&tempCommand);
        }
    }
    else if (count == 3){
        tempCommand.arg1 = data;
        count ++;
    }
    else if (count == 4){
        tempCommand.arg1 += (data << 8);
        count = 0;
        return EnqueueCommand(&tempCommand);
    }

    return 0;
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A1_VECTOR))) USCI_A1_ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(UCA1IV,USCI_UART_UCTXCPTIFG))
  {
    case USCI_NONE: break;
    case USCI_UART_UCRXIFG:
      while(!(UCA1IFG&UCTXIFG));
      int8_t val = BuildCommand(UCA1RXBUF);
      if(val){
          UCA1TXBUF = val;//UCA1RXBUF;
      }
      __no_operation();
      break;
    case USCI_UART_UCTXIFG: break;
    case USCI_UART_UCSTTIFG: break;
    case USCI_UART_UCTXCPTIFG: break;
    default: break;
  }
}