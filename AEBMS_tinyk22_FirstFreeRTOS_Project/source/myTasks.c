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
#include "sensor.h"

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

static void sensorPrintTask(void *pv)
{
	for(;;)
	{
		// read values
		float temp = Sensor_GetTemperature();
		float humid = Sensor_GetHumidity();

		// print
		printf("Read from Sensor: Temperature %d, Humidity %d\n", (int)(temp*10), (int)(humid*10));

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
//
	res = xTaskCreate(	sensorPrintTask,
						"sensorPrintTask",
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
