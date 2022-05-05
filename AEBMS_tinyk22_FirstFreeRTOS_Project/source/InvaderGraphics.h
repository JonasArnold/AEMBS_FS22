/*
 * InvaderGraphics.h
 *
 *  Created on: 02.05.2022
 *      Author: jonas
 */

#ifndef INVADERGRAPHICS_H_
#define INVADERGRAPHICS_H_

typedef enum Ship_MoveDirection_e {
	Ship_MoveDir_Unknown,

	Ship_MoveDir_Left,
	Ship_MoveDir_Right,
	Ship_MoveDir_Up,
	Ship_MoveDir_Down,
} Ship_MoveDirection_e;

/* module initialization */
void InvaderGraphics_Init(void);

/* module de-initialization */
void InvaderGraphics_Deinit(void);

/* shows intro text */
void InvaderGraphics_ShowIntro(void);

/* clears display */
void InvaderGraphics_Clear(void);

/* move ship in a direction */
void InvaderGraphics_MoveShip(Ship_MoveDirection_e direction);

/* shoot a missile */
void InvaderGraphics_ShootMissile();

/* reacts to events and redraws UI */
void InvaderGraphics_UpdateView(void);



#endif /* INVADERGRAPHICS_H_ */
