#include "ship.h" /* include of own header file */

/* the bitmap data, copy from generated file */
static const uint8_t pixData[] = {
  /* put bitmap data here */
	0x00, 0x00,
	0x04, 0x00,
	0x04, 0x00,
	0x04, 0x00,
	0x0e, 0x00,
	0x0e, 0x00,
	0x1f, 0x00,
	0x3f, 0x80,
	0x7f, 0xc0,
	0x4a, 0x40,
};

static const TIMAGE image = {
  .width = 11, /* number from converted file: .header.w */
  .height = 10, /* number from converted file: .header.h */
  .size = sizeof(pixData), /* size of bitmap data */
  .pixmap = pixData, /* pointer to bitmap data above */
  .name = "ship.bmp", /* optional name of file */
};

const PIMAGE Ship_GetImage(void) {
  return (PIMAGE)&image;
}
