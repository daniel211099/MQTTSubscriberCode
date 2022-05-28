//*****************************************************************************
//
// io.c - I/O routines for the enet_io example application.
//
// Copyright (c) 2013-2020 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.2.0.295 of the EK-TM4C129EXL Firmware Package.
//
//*****************************************************************************
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_pwm.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pwm.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "utils/ustdlib.h"
#include "io.h"
#include "driverlib/adc.h"

//*****************************************************************************
//
// Hardware connection for the user LED.
//
//*****************************************************************************
#define LED_PORT_BASE GPIO_PORTN_BASE
#define LED_PIN GPIO_PIN_0

//*****************************************************************************
//
// Hardware connection for the animation LED.
//
//*****************************************************************************
#define LED_ANIM_PORT_BASE GPIO_PORTN_BASE
#define LED_ANIM_PIN GPIO_PIN_1

//*****************************************************************************
//
// The system clock speed.
//
//*****************************************************************************
extern uint32_t g_ui32SysClock;


volatile unsigned long temperature = 0;



//*****************************************************************************
//
// Initialize the IO used in this demo
//
//*****************************************************************************
void
io_init(void)
{
    //
    // Configure Port N0 for as an output for the status LED.
    //
    MAP_GPIOPinTypeGPIOOutput(LED_PORT_BASE, LED_PIN);

    //
    // Configure Port N0 for as an output for the animation LED.
    //
    MAP_GPIOPinTypeGPIOOutput(LED_ANIM_PORT_BASE, LED_ANIM_PIN);

    //
    // Initialize LED to OFF (0)
    //
    MAP_GPIOPinWrite(LED_PORT_BASE, LED_PIN, 0);

    //
    // Initialize animation LED to OFF (0)
    //
    MAP_GPIOPinWrite(LED_ANIM_PORT_BASE, LED_ANIM_PIN, 0);

    //
    // Enable the peripherals used by this example.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);

    //
    // Configure the timer used to pace the animation.
    //
    MAP_TimerConfigure(TIMER2_BASE, TIMER_CFG_PERIODIC);

    //
    // Setup the interrupts for the timer timeouts.
    //
    MAP_IntEnable(INT_TIMER2A);
    MAP_TimerIntEnable(TIMER2_BASE, TIMER_TIMA_TIMEOUT);

    // Init ADC for temperature measurement
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
}

//*****************************************************************************
//
// Set the status LED on or off.
//
//*****************************************************************************
void
io_set_led(bool bOn)
{
    //
    // Turn the LED on or off as requested.
    //
    MAP_GPIOPinWrite(LED_PORT_BASE, LED_PIN, bOn ? LED_PIN : 0);
}

//*****************************************************************************
//
// Return LED state
//
//*****************************************************************************
void
io_get_ledstate(char * pcBuf, int iBufLen)
{
    //
    // Get the state of the LED
    //
    if(MAP_GPIOPinRead(LED_PORT_BASE, LED_PIN))
    {
        usnprintf(pcBuf, iBufLen, "ON");
    }
    else
    {
        usnprintf(pcBuf, iBufLen, "OFF");
    }

}

//*****************************************************************************
//
// Return LED state as an integer, 1 on, 0 off.
//
//*****************************************************************************
int
io_is_led_on(void)
{
    //
    // Get the state of the LED
    //
    if(MAP_GPIOPinRead(LED_PORT_BASE, LED_PIN))
    {
        return(true);
    }
    else
    {
        return(0);
    }
}


//*****************************************************************************
//
// Set the speed of the animation shown on the display.
//
//*****************************************************************************
void
measureTemperature(void)
{
    unsigned long ui32ADC0Value[1];

    ADCProcessorTrigger(ADC0_BASE, 3);
    while(!ADCIntStatus(ADC0_BASE, 3, false)){ }
    ADCIntClear(ADC0_BASE, 3);

    // Read ADC Value.
    ADCSequenceDataGet(ADC0_BASE, 3, &ui32ADC0Value[0]);


    temperature = 137.5 - ((75 * 3.3 * ui32ADC0Value[0]) / 4096);

}

//*****************************************************************************
//
// Get the current animation speed as an ASCII string.
//
//*****************************************************************************
void
getTemperature(char *pcBuf, int iBufLen)
{
    usnprintf(pcBuf, iBufLen, "%d °C", temperature);
}


