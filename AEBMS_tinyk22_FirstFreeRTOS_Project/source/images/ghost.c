#include "ghost.h" /* include of own header file */

/* the bitmap data, copy from generated file */
static const uint8_t pixData[] = {
  /* put bitmap data here */
	0x00, 0x00,
	0x20, 0x80,
	0x91, 0x20,
	0xbf, 0xa0,
	0xff, 0xe0,
	0xed, 0xe0,
	0xff, 0xe0,
	0x7f, 0xc0,
	0x11, 0x00,
	0x20, 0x80,
};

static const TIMAGE image = {
  .width = 11, /* number from converted file: .header.w */
  .height = 10, /* number from converted file: .header.h */
  .size = sizeof(pixData), /* size of bitmap data */
  .pixmap = pixData, /* pointer to bitmap data above */
  .name = "ghost.bmp", /* optional name of file */
};

const PIMAGE Ghost_GetImage(void) {
  return (PIMAGE)&image;
}
