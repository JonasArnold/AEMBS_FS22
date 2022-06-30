/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "sprite.h"
#include "McuRTOS.h"
#include <assert.h>

void Sprite_ImageDeleteList(SpriteImage_t **list);

unsigned int Sprite_NofInList(Sprite_t *list) {
  unsigned int nof = 0;

  while(list!=NULL) {
    nof++;
    list = list->next;
  }
  return nof;
}

Sprite_t *Sprite_IndexInList(Sprite_t *list, unsigned int index) {
  Sprite_t *sprite;
  int i;

  sprite = list;
  i = 0;
  for (;;) {
    if (sprite==NULL) {
      return NULL; /* end of list, index not found */
    }
    if (i==index) {
      return sprite;
    }
    sprite = sprite->next;
    i++;
  }
}

static McuGDisplaySSD1306_PixelDim LimitPosition(int pos, McuGDisplaySSD1306_PixelDim min, McuGDisplaySSD1306_PixelDim max) {
  if (pos<min) {
    return min;
  }
  if (pos>max) {
    return max;
  }
  return pos;
}

void Sprite_MoveLimited(Sprite_t *sprite, int deltaX, int deltaY, McuGDisplaySSD1306_PixelDim minX, McuGDisplaySSD1306_PixelDim maxX, McuGDisplaySSD1306_PixelDim minY, McuGDisplaySSD1306_PixelDim maxY) {
  if (sprite!=NULL) {
    sprite->posX = LimitPosition((int)sprite->posX + deltaX, minX, maxX-sprite->images->image->width);
    sprite->posY = LimitPosition((int)sprite->posY + deltaY, minY, maxY-sprite->images->image->height);
  }
}

/*!
 * brief Decides if area a and b overlap.
 * \param ax0 Area a, x left coordinate
 * \param ax1 Area a, x right coordinate
 * \param ay0 Area a, y top coordinate
 * \param ay1 Area a, y bottom coordinate
 * \param bx0 Area b, x left coordinate
 * \param bx1 Area b, x right coordinate
 * \param by0 Area b, y top coordinate
 * \param by1 Area b, y bottom coordinate
 * \return true if the two areas overlap, false otherwise
 */
bool Sprite_OverlappingAreas(int ax0, int ax1, int ay0, int ay1, int bx0, int bx1, int by0, int by1) {
  /* see https://stackoverflow.com/questions/306316/determine-if-two-rectangles-overlap-each-other */
   /* (x0,y0) are the left upper coordinate, (x1,y1) are lower right coordinates.
   * (0,0) is top left display coordinate, with x increasing to the right and y increasing to the bottom
   */

  /* *no* overlap exists, if
   *  (a) Left_Edge_A > Right_Edge_B (A is totally right of B)
   * or
   *  (b) Right_Edge_A < Left_Edge_B (A is totally left of B)
   * or
   *  (c) Top_A > Bottom_B (A is totally below B)
   * or
   *  (d) Bottom_A < Top_B (A is totally above B)
   *
   *  Therefore no overlap:  a || b || c || d
   *  or *overlap* is: !(a || b || c || d)  => De Morgans's law => !a && !b && !c && !d
   */
  return ax0<=bx1 && ax1>=bx0 && ay0<=by1 && ay1>=by0;
}

bool Sprite_OverlapsWithPixel(Sprite_t *sprite, int x, int y) {
  int ax0, ax1, ay0, ay1;
  int bx0, bx1, by0, by1;

  ax0 = sprite->posX;
  ax1 = sprite->posX + sprite->images->image->width-1;
  ay0 = sprite->posY;
  ay1 = sprite->posY + sprite->images->image->height-1;

  bx0 = x;
  bx1 = x;
  by0 = y;
  by1 = y;

  return Sprite_OverlappingAreas(ax0, ax1, ay0, ay1, bx0, bx1, by0, by1);
}

bool Sprite_Overlap(Sprite_t *a, Sprite_t *b) {
  /* see https://stackoverflow.com/questions/306316/determine-if-two-rectangles-overlap-each-other */
  int ax0, ax1, ay0, ay1;
  int bx0, bx1, by0, by1;
  /* (x0,y0) are the left upper coordinate, (x1,y1) are lower right coordinates.
   * (0,0) is top left display coordinate, with x increasing to the right and y increasing to the bottom
   * */
  ax0 = a->posX;
  ax1 = a->posX + a->images->image->width-1;
  ay0 = a->posY;
  ay1 = a->posY + a->images->image->height-1;

  bx0 = b->posX;
  bx1 = b->posX + b->images->image->width-1;
  by0 = b->posY;
  by1 = b->posY + b->images->image->height-1;

  return Sprite_OverlappingAreas(ax0, ax1, ay0, ay1, bx0, bx1, by0, by1);
}

SpriteImage_t *Sprite_NewImage(PIMAGE image) {
  SpriteImage_t *p;

  p = pvPortMalloc(sizeof(SpriteImage_t));
  assert(p!=NULL);
  p->image = image;
  p->next = p; /* circular */
  p->prev = p; /* circular */
  return p;
}

Sprite_t *Sprite_New(SpriteImage_t *images, McuGDisplaySSD1306_PixelDim x, McuGDisplaySSD1306_PixelDim y) {
  Sprite_t *p = pvPortMalloc(sizeof(Sprite_t));
  assert(p!=NULL);
  p->images = images;
  p->anim.animate = false; /* by default, no animation */
  p->anim.delayCntr = p->anim.delayCntrReload = 0; /* no animation delay */
  p->move.x = p->move.y = 0; /* not moving */
  p->move.delayCntr = p->move.delayCntrReload = 0; /* no moving delay */
  p->alive = -1; /* positive: counts down, zero: delete sprite */
  p->posX = x;
  p->posY = y;
  p->prev = p->next = NULL;
  return p;
}

