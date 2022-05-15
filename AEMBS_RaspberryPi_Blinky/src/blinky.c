/*
 * blinky.c
 *
 * Author: Erich Styger
 * License: PDX-License-Identifier: BSD-3-Clause
 */

#include "blinky.h"
#include "delay.h"
#include <wiringPi.h>
#include <stdio.h>

/* AEMBS HAT V6: wiPi numbers below */
#define WPI_LED_BLUE    26
#define WPI_LED_GREEN   27
#define WPI_LED_RED     29
#define WPI_LED_YELLOW  28

#define WPI_BTN_UP      21
#define WPI_BTN_DOWN    22
#define WPI_BTN_RIGHT   23
#define WPI_BTN_LEFT    24
#define WPI_BTN_CENTER  25

void BLINKY_Blink(void) {
  printf("blinky green!\n");
  digitalWrite(WPI_LED_GREEN, HIGH); /* on */
  delay_ms(50);
  digitalWrite(WPI_LED_GREEN, LOW);  /* off */
  delay_ms(50);
}

void BLINKY_CheckButtons(void) {
  digitalWrite(WPI_LED_RED,      digitalRead(WPI_BTN_UP)==0?HIGH:LOW);
  digitalWrite(WPI_LED_GREEN,    digitalRead(WPI_BTN_DOWN)==0?HIGH:LOW);
  digitalWrite(WPI_LED_YELLOW,   digitalRead(WPI_BTN_LEFT)==0?HIGH:LOW);
  digitalWrite(WPI_LED_BLUE,     digitalRead(WPI_BTN_RIGHT)==0?HIGH:LOW);
  // all leds on when center is pressed
  if(digitalRead(WPI_BTN_CENTER)==0){
    digitalWrite(WPI_LED_RED, HIGH);
    digitalWrite(WPI_LED_GREEN, HIGH);
    digitalWrite(WPI_LED_YELLOW, HIGH);
    digitalWrite(WPI_LED_BLUE, HIGH);
  }
 }

void BLINKY_Init(void) {
  wiringPiSetup(); /* initialize WiringPi library */
  pinMode(WPI_LED_GREEN, OUTPUT);
  pinMode(WPI_LED_RED, OUTPUT);
  pinMode(WPI_LED_YELLOW, OUTPUT);
  pinMode(WPI_LED_BLUE, OUTPUT);

  pinMode(WPI_BTN_UP, INPUT);
  pinMode(WPI_BTN_DOWN, INPUT);
  pinMode(WPI_BTN_LEFT, INPUT);
  pinMode(WPI_BTN_RIGHT, INPUT);
  pinMode(WPI_BTN_CENTER, INPUT);
}
