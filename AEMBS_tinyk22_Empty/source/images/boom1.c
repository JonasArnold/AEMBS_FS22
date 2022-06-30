/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "boom1.h"

static const uint8_t pixData[] = {
  0x92, 0x40,
  0x40, 0x80,
  0x00, 0x00,
  0xcc, 0xc0,
  0x00, 0x00,
  0x40, 0x80,
  0x92, 0x40,
  0x21, 0x00,
};

static const TIMAGE image = {
  .width = 10, /* .header.w */
  .height = 8, /* .header.h */
  .size = sizeof(pixData),
  .pixmap = pixData,
  .name = "boom1.bmp",
};

const PIMAGE Boom1_GetImage(void) {
  return (PIMAGE)&image;
}
