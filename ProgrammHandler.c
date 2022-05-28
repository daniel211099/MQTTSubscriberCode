#include "ProgrammHandler.h"

//The Timer1 Interrupt Handler counts down and buzzers if 0 is reached

void Timer1IntHandler(void){

    //clear the timer interrupt
    TimerIntClear(TIMER1_BASE,TIMER_TIMA_TIMEOUT);
}
