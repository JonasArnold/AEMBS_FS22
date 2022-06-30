/*
 * Copyright (c) 2022, Erich Styger
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "boom0.h"

static const uint8_t pixData[] = {
  0x00, 0x00,
  0x4c, 0x80,
  0x21, 0x00,
  0xcc, 0xc0,
  0x21, 0x00,
  0x4c, 0x80,
  0x12, 0x00,
  0x00, 0x00,
};

static const TIMAGE image = {
  .width = 10, /* .header.w */
  .height = 8, /* .header.h */
  .size = sizeof(pixData),
  .pixmap = pixData,
  .name = "boom0.bmp",
};

const PIMAGE Boom0_GetImage(void) {
  return (PIMAGE)&image;
}
