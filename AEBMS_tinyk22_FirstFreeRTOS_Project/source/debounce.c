/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <stdio.h>
#include "debounce.h"
#include "McuDebounce.h"
#include "McuRTOS.h"
#include "buttons.h"
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
  #include "McuSystemView.h"
#endif

static void OnDebounceEvent(McuDbnc_EventKinds event, uint32_t buttons);

#define TIMER_PERIOD_MS  20 /* frequency of debouncing timer */

static McuDbnc_Desc_t data =
{
  .state = MCUDBMC_STATE_IDLE,  /* state of state machine */
  .timerPeriodMs = TIMER_PERIOD_MS, /* timer period for debouncing */
  .timer = NULL, /* FreeRTOS timer handle */
  .debounceTimeMs = 100, /* debouncing time */
  .repeatTimeMs   = 200, /* time for repeated button events */
  .longKeyTimeMs  = 1000, /* time for a long key press */
  .getButtons = BTN_GetButtons, /* callback to get bitset of buttons */
  .onDebounceEvent = OnDebounceEvent, /* debounce event handler */
};

static void OnDebounceEvent(McuDbnc_EventKinds event, uint32_t buttons) {
  switch(event) {
    case MCUDBNC_EVENT_PRESSED:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("pressed, buttons %d:\n", buttons);
#endif
      break;
    case MCUDBNC_EVENT_PRESSED_REPEAT:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("pressed repeat, buttons %d:\n", buttons);
#endif
     break;

    case MCUDBNC_EVENT_LONG_PRESSED:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("pressed long, buttons %d:\n", buttons);
#endif
      break;
    case MCUDBNC_EVENT_LONG_PRESSED_REPEAT:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("pressed long repeat, buttons %d:\n", buttons);
#endif
      break;

    case MCUDBNC_EVENT_RELEASED:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("released, buttons %d:\n", buttons);

#define MCU_SYSTEM_VIEW_USER_ID_PRINTF (10)

      McuSystemView_OnUserStart(MCU_SYSTEM_VIEW_USER_ID_PRINTF);
      printf("Test");
      McuSystemView_OnUserStop(MCU_SYSTEM_VIEW_USER_ID_PRINTF);

#endif
      break;
    case MCUDBNC_EVENT_LONG_RELEASED:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("long released, buttons %d:\n", buttons);
#endif
      break;

    case MCUDBNC_EVENT_END:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("stopped debouncing\n");
#endif
      (void)xTimerStop(data.timer, pdMS_TO_TICKS(100)); /* stop timer */
      break;

    default:
      break;
  }
}

static void vTimerCallbackDebounce(TimerHandle_t pxTimer) {
  /* called with TIMER_PERIOD_MS during debouncing */
  McuDbnc_Process(&data);
}

void Debounce_StartDebounce(uint32_t buttons) {
  if (data.state==MCUDBMC_STATE_IDLE) {
    data.scanValue = buttons;
    data.state = MCUDBMC_STATE_START;
    McuDbnc_Process(&data);
    (void)xTimerStart(data.timer, pdMS_TO_TICKS(100));
  }
}

void Debounce_StartDebounceFromISR(uint32_t buttons, BaseType_t *pxHigherPriorityTaskWoken) {
  if (data.state==MCUDBMC_STATE_IDLE) {
    data.scanValue = buttons;
    data.state = MCUDBMC_STATE_START;
    McuDbnc_Process(&data);
    (void)xTimerStartFromISR(data.timer, pxHigherPriorityTaskWoken);
  }
}

void Debounce_Deinit(void) {
  /* nothing needed */
}

void Debounce_Init(void) {
  McuDbnc_Init();
  data.timer = xTimerCreate(
        "tmrDbnc", /* name */
        pdMS_TO_TICKS(TIMER_PERIOD_MS), /* period/time */
        pdTRUE, /* auto reload */
        (void*)0, /* timer ID */
        vTimerCallbackDebounce); /* callback */
  if (data.timer==NULL) {
    for(;;); /* failure! */
  }
}
