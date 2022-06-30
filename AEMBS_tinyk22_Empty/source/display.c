/*
 * display.c
 *
 *  Created on: 02.05.2022
 *      Author: jonas
 */
#include "display.h"
#include "McuSSD1306.h"
#include "McuGDisplaySSD1306.h"
#include "McuGFont.h"
#include "McuFontDisplay.h"
#include "McuUtility.h"
#include "McuFontHelv14Bold.h"
#include "McuFontHelv10Normal.h"
#include "McuFontHelv18Bold.h"
#include "i2cbus.h"
#include "images/smiley.h"
#include <stdio.h>

void Display_Init(void){
	McuSSD1306_Init();
	McuGDisplaySSD1306_Init();
	McuGFont_Init();
	McuFontDisplay_Init();
}


void Display_Deinit(void){
	McuSSD1306_Deinit();
	McuGDisplaySSD1306_Deinit();
	McuGFont_Deinit();
	McuFontDisplay_Deinit();
}

void Display_UpdateView(void){
	if(I2CBus_Lock() == false){
		printf("Timeout when locking I2CBus for Display!\n");
	}
	else{  // locked successfully
		McuGDisplaySSD1306_UpdateFull();
		I2CBus_Unlock();
	}
}

void Display_Clear(void){
	McuGDisplaySSD1306_Clear();
	Display_UpdateView();
}

void Display_DrawRectangle(void){
	McuGDisplaySSD1306_Clear();
	McuGDisplaySSD1306_DrawBox(0, 0, McuGDisplaySSD1306_GetWidth()-1, McuGDisplaySSD1306_GetHeight()-1, 1, McuGDisplaySSD1306_COLOR_WHITE);
	Display_UpdateView();
}

void Display_DrawSmiley(void){
	McuGDisplaySSD1306_Clear();
	PIMAGE smiley = Smiley_GetImage();
    McuGDisplaySSD1306_DrawMonoBitmap(0, 0, smiley, McuGDisplaySSD1306_COLOR_WHITE, McuGDisplaySSD1306_COLOR_BLACK);
	Display_UpdateView();
}

void Display_DrawCircle(void){
	McuGDisplaySSD1306_Clear();
	McuGDisplaySSD1306_DrawCircle(20 /* x */, 30 /* y */, 5 /* radius */, McuGDisplaySSD1306_COLOR_WHITE);
	Display_UpdateView();
}

void Display_ShowSensorTemperature(float value) {
	McuFontDisplay_PixelDim x, y, w, h;
	uint8_t buf[24];

	McuGDisplaySSD1306_Clear();
	McuUtility_NumFloatToStr(buf, sizeof(buf), value, 2);
	McuUtility_chcat(buf, sizeof(buf), 'C');
	w = McuFontDisplay_GetStringWidth(buf, McuFontHelv18Bold_GetFont(), NULL);
	h = McuFontDisplay_GetStringHeight(buf, McuFontHelv18Bold_GetFont(), NULL);
	x = (McuGDisplaySSD1306_GetWidth()-w)/2;
	y = (McuGDisplaySSD1306_GetHeight()-h)/2;
	McuFontDisplay_WriteString(buf, McuGDisplaySSD1306_COLOR_BLUE, &x, &y,
	McuFontHelv18Bold_GetFont());

	McuGDisplaySSD1306_DrawBox(0, 0, McuGDisplaySSD1306_GetWidth()-1,
	McuGDisplaySSD1306_GetHeight()-1, 1, McuGDisplaySSD1306_COLOR_WHITE);
	McuGDisplaySSD1306_DrawBox(2, 2, McuGDisplaySSD1306_GetWidth()-5,
	McuGDisplaySSD1306_GetHeight()-5, 1, McuGDisplaySSD1306_COLOR_WHITE);

	Display_UpdateView();
}


