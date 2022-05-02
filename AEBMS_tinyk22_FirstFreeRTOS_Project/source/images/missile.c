#include "missile.h" /* include of own header file */

/* the bitmap data, copy from generated file */
static const uint8_t pixData[] = {
  /* put bitmap data here */
	0x80,
	0x80,
	0x80,
};

static const TIMAGE image = {
  .width = 1, /* number from converted file: .header.w */
  .height = 3, /* number from converted file: .header.h */
  .size = sizeof(pixData), /* size of bitmap data */
  .pixmap = pixData, /* pointer to bitmap data above */
  .name = "missile.bmp", /* optional name of file */
};

const PIMAGE Missile_GetImage(void) {
  return (PIMAGE)&image;
}
