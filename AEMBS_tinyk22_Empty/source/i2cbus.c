/*
 * i2cbus.c
 *
 *  Created on: 26.04.2022
 *      Author: jonas
 */
#include "i2cbus.h"
#include "McuRTOS.h"
#include <stdbool.h>
#include <stdio.h>

static SemaphoreHandle_t mutex = NULL; /* mutex to protect access to bus */

void I2CBus_Init(void){
	// create mutex
	mutex = xSemaphoreCreateRecursiveMutex();
	vQueueAddToRegistry(mutex, "I2C mutex");
}

void I2CBus_Deinit(void){
	vSemaphoreDelete(mutex);
}

bool I2CBus_Lock(void){
	if(xSemaphoreTakeRecursive(mutex, pdMS_TO_TICKS(20)) != pdTRUE){
		/* timeout? */
		return false;
	}
	return true;
}

void I2CBus_Unlock(void){
	if(xSemaphoreGiveRecursive(mutex) != pdTRUE){
		/* issue */
		printf("Could not give back I2CBus mutex semaphore..");
		for(;;);
	}
}
