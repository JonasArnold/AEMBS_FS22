/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "platform.h"
#include "buttons.h"
#include "buttons_config.h"
#include <assert.h>
#include "McuButton.h"
#include "McuRTOS.h"
#include "McuLED.h"
#include "leds.h"
#include "debounce.h"
#if McuLib_CONFIG_CPU_IS_KINETIS
  #include "fsl_port.h"
#elif McuLib_CONFIG_CPU_IS_LPC
  #include "fsl_pint.h"
  #include "fsl_syscon.h"
#endif
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
  #include "McuSystemView.h"
#endif

typedef struct BTN_Desc_t {
  McuBtn_Handle_t handle; /* handle of button pin */
} BTN_Desc_t;

static BTN_Desc_t BTN_Infos[BTN_NOF_BUTTONS];

bool BTN_IsPressed(BTN_Buttons_e btn) {
  assert(btn<BTN_NOF_BUTTONS);
  return McuBtn_IsOn(BTN_Infos[btn].handle);
}

uint32_t BTN_GetButtons(void) {
  uint32_t val = 0;

#if McuLib_CONFIG_CPU_IS_KINETIS
  if (BTN_IsPressed(BTN_NAV_UP)) {
    val |= BTN_BIT_NAV_UP;
  }
  if (BTN_IsPressed(BTN_NAV_DOWN)) {
    val |= BTN_BIT_NAV_DOWN;
  }
  if (BTN_IsPressed(BTN_NAV_LEFT)) {
    val |= BTN_BIT_NAV_LEFT;
  }
  if (BTN_IsPressed(BTN_NAV_RIGHT)) {
    val |= BTN_BIT_NAV_RIGHT;
  }
  if (BTN_IsPressed(BTN_NAV_CENTER)) {
    val |= BTN_BIT_NAV_CENTER;
  }
#elif McuLib_CONFIG_CPU_IS_LPC
  if (BTN_IsPressed(BTN_USER)) {
    val |= BTN_BIT_USER;
  }
#endif
  return val;
}

static void SysViewLogStart(void) {
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
  #define MCU_SYSTEM_VIEW_USER_ID_BUTTON_INTERRUPT (0)
  McuSystemView_RecordEnterISR();
  McuSystemView_Print((const char*)"Pressed button\r\n");
  McuSystemView_OnUserStart(MCU_SYSTEM_VIEW_USER_ID_BUTTON_INTERRUPT);
#endif
}

static void SysViewLogEnd(void) {
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
  McuSystemView_OnUserStop(MCU_SYSTEM_VIEW_USER_ID_BUTTON_INTERRUPT);
  McuSystemView_RecordExitISR();
#endif
}

#if McuLib_CONFIG_CPU_IS_KINETIS
void PORTA_IRQHandler(void) {
  uint32_t flags;
  uint32_t buttons = 0;
  BaseType_t xHigherPriorityTaskWoken = false;

  SysViewLogStart();
  flags = GPIO_PortGetInterruptFlags(GPIOA);
  if (flags&(1U<<BUTTONS_PINS_HATNAVLEFT_PIN)) {
    buttons |= BTN_BIT_NAV_LEFT;
  }
  if (flags&(1U<<BUTTONS_PINS_HATNAVRIGHT_PIN)) {
    buttons |= BTN_BIT_NAV_RIGHT;
  }
  GPIO_PortClearInterruptFlags(GPIOA, flags);
  Debounce_StartDebounceFromISR(buttons, &xHigherPriorityTaskWoken);
  SysViewLogEnd();
  if (xHigherPriorityTaskWoken) {
    taskYIELD();
  }
  __DSB();
}
#endif

