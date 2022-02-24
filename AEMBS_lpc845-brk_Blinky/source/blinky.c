/*
 * blinky.c
 *
 *  Created on: 24.02.2022
 *      Author: jonas
 */
#include "blinky.h"

#include "fsl_gpio.h"

/* LED_BLUE: PIO1_1 */
#ifndef BOARD_LED_BLUE_GPIO
  #define BOARD_LED_BLUE_GPIO GPIO
#endif
#ifndef BOARD_LED_BLUE_GPIO_PORT
  #define BOARD_LED_BLUE_GPIO_PORT 1U
#endif
#ifndef BOARD_LED_BLUE_GPIO_PIN
  #define BOARD_LED_BLUE_GPIO_PIN 1U
#endif

static void delay(void) {
	uint32_t i;

	for(i=0;i<5000000;i++) {
		__asm volatile ("nop");
	}
}

static void InitLed(void) {
  gpio_pin_config_t config;

  GPIO_PortInit(GPIO, 1); /* ungate the clocks for port 1 (PIO1_0, PIO1_1 and PIO1_2): used LED */

  config.outputLogic = 1; /* low active */
  config.pinDirection = kGPIO_DigitalOutput;
  GPIO_PinInit(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PORT, BOARD_LED_BLUE_GPIO_PIN, &config);
}


void BLINKY_Run(void)
{
	InitLed();
	for (;;)
	{
		GPIO_PortToggle(BOARD_LED_BLUE_GPIO, BOARD_LED_BLUE_GPIO_PORT, 1<<BOARD_LED_BLUE_GPIO_PIN);
		delay();
	}
}

