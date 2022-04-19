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
#include "Invader.h"

static void OnDebounceEvent(McuDbnc_EventKinds event, uint32_t buttons);

#define TIMER_PERIOD_MS  20 /* frequency of debouncing timer */
#define EVENT_QUEUE_LENGTH		(5) /* number of elements in queue */
#define EVENT_QUEUE_ELEM_SIZE	(sizeof(Invader_event_e))  /* size of a queue item */

static QueueHandle_t eventQueue;  /* queue handle */
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

static void vQueueButtonEvent(Invader_event_e btn){
	if(xQueueSendToBack(eventQueue, &btn, pdMS_TO_TICKS(100)) != pdPASS){
		printf("Failed adding Button event to eventQueue. Maybe queue is full?");
	}
}

static void OnDebounceEvent(McuDbnc_EventKinds event, uint32_t buttons) {
  switch(event) {
    case MCUDBNC_EVENT_PRESSED:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("pressed, buttons %d:\n", buttons);
#endif
      if (buttons&BTN_BIT_NAV_LEFT) {
    	  vQueueButtonEvent(Invader_Button_Left_Pressed);
      }
	  if (buttons&BTN_BIT_NAV_RIGHT) {
		  vQueueButtonEvent(Invader_Button_Right_Pressed);
	  }
	  if (buttons&BTN_BIT_NAV_UP) {
		  vQueueButtonEvent(Invader_Button_Up_Pressed);
	  }
	  if (buttons&BTN_BIT_NAV_DOWN) {
		  vQueueButtonEvent(Invader_Button_Down_Pressed);
	  }
	  if (buttons&BTN_BIT_NAV_CENTER) {
		  vQueueButtonEvent(Invader_Button_Center_Pressed);
	  }
      break;
    case MCUDBNC_EVENT_PRESSED_REPEAT:
#if configUSE_SEGGER_SYSTEM_VIEWER_HOOKS
      SEGGER_SYSVIEW_PrintfTarget("pressed repeat, buttons %d:\n", buttons);
#endif
      if (buttons&BTN_BIT_NAV_LEFT) {
    	  vQueueButtonEvent(Invader_Button_LeftRepeat_Pressed);
      }
      if (buttons&BTN_BIT_NAV_RIGHT) {
    	  vQueueButtonEvent(Invader_Button_RightRepeat_Pressed);
      }
      if (buttons&BTN_BIT_NAV_UP) {
    	  vQueueButtonEvent(Invader_Button_UpRepeat_Pressed);
      }
      if (buttons&BTN_BIT_NAV_DOWN) {
    	  vQueueButtonEvent(Invader_Button_DownRepeat_Pressed);
      }
      if (buttons&BTN_BIT_NAV_CENTER) {
    	  vQueueButtonEvent(Invader_Button_CenterRepeat_Pressed);
      }
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

      if (buttons&BTN_BIT_NAV_LEFT) {
    	  vQueueButtonEvent(Invader_Button_Left_Released);
      }
      if (buttons&BTN_BIT_NAV_RIGHT) {
    	  vQueueButtonEvent(Invader_Button_Right_Released);
      }
      if (buttons&BTN_BIT_NAV_UP) {
    	  vQueueButtonEvent(Invader_Button_Up_Released);
      }
      if (buttons&BTN_BIT_NAV_DOWN) {
    	  vQueueButtonEvent(Invader_Button_Down_Released);
      }
      if (buttons&BTN_BIT_NAV_CENTER) {
    	  vQueueButtonEvent(Invader_Button_Center_Released);
      }

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

	// initialize debounce and timer
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

    // initialize queue
    eventQueue = xQueueCreate(EVENT_QUEUE_LENGTH, EVENT_QUEUE_ELEM_SIZE);
    if(eventQueue == NULL){
    	for(;;); /* out of memory? */
    }
    vQueueAddToRegistry(eventQueue, "eventQueue");
    Invader_RegisterEventQueueHandle(eventQueue);  // register event queue for the invader game
}
