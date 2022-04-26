/*
 * sensor.c
 *
 *  Created on: 26.04.2022
 *      Author: jonas
 */
#include "sensor.h"
#include "McuRTOS.h"
#include "McuSHT31.h"
#include "i2cbus.h"
#include <stdint.h>
#include <stdio.h>

// variables
static float temp = 0.0f, humid = 0.0f;

// task that reads temperature and humidity once a second
static void sht31Task(void *pv)
{
	for(;;)
	{
		if(I2CBus_Lock() == false)
		{
			printf("Timeout when locking I2CBus!");
		}
		else
		{
			// locked successfully => read from bus
			if(McuSHT31_ReadTempHum(&temp, &humid) != ERR_OK){
				printf("Some error while reading temperature and humidity from SHT31.");
			}
			I2CBus_Unlock();
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void Sensor_Init(void){
	I2CBus_Init();

	BaseType_t res = xTaskCreate(	sht31Task,
									"sht31Task",
									600/sizeof(StackType_t),
									NULL,
									tskIDLE_PRIORITY,
									NULL);

	if(res != pdPASS) // task creation not successful?
	{
		printf("Some error while creating sensor task in Sensor_Init().");
		for(;;) {} // Endless loop
	}

	// vTaskStartScheduler is being called in main
}

void Sensor_Deinit(void){
	I2CBus_Deinit();
}


float Sensor_GetTemperature(void){
	return temp;
}

float Sensor_GetHumidity(void){
	return humid;
}
