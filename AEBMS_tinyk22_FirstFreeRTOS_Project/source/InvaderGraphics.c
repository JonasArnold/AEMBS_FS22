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
#include "images/alien0.h"
#include "sprite.h"
#include "McuRTOS.h"
#include "sensor.h"
#include <stdio.h>
#include <stdbool.h>

static SpriteImage_t *shipImage;
static Sprite_t *ship, *missiles, *aliens;
static SemaphoreHandle_t missilesMutex = NULL, aliensMutex = NULL;

// declarations
static void DrawAliens(uint8_t rows);
static bool MissilesMutex_Lock(void);
static void MissilesMutex_Unlock(void);
static bool AliensMutex_Lock(uint16_t waitTimeMS);
static void AliensMutex_Unlock(void);
static bool CheckAndRemoveCollidingObjects(void);

/* module initialization */
void InvaderGraphics_Init(void){
	Display_Init();

	// create mutex for missile and alien lists
	missilesMutex = xSemaphoreCreateRecursiveMutex();
	vQueueAddToRegistry(missilesMutex, "Invader missiles mutex");
	aliensMutex = xSemaphoreCreateRecursiveMutex();
	vQueueAddToRegistry(aliensMutex, "Invader aliens mutex");
}

/* module de-initialization */
void InvaderGraphics_Deinit(void){
	Display_Deinit();

	vSemaphoreDelete(missilesMutex);
	vSemaphoreDelete(aliensMutex);
}

void InvaderGraphics_DrawNewGame(void){
	// draw initial objects
	shipImage = Sprite_NewImage(Ship_GetImage());
	McuGDisplaySSD1306_PixelDim x = McuGDisplaySSD1306_GetWidth()/2;
	McuGDisplaySSD1306_PixelDim y = McuGDisplaySSD1306_GetHeight()-(shipImage->image->height);
	ship = Sprite_New(shipImage, x, y);
	DrawAliens(1);
}