void Sprite_FreeImage(SpriteImage_t *image) {
  if (image!=NULL) {
    vPortFree(image);
  }
}

void Sprite_FreeSprite(Sprite_t *sprite) {
  if (sprite!=NULL) {
    Sprite_ImageDeleteList(&sprite->images);
    vPortFree(sprite);
  }
}

void Sprite_AddToList(Sprite_t **list, Sprite_t *sprite) {
  if (*list==NULL) { /* empty list */
    *list = sprite;
  } else { /* insert in front */
    sprite->next = *list;
    sprite->prev = NULL;
    (*list)->prev = sprite;
    *list = sprite;
  }
}

void Sprite_ImageAddToList(SpriteImage_t **list, SpriteImage_t *image) {
  if (*list==NULL) { /* empty list: add to it (element is circular) */
    *list = image;
    image->next = image;
    image->prev = image;
  } else { /* insert in front, update circular list */
    (*list)->prev->next = image;
    image->next = *list;
    image->prev = (*list)->prev;
    (*list)->prev = image;
    *list = image;
  }
}

void Sprite_UnlinkFromList(Sprite_t **list, Sprite_t *sprite) {
  if (*list==sprite) { /* remove from front of list */
    if (sprite->next!=NULL) {
      sprite->next->prev = NULL;
    }
    *list = sprite->next;
  } else { /* unlink */
    if (sprite->next != NULL) {
      sprite->next->prev = sprite->prev;
    }
    if (sprite->prev!=NULL) {
      sprite->prev->next = sprite->next;
    }
  }
  sprite->next = NULL;
  sprite->prev = NULL;
}

void Sprite_DeleteFromList(Sprite_t **list, Sprite_t *sprite) {
  Sprite_UnlinkFromList(list, sprite);
  Sprite_FreeSprite(sprite);
}

void Sprite_ImageDeleteFromList(SpriteImage_t **list, SpriteImage_t *image) {
  if (image==NULL || list==NULL || *list==NULL) {
    return;
  }
  if (*list==image && (*list)->next==image) { /* only element in list? */
    *list = NULL; /* empty list */
  } else if (*list==image) { /* unlink and update pointer to list */
    *list = image->next; /* point to next element */
    image->next->prev = image->prev;
    image->prev->next = image->next;
  } else { /* unlink only */
    image->next->prev = image->prev;
    image->prev->next = image->next;
  }
  image->next = NULL;
  image->prev = NULL;
  Sprite_FreeImage(image);
}

void Sprite_DeleteList(Sprite_t **list) {
  if (list==NULL) {
    return;
  }
  while(*list!=NULL) {
    Sprite_DeleteFromList(list, *list);
  }
}

void Sprite_ImageDeleteList(SpriteImage_t **list) {
  if (list==NULL) {
    return;
  }
  while(*list!=NULL) {
    Sprite_ImageDeleteFromList(list, *list);
  }
}

bool Sprite_AgeAndCheckIfAlive(Sprite_t **list) {
  bool changed = false;
  Sprite_t *sprite, *next;

  /* check if ship missiles have hit an alien */
  sprite = *list;
  while (sprite!=NULL) { /* check all aliens */
    next = sprite->next;
    if (sprite->alive==0) { /* life expired? */
      Sprite_DeleteFromList(list, sprite); /* remove from list */
      changed = true;
    } else if (sprite->alive>0) {
      sprite->alive--;
      changed = true;
    }
    sprite = next;
  } /* while */
  return changed;
}

void Sprite_Paint(Sprite_t *sprite) {
  if (sprite->images!=NULL) {
    McuGDisplaySSD1306_DrawMonoBitmap(sprite->posX, sprite->posY, sprite->images->image, McuGDisplaySSD1306_COLOR_WHITE, McuGDisplaySSD1306_COLOR_BLACK);

    /* update animation for next iteration */
    if (sprite->anim.animate) {
      if (sprite->anim.delayCntrReload != 0) { /* have a delay? */
        if (sprite->anim.delayCntr==0) { /* expired: reload and paint */
          sprite->anim.delayCntr = sprite->anim.delayCntrReload;
          sprite->images = sprite->images->next; /* go to next image for next iteration */
        } else if (sprite->anim.delayCntr>0) { /* still counting down */
          sprite->anim.delayCntr--;
        }
      }
    }
  } /* if animate */

  /* update position for next iteration */
  if (sprite->move.x != 0 || sprite->move.y!=0) { /* move it? */
    if (sprite->move.delayCntr==0) { /* expired or no delay? */
      Sprite_MoveLimited(sprite, sprite->move.x, sprite->move.y, 0, McuGDisplaySSD1306_GetWidth(), 0, McuGDisplaySSD1306_GetHeight());
      if (sprite->move.delayCntrReload != 0) { /* restart counter? */
        sprite->move.delayCntr = sprite->move.delayCntrReload;
      }
    } else if (sprite->move.delayCntr>0) { /* count down? */
      sprite->move.delayCntr--;
    }
  } /* if move */
}

void Sprite_PaintList(Sprite_t *list) {
  Sprite_t *sprite = list;

  while(sprite!=NULL) {
    Sprite_Paint(sprite);
    sprite = sprite->next;
  }
}
