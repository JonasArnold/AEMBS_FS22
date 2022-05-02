/*
 * display.h
 *
 *  Created on: 02.05.2022
 *      Author: jonas
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/* module initialization */
void Display_Init(void);

/* module de-initialization */
void Display_Deinit(void);


void Display_UpdateView(void);
void Display_Clear(void);
void Display_DrawRectangle(void);
void Display_DrawCircle(void);
void Display_ShowSensorTemperature(float value);
void Display_ShowGameIntro(void);


#endif /* DISPLAY_H_ */
