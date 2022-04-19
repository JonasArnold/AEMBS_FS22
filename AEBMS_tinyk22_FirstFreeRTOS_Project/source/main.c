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
#include "fsl_clock.h"
/* TODO: insert other include files here. */
#include "leds.h"
#include "myTasks.h"
#include "myLedTask.h"
#include "McuRTOS.h"
#include "mySystemView.h"
#include "buttons.h"
#include "debounce.h"
#include "Invader.h"
#include "McuI2cLib.h"
#include "McuGenericI2C.h"
#include "McuSHT31.h"

/* TODO: insert other definitions and declarations here. */

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

    printf("Hello World\n");
    LEDS_Init();

    //MyLedTask_Init(); /* creates led task */

    printf("Tasks created\n");

    printf("System Viewer initialization\n");
    mySystemView_Init();

    printf("Mcu Debounce initialization\n");
    McuRTOS_Init();
    BTN_Init();
    Debounce_Init();

    printf("Invader initialization\n");
    Invader_Init();

    printf("I2C initialization\n");
    McuGenericI2C_Init();  // initialize generic I2C module
    CLOCK_EnableClock(kCLOCK_PortB);  // I2C pins on Port B, needed for ResetBus()
    McuI2cLib_Init();

    printf("SHT31 initialization\n");
    McuSHT31_Init();
    MyTasks_Init();   /* creates sht31 task*/

    vTaskStartScheduler();
    /* end => nothing else is called from here */

    return 0 ;
}
