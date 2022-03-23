/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "blinky.h"
#include "fsl_gpio.h"
#include "McuWait.h"

/* LED_BLUE: PTC2 */
#define BOARD_LED_BLUE_GPIO             GPIOC
#define BOARD_LED_BLUE_PORT             PORTC
#define BOARD_LED_BLUE_GPIO_PIN         2U
#define BOARD_LED_BLUE_GPIO_PIN_MASK    (1U << BOARD_LED_BLUE_GPIO_PIN)

void BLINKY_Init(void) {
	McuLib_Init();
	McuWait_Init();
}

void BLINKY_Deinit(void) {
	McuWait_Deinit();
	McuLib_Deinit();
}

void BLINKY_Run(void) {
  for(;;) {
	  // toggle port
	  GPIO_PortToggle(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN_MASK);
	  // use mcu wait
	  McuWait_Waitms(1000);
  }
}
