/*
 * myLedTask.c
 *
 *  Created on: 24.03.2022
 *      Author: jonas
 */
#include "myLedTask.h"
#include "McuRTOS.h"
#include <stdint.h>
#include "leds.h"
#include "McuSystemView.h"

LEDS_Leds_e chosenLed = LEDS_RED;

static void ledTask(void *pv)
{
	vTaskDelay(1);
	vTaskPrioritySet(NULL,uxTaskPriorityGet(NULL)+2);  // increase priority by two
	TickType_t xLastWakeTime = xTaskGetTickCount();
	LEDS_Leds_e led = *(LEDS_Leds_e*)pv;
	for(;;)
	{

		#define MCU_SYSTEM_VIEW_USER_ID_LED_BLINK 11
	    McuSystemView_OnUserStart(MCU_SYSTEM_VIEW_USER_ID_LED_BLINK);
		LEDS_Neg(led);
	    McuSystemView_OnUserStop(MCU_SYSTEM_VIEW_USER_ID_LED_BLINK);

		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
	}
}

void MyLedTask_Init(void)
{
    // configure leds
    LEDS_Init();

	BaseType_t res;
	res = xTaskCreate(	ledTask,
						"ledTask",
						400/sizeof(StackType_t),
						&chosenLed,
						tskIDLE_PRIORITY+1,
						NULL);

	if(res != pdPASS) // task creation not successful?
	{	// TODO error handling
		for(;;) {} // Endless loop
	}

	// vTaskStartScheduler is being called in main
}

