/*
 * InvaderGraphics.c
 *
 *  Created on: 02.05.2022
 *      Author: jonas
 */
#include "InvaderGraphics.h"
#include "display.h"
#include "McuSSD1306.h"
#include "McuGDisplaySSD1306.h"
#include "McuGFont.h"
#include "McuFontDisplay.h"
#include "McuUtility.h"
#include "McuFontHelv14Bold.h"
#include "McuFontHelv10Normal.h"
#include "images/ship.h"
#include "images/missile.h"
#include "sprite.h"
#include "McuRTOS.h"
#include <stdio.h>
#include <stdbool.h>

static SpriteImage_t *shipImage;
static Sprite_t *ship, *missiles;
static SemaphoreHandle_t missilesMutex = NULL;

/* module initialization */
void InvaderGraphics_Init(void){
	Display_Init();

	shipImage = Sprite_NewImage(Ship_GetImage());
	McuGDisplaySSD1306_PixelDim x = McuGDisplaySSD1306_GetWidth()/2;
	McuGDisplaySSD1306_PixelDim y = McuGDisplaySSD1306_GetHeight()-(shipImage->image->height);
	ship = Sprite_New(shipImage, x, y);

	missilesMutex = xSemaphoreCreateRecursiveMutex();
	vQueueAddToRegistry(missilesMutex, "Invader missiles mutex");
}

/* module de-initialization */
void InvaderGraphics_Deinit(void){
	Display_Deinit();

	vSemaphoreDelete(missilesMutex);
}

static bool MissilesMutex_Lock(void){
	/* aquire mutex */
	if(xSemaphoreTakeRecursive(missilesMutex, pdMS_TO_TICKS(20)) != pdTRUE){
		return false; /* timeout? */
	}
	return true;
}

void MissilesMutex_Unlock(void){
	/* give back mutex */
	if(xSemaphoreGiveRecursive(missilesMutex) != pdTRUE){
		/* issue */
		printf("Could not give back missiles mutex..");
		for(;;);
	}
}

static void CheckAndRemoveMissiles(void){
	Sprite_t *s, *next;

	if(MissilesMutex_Lock()){
		/* mutex aquired successfully */
		s = missiles;
		while(s != NULL){
			/* if hit the border */
			if(s->posY <= 0){
				next = s->next;  // remember following next element
				Sprite_DeleteFromList(&missiles, s); // remove element
				s = next;   // set iterator to current element
			} else {
				s = s->next;  // go to next element
			}
		}
		MissilesMutex_Unlock();
	}
}

void InvaderGraphics_UpdateView(void){
	McuGDisplaySSD1306_Clear();	/* blank screen */
	Sprite_Paint(ship);			/* update all objects */
	if(MissilesMutex_Lock()){
		Sprite_PaintList(missiles);
		MissilesMutex_Unlock();
	}
	CheckAndRemoveMissiles();
	Display_UpdateView();		/* transmit data to display */
}

void InvaderGraphics_MoveShip(Ship_MoveDirection_e direction){
	switch (direction) {
		case Ship_MoveDir_Left:
			Sprite_MoveLimited(ship, -5, 0, 0, McuGDisplaySSD1306_GetWidth(), 0, McuGDisplaySSD1306_GetHeight());
			break;
		case Ship_MoveDir_Right:
			Sprite_MoveLimited(ship, 5, 0, 0, McuGDisplaySSD1306_GetWidth(), 0, McuGDisplaySSD1306_GetHeight());
			break;
		case Ship_MoveDir_Unknown:
		default:
			break;
	}
}

void InvaderGraphics_ShootMissile(){
	SpriteImage_t *missileImage = Sprite_NewImage(Missile_GetImage());
	Sprite_t *missile = Sprite_New(	missileImage,
									ship->posX + ship->images->image->width/2 - missileImage->image->width/2,
									ship->posY - missileImage->image->height);
	missile->move.x = 0;
	missile->move.y = -1; //moving from bottom to top
	if(MissilesMutex_Lock()){
		Sprite_AddToList(&missiles, missile);
		MissilesMutex_Unlock();
	}
}

void InvaderGraphics_ShowIntro(void) {
	McuFontDisplay_PixelDim x, y, w_title, w_sub, h_title, h_sub;
	uint8_t title[] = "Invader";
	uint8_t subtitle[] = "Press button!";

	McuGDisplaySSD1306_Clear();
	w_title = McuFontDisplay_GetStringWidth(title, McuFontHelv14Bold_GetFont(), NULL);
	h_title = McuFontDisplay_GetStringHeight(title, McuFontHelv14Bold_GetFont(), NULL);
	w_sub = McuFontDisplay_GetStringWidth(subtitle, McuFontHelv10Normal_GetFont(), NULL);
	h_sub = McuFontDisplay_GetStringHeight(subtitle, McuFontHelv10Normal_GetFont(), NULL);
	// calc position for title and write
	x = (McuGDisplaySSD1306_GetWidth()-w_title)/2;
	y = (h_title/2);
	McuFontDisplay_WriteString(title, McuGDisplaySSD1306_COLOR_BLUE, &x, &y, McuFontHelv14Bold_GetFont());

	// calc position for subtitle and write
	x = (McuGDisplaySSD1306_GetWidth()-w_sub)/2;
	y += (h_title + h_sub/2);
	McuFontDisplay_WriteString(subtitle, McuGDisplaySSD1306_COLOR_BLUE, &x, &y, McuFontHelv10Normal_GetFont());

	McuGDisplaySSD1306_DrawBox(0, 0, McuGDisplaySSD1306_GetWidth()-1,
	McuGDisplaySSD1306_GetHeight()-1, 1, McuGDisplaySSD1306_COLOR_WHITE);
	McuGDisplaySSD1306_DrawBox(2, 2, McuGDisplaySSD1306_GetWidth()-5,
	McuGDisplaySSD1306_GetHeight()-5, 1, McuGDisplaySSD1306_COLOR_WHITE);

	Display_UpdateView();
}

void InvaderGraphics_Clear(void){
	Display_Clear();
}


