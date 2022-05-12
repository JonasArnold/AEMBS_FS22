/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "alien0.h"

static const uint8_t pixData[] = {
  0x21, 0x00,
  0x92, 0x40,
  0xbf, 0x40,
  0xed, 0xc0,
  0xff, 0xc0,
  0x7f, 0x80,
  0x21, 0x00,
  0x40, 0x80,
};

static const TIMAGE image = {
  .width = 10, /* .header.w */
  .height = 8, /* .header.h */
  .size = sizeof(pixData),
  .pixmap = pixData,
  .name = "alien0.bmp",
};

const PIMAGE Alien0_GetImage(void) {
  return (PIMAGE)&image;
}
