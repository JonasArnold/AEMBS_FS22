/*
 * Invader.c
 *
 *  Created on: 04.04.2022
 *      Author: jonas
 */
#include "Invader.h"
#include "McuRTOS.h"
#include "leds.h"
#include <stdio.h>
#include "debounce.h"

static void gameTask(void *pv);
static TaskHandle_t gameTaskHandle = NULL;
static QueueHandle_t eventQueueHandle = NULL;

void Invader_Init()
{
	/* initialize */
	BaseType_t res;
	res = xTaskCreate(	gameTask,
						"gameTask",
						600/sizeof(StackType_t),
						NULL,
						tskIDLE_PRIORITY+1,
						&gameTaskHandle);

	if(res != pdPASS) // task creation not successful?
	{
		printf("Creating invader task failed");
		for(;;) {} // Endless loop
	}

	// get the event queue handle from debounce
	eventQueueHandle = Debounce_GetEventQueueHandle();
}

void Invader_Deinit()
{
	/* deinitialize */
}

/*!
 * \brief Task safe way to get invader event
 * \return Event
 */
static Invader_event_e Invader_RecieveEvent(void)
{
	Invader_event_e event = Invader_Event_None;

	// get event data from queue if it can be reached
	if(eventQueueHandle != NULL)
	{
		// ignore errors, since it is very well possible that the queue is empty
		xQueueReceive(eventQueueHandle, &event, pdMS_TO_TICKS(20));
	}
	return event;
}

/*!
 * \brief Game task
 * \param pv pointer
 */
static void gameTask(void *pv)
{
	Invader_event_e event;

	for(;;) {
		event = Invader_RecieveEvent();
		if (event!=Invader_Event_None) {
			printf("Invader Game: Recieved event: %d\n", event);

			switch(event) {
				case Invader_Button_Left_Pressed: LEDS_On(LEDS_RED); break;
				case Invader_Button_LeftRepeat_Pressed: break;
				case Invader_Button_Left_Released: LEDS_Off(LEDS_RED); break;

				case Invader_Button_Right_Pressed: LEDS_On(LEDS_GREEN); break;
				case Invader_Button_RightRepeat_Pressed: break;
				case Invader_Button_Right_Released: LEDS_Off(LEDS_GREEN); break;

				case Invader_Button_Up_Pressed: LEDS_On(LEDS_ORANGE); break;
				case Invader_Button_UpRepeat_Pressed: break;
				case Invader_Button_Up_Released: LEDS_Off(LEDS_ORANGE); break;

				case Invader_Button_Down_Pressed: LEDS_On(LEDS_TINY_BLUE); break;
				case Invader_Button_DownRepeat_Pressed: break;
				case Invader_Button_Down_Released: LEDS_Off(LEDS_TINY_BLUE); break;

				case Invader_Button_Center_Pressed: LEDS_On(LEDS_BLUE); break;
				case Invader_Button_CenterRepeat_Pressed: break;
				case Invader_Button_Center_Released: LEDS_Off(LEDS_BLUE); break;
				default: break;
			}
		}
		else
		{
			vTaskDelay(pdMS_TO_TICKS(2));
		}
	}
}


