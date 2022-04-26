/*
 * i2cbus.h
 *
 *  Created on: 26.04.2022
 *      Author: jonas
 */

#ifndef I2CBUS_H_
#define I2CBUS_H_

#include <stdbool.h>

/* module initialization */
void I2CBus_Init(void);

/* module deinitialization */
void I2CBus_Deinit(void);


/* request mutex
 * return true 	 => mutex was aquired successfully
 * returns false => timeout or other error*/
bool I2CBus_Lock(void);

/* give back mutex */
void I2CBus_Unlock(void);

#endif /* I2CBUS_H_ */
