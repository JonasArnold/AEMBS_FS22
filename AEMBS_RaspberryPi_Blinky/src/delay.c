/*
 * delay.c
 *
 * Author: Erich Styger
 * License: PDX-License-Identifier: BSD-3-Clause
 */

#include "delay.h"
#include <unistd.h> /* interface to sleep */
#include <wiringPi.h>

void delay_ms(unsigned int ms) {
  //sleep(1); /* sleep for 1 second */
  delay(ms); /* http://wiringpi.com/reference/timing/ */
#if 0
  volatile long i;
  for(i=0; i<50000000; i++) {
	  /* burning time */
  }
#endif
}

