#include "blit.h"
#include "font.h"

extern FNThead fnt[];
extern unsigned char screen[];
extern volatile unsigned long ticks;

void start(void)
{
	/* Jaguar RGB16 bits: RRRR.RBBB.BBGG.GGGG. */
	unsigned short r = 0x1f;
	unsigned short g = 0x3f;
	unsigned short b = 0x1f;
	unsigned short bgcolor;
	int dy = 1;
	int y = 50;
	long blitflags = PITCH1|PIXEL16|WID320;

	unsigned long oldticks;

	/* Infinite Loop */
	while (1) {
		/* Cycle the clear color white->gray */
		if (r == 0) r = 0x1f; else r -= 1;
		if (b == 0) b = 0x1f; else b -= 1;
		if (g < 2) g = 0x3f; else g -= 2;
		/* Jaguar RGB16 bits: RRRR.RBBB.BBGG.GGGG. */
		bgcolor = (r << 11) | (b << 6) | g;

		/* Clear the screen */
		A1_BASE = (long)screen;
		A1_FLAGS = blitflags|XADDPHR;
		A1_PIXEL = 0ul;
		A1_CLIP = 0ul;
		A1_STEP = (1ul << 16) | (-320 & 0xfffful);
		B_COUNT = (240ul << 16) | 320ul;
		B_PATD[0] = (unsigned long)bgcolor << 16 | bgcolor;
		B_PATD[1] = (unsigned long)bgcolor << 16 | bgcolor;
		B_CMD = UPDA1|PATDSEL;

		y += dy;
		if (y > 239 || y < 1) dy = -dy;

		FNTstr(100, y, "Hello the world!", screen, blitflags,
		       fnt, 0xffff /* text color */, 0 /* no background color */);

		/* Wait for a new frame */
		oldticks = ticks;
		while (ticks < (oldticks + 4));
	}

}
