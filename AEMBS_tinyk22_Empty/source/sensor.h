/*
 * sensor.h
 *
 *  Created on: 26.04.2022
 *      Author: jonas
 */

#ifndef SENSOR_H_
#define SENSOR_H_

/* module initialization */
void Sensor_Init(void);

/* module deinitialization */
void Sensor_Deinit(void);


/* reads temperature and returns it a s float */
float Sensor_GetTemperature(void);

/* reads humididy and returns it a s float */
float Sensor_GetHumidity(void);

#endif /* SENSOR_H_ */
