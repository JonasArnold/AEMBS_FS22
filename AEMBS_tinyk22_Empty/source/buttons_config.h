/*
 * buttons_config.h
 *
 * Copyright (c) 2021, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#ifndef BUTTONS_CONFIG_H_
#define BUTTONS_CONFIG_H_

#include "McuLib.h"

#if McuLib_CONFIG_CPU_IS_KINETIS
  #define BUTTONS_PINS_HATNAVUP_GPIO      GPIOB
  #define BUTTONS_PINS_HATNAVUP_PORT      PORTB
  #define BUTTONS_PINS_HATNAVUP_PIN       3u

  #define BUTTONS_PINS_HATNAVDOWN_GPIO    GPIOB
  #define BUTTONS_PINS_HATNAVDOWN_PORT    PORTB
  #define BUTTONS_PINS_HATNAVDOWN_PIN     2u

  #define BUTTONS_PINS_HATNAVLEFT_GPIO    GPIOA
  #define BUTTONS_PINS_HATNAVLEFT_PORT    PORTA
  #define BUTTONS_PINS_HATNAVLEFT_PIN     13u

  #define BUTTONS_PINS_HATNAVRIGHT_GPIO   GPIOA
  #define BUTTONS_PINS_HATNAVRIGHT_PORT   PORTA
  #define BUTTONS_PINS_HATNAVRIGHT_PIN    12u

  #define BUTTONS_PINS_HATNAVCENTER_GPIO  GPIOB
  #define BUTTONS_PINS_HATNAVCENTER_PORT  PORTB
  #define BUTTONS_PINS_HATNAVCENTER_PIN   16u

  #define BUTTONS_ENABLE_CLOCK() CLOCK_EnableClock(kCLOCK_PortA); CLOCK_EnableClock(kCLOCK_PortB) /* enable clocking */
#elif McuLib_CONFIG_CPU_IS_LPC
   /* user button on LPC845-BRK board: PIO0_4 */
  #define BUTTONS_USER_GPIO       GPIO
  #define BUTTONS_USER_PORT       0
  #define BUTTONS_USER_PIN        4
  #define BUTTONS_USER_IOCON      IOCON_INDEX_PIO0_4

  #define BUTTONS_ENABLE_CLOCK() GPIO_PortInit(GPIO, 0) /* ungate the clocks for GPIO0: used for user button */
#endif

typedef enum {
#if McuLib_CONFIG_CPU_IS_KINETIS
  BTN_NAV_UP,
  BTN_NAV_DOWN,
  BTN_NAV_LEFT,
  BTN_NAV_RIGHT,
  BTN_NAV_CENTER,
#elif McuLib_CONFIG_CPU_IS_LPC
  BTN_USER,
#endif
  BTN_NOF_BUTTONS  /* sentinel, must be last in list! */
} BTN_Buttons_e;

/* bits of the buttons */
#if McuLib_CONFIG_CPU_IS_KINETIS
  #define BTN_BIT_NAV_UP        (1<<0)
  #define BTN_BIT_NAV_DOWN      (1<<1)
  #define BTN_BIT_NAV_LEFT      (1<<2)
  #define BTN_BIT_NAV_RIGHT     (1<<3)
  #define BTN_BIT_NAV_CENTER    (1<<4)
#elif McuLib_CONFIG_CPU_IS_LPC
  #define BTN_BIT_USER          (1<<0)
#endif

#endif /* BUTTONS_CONFIG_H_ */
