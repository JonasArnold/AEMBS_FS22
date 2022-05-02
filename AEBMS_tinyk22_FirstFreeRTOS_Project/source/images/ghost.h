/*
 * ghost.h
 *
 *  Created on: 02.05.2022
 *      Author: jonas
 */

#ifndef IMAGES_GHOST_H_
#define IMAGES_GHOST_H_

#include "McuGDisplaySSD1306.h"

/*!
 * \brief Return the image
 * \return Pointer to the (const) image data
 */
const PIMAGE Ghost_GetImage(void);


#endif /* IMAGES_GHOST_H_ */