static void CheckAndRemoveMissilesOutOfBorder(void){
	Sprite_t *s, *next;

	if(MissilesMutex_Lock()){
		/* mutex aquired successfully */
		s = missiles;
		while(s != NULL){
			/* if hit the border top or bottom */
			if(s->posY <= 0 || (s->posY + s->images->image->height) >= McuGDisplaySSD1306_GetHeight()){
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

UpdateView_Result_e InvaderGraphics_UpdateView(void){
	UpdateView_Result_e result = UpdateView_Result_Normal;
	McuGDisplaySSD1306_Clear();	/* blank screen */
	Sprite_Paint(ship);			/* update all objects */
	if(MissilesMutex_Lock()){
		Sprite_PaintList(missiles);
		MissilesMutex_Unlock();
	}
	if(AliensMutex_Lock(20)){
		Sprite_PaintList(aliens);
		AliensMutex_Unlock();
	}
	CheckAndRemoveMissilesOutOfBorder();  // removes missiles that are getting out of the border
	if(CheckAndRemoveCollidingObjects()){ // removes aliens or the ship if they collide with a missile
		result = UpdateView_Result_CollisionWithShip;
	}
	if(Sprite_NofInList(aliens) == 0){    // no more aliens (higher priority than collision)
		result = UpdateView_Result_NoMoreAliens;
	}
	Display_UpdateView();		/* transmit data to display */
	return result;
}

void InvaderGraphics_MoveShip(Ship_MoveDirection_e direction){
	switch (direction) {
		case Ship_MoveDir_Left:
			Sprite_MoveLimited(ship, -5, 0, 0, McuGDisplaySSD1306_GetWidth(), 0, McuGDisplaySSD1306_GetHeight());
			break;
		case Ship_MoveDir_Right:
			Sprite_MoveLimited(ship, 5, 0, 0, McuGDisplaySSD1306_GetWidth(), 0, McuGDisplaySSD1306_GetHeight());
			break;
		case Ship_MoveDir_Down:
			Sprite_MoveLimited(ship, 0, 5, 0, McuGDisplaySSD1306_GetWidth(), 0, McuGDisplaySSD1306_GetHeight());
			break;
		case Ship_MoveDir_Up:
			Sprite_MoveLimited(ship, 0, -5, 0, McuGDisplaySSD1306_GetWidth(), 0, McuGDisplaySSD1306_GetHeight());
			break;
		case Ship_MoveDir_Unknown:
		default:
			break;
	}
}

void InvaderGraphics_ShootMissile(void){
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

void InvaderGraphics_GenerateAlienMissile(void){
	McuUtility_randomSetSeed((uint)(Sensor_GetTemperature()*100));
	unsigned int NumOfAliens = Sprite_NofInList(aliens);
	if(NumOfAliens > 0){ // if there are any aliens left
		int32_t alienNum = McuUtility_random(0, NumOfAliens-1);  // -1 because iterating from 0
		Sprite_t *randomAlien = Sprite_IndexInList(aliens, alienNum);
		SpriteImage_t *missileImage = Sprite_NewImage(Missile_GetImage());
		Sprite_t *missile = Sprite_New(	missileImage,
										randomAlien->posX + randomAlien->images->image->width/2 - randomAlien->images->image->width/2,
										randomAlien->posY + randomAlien->images->image->height);
		missile->move.x = 0;
		missile->move.y = 1; //moving from top to bottom
		if(MissilesMutex_Lock()){
			Sprite_AddToList(&missiles, missile);
			MissilesMutex_Unlock();
		}
	}

}

/* delete all sprites*/
static void FreeUpSprites(void){
	Sprite_DeleteList(&missiles);
	Sprite_DeleteList(&aliens);
	Sprite_FreeSprite(ship);
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

void InvaderGraphics_ShowEnded(bool won) {
	McuFontDisplay_PixelDim x, y, w_title, w_sub, h_title, h_sub;
	uint8_t title[10];
	const uint8_t titleLost[] = "You lost", titleWon[] = "You won";
	if(won){
		McuUtility_strcpy(title, sizeof(title), titleWon);
	} else{
		McuUtility_strcpy(title, sizeof(title), titleLost);
	}
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
	FreeUpSprites();			// game ended, remove all sprite images from storage
}

/* @brief Draws aliens
 * rows decides how many rows to draw
 */
static void DrawAliens(uint8_t rows){
	McuFontDisplay_PixelDim display_w, alien_w, alien_h, x_spacing = 2, y_spacing = 1;
	display_w = McuGDisplaySSD1306_GetWidth();
	alien_w = Alien0_GetImage()->width + x_spacing;
	alien_h = Alien0_GetImage()->height + y_spacing;
	uint8_t aliensPerRow = display_w/alien_w;

	if(AliensMutex_Lock(100))
	{
		// go through rows and columns and draw aliens
		for (int row = 0; row < rows; ++row) {
			for (int column = 0; column < aliensPerRow; ++column) {
				SpriteImage_t *alienImage = Sprite_NewImage(Alien0_GetImage());
				Sprite_t *alien = Sprite_New(	alienImage,	column*alien_w, row*alien_h);
				Sprite_AddToList(&aliens, alien);
			}
		}
		AliensMutex_Unlock();
	} else {
		printf("Could not aquire aliens mutex to draw aliens.");
	}
}

static bool SpriteCollidedWithTarget(Sprite_t *sprite, Sprite_t **targetList) {
	Sprite_t *target, *next;

	/* check if the sprite hits one in the target list: if yes, remove target */
	target = *targetList;
	while (target!=NULL) { /* check all targets, if we have hit one of them */
		next = target->next;
		if (   sprite->alive==-1 /* alive? */
		&& target->alive==-1 /* alive? */
		&& Sprite_Overlap(sprite, target) /* they collide? */
		)
		{ /* hit a target? */
			sprite->alive = target->alive = 0; /* mark to delete them  */
			return true; /* yes, sprite has hit a target */
		}
		target = next;
	} /* all targets */
	return false; /* no target hit */
}

static void CheckSpriteCollision(Sprite_t **spriteList, Sprite_t **targetList) {
	Sprite_t *sprite, *next, *newList = NULL;

	sprite = *spriteList;
	while (sprite!=NULL) { /* iterate list */
		next = sprite->next;
		Sprite_UnlinkFromList(spriteList, sprite); /* temporarily, remove current sprite from list, will be re-added later */
		if (!SpriteCollidedWithTarget(sprite, targetList)) { /* no hit and? Otherwise it is marked for deletion */
			Sprite_AddToList(&newList, sprite); /* add to temporary list */
		}
		sprite = next; /* next in list */
	} /* while */
	*spriteList = newList; /* restore list */
}

// check for collisions
// returns true if a collision with the ship happened
static bool CheckAndRemoveCollidingObjects(void){
	CheckSpriteCollision(&aliens, &missiles);
	SpriteCollidedWithTarget(ship, &missiles);
	Sprite_AgeAndCheckIfAlive(&missiles);
	Sprite_AgeAndCheckIfAlive(&aliens);
	return Sprite_AgeAndCheckIfAlive(&ship);
}



/* HELPERS */

void InvaderGraphics_Clear(void){
	Display_Clear();
}


static bool MissilesMutex_Lock(void){
	/* aquire mutex */
	if(xSemaphoreTakeRecursive(missilesMutex, pdMS_TO_TICKS(20)) != pdTRUE){
		return false; /* timeout? */
	}
	return true;
}

static void MissilesMutex_Unlock(void){
	/* give back mutex */
	if(xSemaphoreGiveRecursive(missilesMutex) != pdTRUE){
		/* issue */
		printf("Could not give back missiles mutex..");
		for(;;);
	}
}

static bool AliensMutex_Lock(uint16_t waitTimeMS){
	/* aquire mutex */
	if(xSemaphoreTakeRecursive(aliensMutex, pdMS_TO_TICKS(waitTimeMS)) != pdTRUE){
		return false; /* timeout? */
	}
	return true;
}

static void AliensMutex_Unlock(void){
	/* give back mutex */
	if(xSemaphoreGiveRecursive(aliensMutex) != pdTRUE){
		/* issue */
		printf("Could not give back aliens mutex..");
		for(;;);
	}
}

