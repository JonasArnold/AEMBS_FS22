/*
 * myTasks.c
 *
 *  Created on: 23.03.2022
 *      Author: jonas
 */
#include "myTasks.h"
#include "McuRTOS.h"
#include "McuSHT31.h"
#include <stdint.h>
#include <stdio.h>

//static uint32_t taskParameter1 = 5;
//static uint32_t taskParameter2 = 10;
//static TaskHandle_t myTaskHandle = NULL;

//static void myTask(void *pv)
//{
//	uint32_t param = *(uint32_t*)pv;
//	printf("myTask param is %d\n", (int)param);
//	for(;;)
//	{
//		taskDISABLE_INTERRUPTS();
//		__asm volatile("nop");
//		taskENABLE_INTERRUPTS();
//		vTaskDelay(pdMS_TO_TICKS(100));
//	}
//}

static void sht31Task(void *pv)
{
	float temp = 0.0f, humid = 0.0f;
	for(;;)
	{
		if(McuSHT31_ReadTempHum(&temp, &humid) != ERR_OK){
			printf("Some error while reading temperature and humidity from SHT31.");
		}

		// does not work !! float takes very much stack space
//		printf("Read from SHT31: Temperature %.2f, Humidity %.2f\n", temp, humid);

		// Solution with McuUtility (untested)
//		char tempString[10], humidString[10];
//		McuUtility_NumFloatToStr(tempString, sizeof(tempString), temp, 2);
//		McuUtility_NumFloatToStr(humidString, sizeof(humidString), humid, 2);
//		printf("Read from SHT31: Temperature %.2f, Humidity %.2f\n", temp, humid);

		// simplest solution
		printf("Read from SHT31: Temperature %d, Humidity %d\n", (int)(temp*10), (int)(humid*10));

		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}


void MyTasks_Init(void)
{
	BaseType_t res;
//	res = xTaskCreate(	myTask,
//						"myTask1",
//						400/sizeof(StackType_t),
//						&taskParameter1,
//						tskIDLE_PRIORITY,
//						&myTaskHandle);
//
//	if(res != pdPASS) // task creation not successful?
//	{	// TODO error handling
//		for(;;) {} // Endless loop
//	}
//
//	res = xTaskCreate(	myTask,
//						"myTask2",
//						400/sizeof(StackType_t),
//						&taskParameter2,
//						tskIDLE_PRIORITY,
//						NULL);
//
//	if(res != pdPASS) // task creation not successful?
//	{	// TODO error handling
//		for(;;) {} // Endless loop
//	}

	res = xTaskCreate(	sht31Task,
						"sht31Task",
						600/sizeof(StackType_t),
						NULL,
						tskIDLE_PRIORITY,
						NULL);

	if(res != pdPASS) // task creation not successful?
	{	// TODO error handling
		for(;;) {} // Endless loop
	}

	// vTaskStartScheduler is being called in main
}
