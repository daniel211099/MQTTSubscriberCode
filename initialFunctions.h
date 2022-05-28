/*
 * initialFunctions.h
 *
 *  Created on: 10.05.2022
 */

#ifndef INITIALFUNCTIONS_H_
#define INITIALFUNCTIONS_H_


#include <stdlib.h> //For malloc
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>   // stack size for sprintf min. 2048 else FaultISR
#include <math.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include <driverlib/interrupt.h>
#include <driverlib/pin_map.h>
#include <driverlib/pwm.h>
#include <driverlib/sysctl.h>
#include <driverlib/timer.h>
#include <driverlib/i2c.h>
#include "CFAF128128B0145T/CFAF128128B0145T.h"



void initRGB(uint32_t ui32SysClock);    //Initialize the RGB-LEDs
void lightSingleMode(int lightcolor);   //set rgbLED 0:off 1:red 2:green 3:blue 4:yellow 5:purple 6:turquoise 7:white

void initBuzzer(uint32_t ui32SysClock); //Initialize the Buzzer

void buzzing(bool high);    //Plays an alarmsound through the buzzer and the red led is blinking
void endbuzz();

void ConfigureTimer1(uint32_t Clock);

void ConfigureControlGPIO();

#endif /* INITIALFUNCTIONS_H_ */
