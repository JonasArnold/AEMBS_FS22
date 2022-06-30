/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include "McuGDisplaySSD1306.h"
#include "stdbool.h"

/* structure to describe a sprite image */
typedef struct SpriteImage_t {
  struct SpriteImage_t *prev, *next; /* circular double linked list, used for animations */
  PIMAGE image; /* pointer to image information */
} SpriteImage_t;

/* structure to describe a sprite with all its properties. The sprites can build a linear list (double-linked) */
typedef struct Sprite_t {
  struct Sprite_t *prev, *next; /* double-linked list of sprites */
  SpriteImage_t *images; /* pointer to circular image list: the list gets played if 'animate' is set */
  struct {
    bool animate; /* true if it shall play animation: it will iterate through list of images */
    uint8_t delayCntr; /* delay counter (counting down) to delay animation */
    uint8_t delayCntrReload; /* default 0. Reload value of delayCntr after it reaches zero. */
  } anim; /* used for animations */
  struct {
    int8_t x, y; /* moving deltas */
    uint8_t delayCntr; /* delay counter (counting down) to delay moving */
    uint8_t delayCntrReload; /* default 0. Reload value of delayCntr after it reaches zero. */
  } move; /* used to move a sprite */
  int alive; /* default -1. Used for timed removal of sprite. If positive, it gets counted down to zero. With zero it gets removed */
  McuGDisplaySSD1306_PixelDim posX, posY; /* current position on screen */
} Sprite_t;

/*!
 * \brief Create a new sprite object
 * \param images List of images. Can be a single image or a list of images for an animation
 * \param x Initial x position coordinate
 * \param y Initial y position coordinate
 * \return The newly created sprite object, or NULL if failed (no memory)
 */
Sprite_t *Sprite_New(SpriteImage_t *images, McuGDisplaySSD1306_PixelDim x, McuGDisplaySSD1306_PixelDim y);

/*!
 * \brief Releases a sprite object and frees the memory for it, including its images
 * @param sprite The sprite object to be deleted
 */
void Sprite_FreeSprite(Sprite_t *sprite);

/*!
 * \brief Return the number of sprites in a list
 * \param list List to count the elements in it
 * \return Number of sprites in the list
 */
unsigned int Sprite_NofInList(Sprite_t *list);

/*!
 * \brief Used to return a sprite in a list with a given index (order number).
 * \param list List of sprites
 * \param index The index of the sprite to be returned
 * \return Pointer to the sprite or NULL if index does not exist
 */
Sprite_t *Sprite_IndexInList(Sprite_t *list, unsigned int index);

/*!
 * \brief Add a sprite to a list
 * \param list List where the sprite will be added. Note that this is a reference paramter, as the new sprite could be added at the beginning of the list
 * \param sprite Sprite to be added
 */
void Sprite_AddToList(Sprite_t **list, Sprite_t *sprite);

/*!
 * \brief Remove a sprite from a list and free the memory of it (deletes it)
 * \param list List of Sprites
 * \param sprite The resulting new list (could be empty list or NULL)
 */
void Sprite_DeleteFromList(Sprite_t **list, Sprite_t *sprite);

/*!
 * \brief Removes a sprite from a list, but does not delete it
 * \param list List of sprites
 * \param sprite Sprite to be removed
 */
void Sprite_UnlinkFromList(Sprite_t **list, Sprite_t *sprite);

/*!
 * \brief Deletes a list of sprites and frees the memory
 * \param list List to be deleted
 */
void Sprite_DeleteList(Sprite_t **list);

/*!
 * \brief Moves a sprite, but keeps it within the display limits.
 * \param sprite Sprite to be moved
 * \param deltaX Amount of pixels in X direction
 * \param deltaY Amount of pixels in Y direction
 * \param minX Display limit, x minimum (left) coordinate
 * \param maxX Display limit, x maximum (top) coordinate
 * \param minY Display limit, y minimum (top) coordinate
 * \param maxY Display limit, y maximum (bottom) coordinate
 */
void Sprite_MoveLimited(Sprite_t *sprite, int deltaX, int deltaY, McuGDisplaySSD1306_PixelDim minX, McuGDisplaySSD1306_PixelDim maxX, McuGDisplaySSD1306_PixelDim minY, McuGDisplaySSD1306_PixelDim maxY);

/*!
 * \brief Decides if two sprite overlap
 * \param a First sprite
 * \param b Second sprite
 * \return true if they overlap, false otherwise
 */
bool Sprite_Overlap(Sprite_t *a, Sprite_t *b);

/*!
 * Decides if two areas a and b overlap
 * \param ax0 Area a x left coordinate
 * \param ax1 Area a x right coordinate
 * \param ay0 Area a y top coordinate
 * \param ay1 Area a y bottom coordinate
 * \param bx0 Area b x left coordinate
 * \param bx1 Area b x right coordinate
 * \param by0 Area b y top coordinate
 * \param by1 Area b y bottom coordinate
 * \return true if a and b overlap, false otherwise
 */
bool Sprite_OverlappingAreas(int ax0, int ax1, int ay0, int ay1, int bx0, int bx1, int by0, int by1);

/*!
 * \brief Decides if a sprite and a pixel overlap
 * \param sprite Sprite to check
 * \param x Pixel x coordinate
 * \param y Pixel y coordinate
 * \return true if sprite and pixel overlap, false otherwise
 */
bool Sprite_OverlapsWithPixel(Sprite_t *sprite, int x, int y);


/*------------------------------------------------------------------------------ */
/* Painting routines */
/*------------------------------------------------------------------------------ */
/*!
 * \item Traverses a list of sprites and increases their age. If a sprite expires, it gets removed from the list. Useful for animations (explosions)
 * \param list List of sprites
 * \return true if list has changed (sprite deleted), false otherwise
 */
bool Sprite_AgeAndCheckIfAlive(Sprite_t **list);

/*!
 * \brief Paints or draws a sprite. Additionally if a sprite is setup for animation, it will advance the animation picture state.
 * \param sprite Sprite to be painted
 */
void Sprite_Paint(Sprite_t *sprite);

/*!
 * \brief Same as Sprite_Paint(), but iterates a list of sprites
 * \param list List of sprites
 */
void Sprite_PaintList(Sprite_t *list);


/*------------------------------------------------------------------------------ */
/* images */
/*------------------------------------------------------------------------------ */
/*!
 * \brief Create a new sprite image
 * \param image Image data to be used
 * \return Pointer to sprite image or NULL if failed (no memory)
 */
SpriteImage_t *Sprite_NewImage(PIMAGE image);

/*!
 * \brief Ad a sprite image to a list of sprite images
 * @param list
 * @param image
 */
void Sprite_ImageAddToList(SpriteImage_t **list, SpriteImage_t *image);

/*!
 * \brief Delete a list of sprite images
 * \param list List to be deleted
 */
void Sprite_ImageDeleteList(SpriteImage_t **list);


#endif /* SPRITE_H_ */
