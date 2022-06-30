/*
 * InvaderGraphics.h
 *
 *  Created on: 02.05.2022
 *      Author: jonas
 */

#ifndef INVADERGRAPHICS_H_
#define INVADERGRAPHICS_H_

#include <stdbool.h>

typedef enum Ship_MoveDirection_e {
	Ship_MoveDir_Unknown,

	Ship_MoveDir_Left,
	Ship_MoveDir_Right,
	Ship_MoveDir_Up,
	Ship_MoveDir_Down,
} Ship_MoveDirection_e;

typedef enum UpdateView_Result_e {
	UpdateView_Result_Normal,

	UpdateView_Result_CollisionWithShip,
	UpdateView_Result_NoMoreAliens,
} UpdateView_Result_e;

/* module initialization */
void InvaderGraphics_Init(void);

/* module de-initialization */
void InvaderGraphics_Deinit(void);

/* shows intro text */
void InvaderGraphics_ShowIntro(void);

/* shows ended text, if won is true, the game was won */
void InvaderGraphics_ShowEnded(bool won);

/* draws initial objects */
void InvaderGraphics_DrawNewGame(void);

/* clears display */
void InvaderGraphics_Clear(void);

/* move ship in a direction */
void InvaderGraphics_MoveShip(Ship_MoveDirection_e direction);

/* shoot a missile */
void InvaderGraphics_ShootMissile(void);

/* lets a random alien shoot a missile at the ship */
void InvaderGraphics_GenerateAlienMissile(void);

/* reacts to events and redraws UI, returns an enum to inform about potential events */
UpdateView_Result_e InvaderGraphics_UpdateView(void);



#endif /* INVADERGRAPHICS_H_ */
