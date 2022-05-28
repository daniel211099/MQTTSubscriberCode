/*
 * initialFunctions.c
 *
 *  Created on: 09.05.2022
 */

#include "initialFunctions.h"

#define PWM_FREQUENCY 200      // PWM frequency must be > 100 Hz for human eyes
 #define BUZZER_FREQUENCY 1000 //Buzzer frequency must be higher than 20Hz and lower than 20kHz for human ears
uint32_t ui32PWMClock;
uint32_t ui32Load;
uint32_t ui32BuzzerIndex = 1;



//for the game every 1.0 seconds
//Input: uint32_t Clock is the value of the system clock frequency
void ConfigureTimer1(uint32_t Clock)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);       //Enable timer 1 peripheral
    TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);    //Configurs timer 1
    TimerLoadSet(TIMER1_BASE,TIMER_A,(Clock-1));        //Sets the Load Value of the timer
    TimerIntEnable(TIMER1_BASE,TIMER_TIMA_TIMEOUT);     //Enables the Interrupt for timer 1
    IntEnable(INT_TIMER1A);
    IntMasterEnable();
    //TimerEnable(TIMER1_BASE,TIMER_A);
}

void initRGB(uint32_t ui32SysClock) {
    //Enable Peripherals//
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK); //Red & Green LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOM); //Blue LED
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); //PWM0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2); //Enable Timer 2 peripheral

    SysCtlDelay(5); // wait a bit before accessing

    // BoosterPack Fct. BoosterPack Con.    LaunchPad Pin/Fct.  Config Parameter
    // RED              J4-37                PK4 / PWM6           PWM_Out_2_BIT
    // GREEN            J4-38                PK35 / PWM7           PWM_Out_3_BIT
    // BLUE             J4-39                PM0 / CCP0           PWM_Out_4_BIT
    //T2CCP0 hat GPIOPCTL Registernummer 0X001

    SysCtlDelay(5); // wait a bit before accessing

    //Setup PWM-LED's//
    //Pin Configure
    GPIOPinConfigure(GPIO_PK4_M0PWM6); //Pin Configure Multiplexer Red LED
    GPIOPinConfigure(GPIO_PK5_M0PWM7); //Pin Configure Multiplexer Green LED
    //PWM Pins, configure all three RGB LED pins to PWM
    GPIOPinTypePWM(GPIO_PORTK_BASE, GPIO_PIN_4); //Configure Pin as PWM Red LED
    GPIOPinTypePWM(GPIO_PORTK_BASE, GPIO_PIN_5); //Configure Pin as PWM Green LED
    //    GPIOPinType(GPIO_PORTG_BASE, GPIO_PIN_0);


    //PWM CLock set
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_1); // 120MHz/64 = 1.875MHz
    // PWM Configuration (Red LED)
    PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC |PWM_GEN_MODE_DBG_RUN); //Generator 3 in Countdown Mode
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, PWM_FREQUENCY); //Period Set

    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, 1);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 1);

    // PWM Configuration (Green LED)
    PWMGenEnable(PWM0_BASE, PWM_GEN_3); //Start Timer in Generator 1 for Red & Green LED
    PWMOutputState(PWM0_BASE, PWM_OUT_6_BIT, true); //Enable the Outputs of Red & Green LED
    PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT , true);

    ////    // PWM Configuration (Blue LED)/////////
    //SysCtlPeripheralPowerOn(SYSCTL_PERIPH_TIMER2); //Enable Power of Timer2 Peripheral, even if it's clock is not activated
    GPIOPinConfigure(GPIO_PM0_T2CCP0);//Configure PM0 as Timer to Comperator
    GPIOPinTypeTimer(GPIO_PORTM_BASE, GPIO_PIN_0); //Configure CCP Pin as Timer
    TimerClockSourceSet(GPIO_PORTM_BASE, TIMER_CLOCK_SYSTEM);
    TimerConfigure(TIMER2_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PWM); //Configure Timer 2 as PWM

    TimerLoadSet(TIMER2_BASE, TIMER_A, PWM_FREQUENCY);
    TimerControlLevel(TIMER2_BASE, TIMER_A, false);
    TimerEnable(TIMER2_BASE, TIMER_A);
    TimerMatchSet(TIMER2_BASE, TIMER_A, PWM_FREQUENCY - 1);

    lightSingleMode(0);

    //      //    // PWM Configuration (Blue LED) END/////////
}



