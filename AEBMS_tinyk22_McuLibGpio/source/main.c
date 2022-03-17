/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    AEBMS_tinyk22_First.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MK22F51212.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "McuGPIO.h"
#include "leds.h"
#include <stdbool.h>

/* TODO: insert other definitions and declarations here. */
static McuGPIO_Handle_t myPin;

/*
 * @brief   Application entry point.
 */
int main(void) {

    /* Init board hardware. */
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

    // configure gpio
	#if McuLib_CONFIG_CPU_IS_KINETIS
	  CLOCK_EnableClock(kCLOCK_PortB);
	#elif McuLib_CONFIG_CPU_IS_LPC
	  GPIO_PortInit(GPIO, 0);
	#else
	  #error "unknown CPU"
	#endif

    // configure mcu gpio
    McuGPIO_Init();
    McuGPIO_Config_t config;
    McuGPIO_GetDefaultConfig(&config);
    config.hw.gpio = GPIOB;
    config.hw.port = PORTB;
    config.hw.pin = 16;
    config.isHighOnInit = true;
    config.isInput = true;

    // configure led
    LEDS_Init();

    myPin = McuGPIO_InitGPIO(&config);
    if(myPin == NULL){
    	PRINTF("Error initializing gpio.\n");
    	for(;;) { /* error */ }
    }

    PRINTF("Program Started\n");


    for(;;){
    	 bool gpioLow = McuGPIO_IsLow(myPin);
    	 if(gpioLow)
    	 {
    		 LEDS_On(LEDS_RED);
    	 }
    	 else
    	 {
    		 LEDS_Off(LEDS_RED);
    	 }
    }

    return 0 ;
}
