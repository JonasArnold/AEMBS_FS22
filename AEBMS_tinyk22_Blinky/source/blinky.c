/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform.h"
#include "blinky.h"
#include "fsl_gpio.h"
#if PL_CONFIG_IS_TINY
  #include "fsl_port.h"
#endif

#if PL_CONFIG_IS_LPC
  /* LED_BLUE: PIO1_1 */
  #define BOARD_LED_BLUE_GPIO             GPIO
  #define BOARD_LED_BLUE_GPIO_PORT        1U
  #define BOARD_LED_BLUE_GPIO_PIN         1U
  #define BOARD_LED_BLUE_GPIO_PIN_MASK    (1U << BOARD_LED_BLUE_GPIO_PIN)
#elif PL_CONFIG_IS_TINY
  /* LED_BLUE: PTC2 */
  #define BOARD_LED_BLUE_GPIO             GPIOC
  #define BOARD_LED_BLUE_PORT             PORTC
  #define BOARD_LED_BLUE_GPIO_PIN         2U
  #define BOARD_LED_BLUE_GPIO_PIN_MASK    (1U << BOARD_LED_BLUE_GPIO_PIN)
#endif

static void delay(void) {
  uint32_t i;

  for(i=0;i<1000000;i++) {
    __asm volatile ("nop");
  }
}

void BLINKY_Init(void) {
#if PL_CONFIG_IS_LPC
  gpio_pin_config_t config;

  GPIO_PortInit(GPIO, 1); /* ungate the clocks for port 1 (PIO1_0, PIO1_1 and PIO1_2): used LED */

  config.outputLogic = 1; /* low active */
  config.pinDirection = kGPIO_DigitalOutput;
  GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PORT, BOARD_LED_BLUE_GPIO_PIN, &config);
#elif PL_CONFIG_IS_TINY
  CLOCK_EnableClock(kCLOCK_PortC);

  gpio_pin_config_t LED_BLUE_config = {
        .pinDirection = kGPIO_DigitalOutput,
        .outputLogic = 1U
  };
  /* Initialize GPIO functionality on pin PTC2 (pin 45)  */
  GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN, &LED_BLUE_config);

  /* PORTC2 (pin 45) is configured as PTC2 */
  PORT_SetPinMux(BOARD_LED_BLUE_PORT, BOARD_LED_BLUE_GPIO_PIN, kPORT_MuxAsGpio);
#endif
}

void BLINKY_Run(void) {
  for(;;) {
  #if PL_CONFIG_IS_LPC
    GPIO_PortToggle(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PORT, BOARD_LED_BLUE_GPIO_PIN_MASK);
  #elif PL_CONFIG_IS_TINY
		GPIO_PortToggle(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PIN_MASK);
  #endif
    delay();
  }
}