#if McuLib_CONFIG_CPU_IS_KINETIS
void PORTB_IRQHandler(void) {
  uint32_t flags;
  uint32_t buttons = 0;
  BaseType_t xHigherPriorityTaskWoken = false;

  SysViewLogStart();
  flags = GPIO_PortGetInterruptFlags(GPIOB);
  if (flags&(1U<<BUTTONS_PINS_HATNAVCENTER_PIN)) {
    buttons |= BTN_BIT_NAV_CENTER;
  }
  if (flags&(1U<<BUTTONS_PINS_HATNAVDOWN_PIN)) {
    buttons |= BTN_BIT_NAV_DOWN;
  }
  if (flags&(1U<<BUTTONS_PINS_HATNAVUP_PIN)) {
    buttons |= BTN_BIT_NAV_UP;
  }
  GPIO_PortClearInterruptFlags(GPIOB, flags);
  Debounce_StartDebounceFromISR(buttons, &xHigherPriorityTaskWoken);
  SysViewLogEnd();
  if (xHigherPriorityTaskWoken) {
    taskYIELD();
  }
  __DSB(); /* need barrier due ARM Cortex bug */
}
#endif

#if McuLib_CONFIG_CPU_IS_LPC
static void pint_intr_callback(pint_pin_int_t pintr, uint32_t pmatch_status) {
  SysViewLogStart();
  /*! \todo handle interrupt */
  SysViewLogEnd();
}
#endif

void BTN_Deinit(void) {
#if McuLib_CONFIG_CPU_IS_KINETIS
  DisableIRQ(PORTA_IRQn);
  DisableIRQ(PORTB_IRQn);
#endif
  for(int i=0; i<BTN_NOF_BUTTONS; i++) {
    BTN_Infos[i].handle = McuBtn_DeinitButton(BTN_Infos[i].handle);
  }
}