//******Flashlight single mode start******//
void lightSingleMode(int lightcolor) {

  //Initialize Colors
  int red = 1;
  int green = 1;
  int blue = 1;

  switch(lightcolor)
  {
  case 0 : red=1; green=1; blue=1; break;           //OFF
  case 1 : red=255; green=1; blue=1; break;         //Red
  case 2 : red=1; green=255; blue=1; break;         //Green
  case 3 : red=1; green=1; blue=255; break;         //Blue
  case 4 : red=255; green=255; blue=1; break;       //Yellow
  case 5 : red=255; green=1; blue=255; break;       //Purple
  case 6 : red=1; green=255; blue=255; break;       //Turquoise
  case 7 : red=255; green=255; blue=255; break;     //White
  }


  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, red);
  PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, green);
  TimerMatchSet(TIMER2_BASE, TIMER_A,PWM_FREQUENCY-blue);


}
//******Flashlight single mode end******//

void initBuzzer(uint32_t ui32SysClock){
    //Enable Peripherals//
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); //PWM0
    //Setup Buzzer start//
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0); //Enable peripheral PWM1
    //Pin Configure
    GPIOPinTypePWM(GPIO_PORTG_BASE, GPIO_PIN_1); //Configure GPIO Pin 1 as an PWM
    GPIOPinConfigure(GPIO_PG1_M0PWM5);
    //PWM CLock set
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_64); // 120MHz/64 = 1.875MHz
    //Calculate PWM load values
    ui32PWMClock = ui32SysClock / 64; // 120 MHz/64
    ui32Load = (ui32PWMClock / BUZZER_FREQUENCY) - 1; // (1875000/1000)-1 = 18749
    //  Since the PWM reloads at zero, we subtract one.
    PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_DBG_RUN); //Generator 1 of PWM1 in Countdown Mode
    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, ui32Load); //Period Set
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, ui32Load / 2); //Set the Pulswidth of the Buzzer to generate a tone
    PWMGenEnable(PWM0_BASE, PWM_GEN_2); //Start Timer in Generator 1 for Buzzer
}

void buzzing(bool high) {

  PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, true); //Enable the Outputs of Buzzer


  if (high==true){
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 131800 ); //Set the pulsewidth of the Buzzer to generate a tone
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, (1874) * ui32BuzzerIndex); //Set pulsewidth Red LED synchronized with the buzzer
  }else{
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_5, 131300 ); //Set the pulsewidth of the Buzzer to generate a tone
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, (187) * ui32BuzzerIndex);
  }
}

void endbuzz(){
    PWMOutputState(PWM0_BASE, PWM_OUT_5_BIT, false); //Disable the Outputs of Buzzer
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, 1);
}

 void ConfigureControlGPIO() {

    GPIODirModeSet(GPIO_PORTH_BASE, GPIO_PIN_1, GPIO_DIR_MODE_IN); //specifies that the PH1 (Push button 1) is programmed as a software controlled input
    GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //specifies that the PH1 (Push button 1) have a 2mA output drive strength with a weak pull up
    GPIODirModeSet(GPIO_PORTK_BASE, GPIO_PIN_6, GPIO_DIR_MODE_IN); //specifies that the PK6 (Push button 2) is programmed as a software controlled input
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_6, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); //specifies that the PK6 (Push button 2) have a 2mA output drive strength with a weak pull up
  }