void BTN_Init(void) {

  McuGPIO_Init();
  McuBtn_Init();

#if McuLib_CONFIG_CPU_IS_KINETIS
  McuBtn_Config_t btnConfig;

  BUTTONS_ENABLE_CLOCK();
  McuBtn_GetDefaultConfig(&btnConfig);
  btnConfig.isLowActive = true;

  btnConfig.hw.gpio = BUTTONS_PINS_HATNAVCENTER_GPIO;
  btnConfig.hw.port = BUTTONS_PINS_HATNAVCENTER_PORT;
  btnConfig.hw.pin = BUTTONS_PINS_HATNAVCENTER_PIN;
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  btnConfig.hw.pull = McuGPIO_PULL_UP;
#endif
  BTN_Infos[BTN_NAV_CENTER].handle = McuBtn_InitButton(&btnConfig);
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  McuBtn_EnablePullResistor(BTN_Infos[BTN_NAV_CENTER].handle); /* note: only required if using a McuLib prior 18-Mar-21 */
#endif

  btnConfig.hw.gpio = BUTTONS_PINS_HATNAVLEFT_GPIO;
  btnConfig.hw.port = BUTTONS_PINS_HATNAVLEFT_PORT;
  btnConfig.hw.pin = BUTTONS_PINS_HATNAVLEFT_PIN;
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  btnConfig.hw.pull = McuGPIO_PULL_UP;
#endif
  BTN_Infos[BTN_NAV_LEFT].handle = McuBtn_InitButton(&btnConfig);
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  McuBtn_EnablePullResistor(BTN_Infos[BTN_NAV_LEFT].handle); /* note: only required if using a McuLib prior 18-Mar-21 */
#endif

  btnConfig.hw.gpio = BUTTONS_PINS_HATNAVRIGHT_GPIO;
  btnConfig.hw.port = BUTTONS_PINS_HATNAVRIGHT_PORT;
  btnConfig.hw.pin = BUTTONS_PINS_HATNAVRIGHT_PIN;
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  btnConfig.hw.pull = McuGPIO_PULL_UP;
#endif
  BTN_Infos[BTN_NAV_RIGHT].handle = McuBtn_InitButton(&btnConfig);
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  McuBtn_EnablePullResistor(BTN_Infos[BTN_NAV_RIGHT].handle); /* note: only required if using a McuLib prior 18-Mar-21 */
#endif

  btnConfig.hw.gpio = BUTTONS_PINS_HATNAVUP_GPIO;
  btnConfig.hw.port = BUTTONS_PINS_HATNAVUP_PORT;
  btnConfig.hw.pin = BUTTONS_PINS_HATNAVUP_PIN;
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  btnConfig.hw.pull = McuGPIO_PULL_UP;
#endif
  BTN_Infos[BTN_NAV_UP].handle = McuBtn_InitButton(&btnConfig);
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  McuBtn_EnablePullResistor(BTN_Infos[BTN_NAV_UP].handle); /* note: only required if using a McuLib prior 18-Mar-21 */
#endif

  btnConfig.hw.gpio = BUTTONS_PINS_HATNAVDOWN_GPIO;
  btnConfig.hw.port = BUTTONS_PINS_HATNAVDOWN_PORT;
  btnConfig.hw.pin = BUTTONS_PINS_HATNAVDOWN_PIN;
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  btnConfig.hw.pull = McuGPIO_PULL_UP;
#endif
  BTN_Infos[BTN_NAV_DOWN].handle = McuBtn_InitButton(&btnConfig);
#if PL_CONFIG_BOARD_ID==PL_CONFIG_BOARD_ID_TINYK22_APROG_HAT_V7
  McuBtn_EnablePullResistor(BTN_Infos[BTN_NAV_DOWN].handle); /* note: only required if using a McuLib prior 18-Mar-21 */
#endif

  PORT_SetPinInterruptConfig(BUTTONS_PINS_HATNAVLEFT_PORT,  BUTTONS_PINS_HATNAVLEFT_PIN,  kPORT_InterruptFallingEdge);
  PORT_SetPinInterruptConfig(BUTTONS_PINS_HATNAVRIGHT_PORT, BUTTONS_PINS_HATNAVRIGHT_PIN, kPORT_InterruptFallingEdge);
  NVIC_SetPriority(PORTA_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
  EnableIRQ(PORTA_IRQn);

  PORT_SetPinInterruptConfig(BUTTONS_PINS_HATNAVDOWN_PORT, BUTTONS_PINS_HATNAVDOWN_PIN, kPORT_InterruptFallingEdge);
  PORT_SetPinInterruptConfig(BUTTONS_PINS_HATNAVUP_PORT,   BUTTONS_PINS_HATNAVUP_PIN,   kPORT_InterruptFallingEdge);
  PORT_SetPinInterruptConfig(BUTTONS_PINS_HATNAVCENTER_PORT, BUTTONS_PINS_HATNAVCENTER_PIN, kPORT_InterruptFallingEdge);
  NVIC_SetPriority(PORTB_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY);
  EnableIRQ(PORTB_IRQn);
#elif McuLib_CONFIG_CPU_IS_LPC
  /* user button on LPC845-BRK board: PIO0_4 */

  McuBtn_Config_t btnConfig;

  BUTTONS_ENABLE_CLOCK();
  McuBtn_GetDefaultConfig(&btnConfig);
  btnConfig.isLowActive = true;

  btnConfig.hw.gpio = BUTTONS_USER_GPIO;
  btnConfig.hw.port = BUTTONS_USER_PORT;
  btnConfig.hw.pin = BUTTONS_USER_PIN;
  btnConfig.hw.iocon = BUTTONS_USER_IOCON;
  BTN_Infos[BTN_USER].handle = McuBtn_InitButton(&btnConfig);

  SYSCON_AttachSignal(SYSCON, kPINT_PinInt0, kSYSCON_GpioPort0Pin4ToPintsel);

  PINT_Init(PINT); /* Initialize PINT */
  /* Setup Pin Interrupt 0 for rising edge */
  PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableFallEdge, pint_intr_callback);
  /* Enable callbacks for PINT0 by Index */
  PINT_EnableCallbackByIndex(PINT, kPINT_PinInt0);
#endif
}
