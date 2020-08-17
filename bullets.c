#include "jaguar.h"
#include "font.h"

extern FNThead fnt[];
extern unsigned short screen[];
extern volatile unsigned long ticks;
extern int sprintf(char *, const char *, ...);
extern unsigned long readgun(void);
extern void guninit(unsigned fb_width, unsigned fb_height);

void start(void)
{
	/* Jaguar RGB16 bits: RRRR.RBBB.BBGG.GGGG. */
	unsigned short r = 0x1f;
	unsigned short g = 0x3f;
	unsigned short b = 0x1f;
	unsigned short bgcolor;
	char str[80];
	int y = 200;
	int x = 20;
	long blitflags = PITCH1|PIXEL16|WID320;
	short lpx[16];
	short lpy[16];
	long lpavgx;
	long lpavgy;
	int lpi = 0;
	int i;

	guninit(320, 240);

	unsigned long oldticks;

	/* Jaguar RGB16 bits: RRRR.RBBB.BBGG.GGGG. */
	bgcolor = (r << 11) | (b << 6) | g;

	/* Clear the screen */
	*A1_BASE = (long)screen;
	*A1_FLAGS = blitflags|XADDPHR;
	*A1_PIXEL = 0ul;
	*A1_CLIP = 0ul;
	*A1_STEP = (1ul << 16) | (-320 & 0xfffful);
	*B_COUNT = (240ul << 16) | 320ul;
	(B_PATD)[0] = (unsigned long)bgcolor << 16 | bgcolor;
	(B_PATD)[1] = (unsigned long)bgcolor << 16 | bgcolor;
	*B_CMD = UPDA1|PATDSEL;

	/* Infinite Loop */
	while (1) {
		unsigned long lp = readgun();

		/* Smooth out the value a bit: Average last 16 frames */
		lpx[lpi] = (unsigned short)(lp & 0xffff);
		lpy[lpi] = (unsigned short)(lp >> 16);
		lpi = (lpi + 1) % 16;
		lpavgx = lpavgy = 0;
		for (i = 0; i < 16; i++) {
			lpavgx += lpx[i];
			lpavgy += lpy[i];
		}
		lpavgx >>= 4;
		lpavgy >>= 4;

		/* Plot the point on the framebuffer */
		if (lpavgx < 320 && lpavgy < 240)
			screen[320 * lpavgy + lpavgx] = (0x18 << 11) | (0x18 << 6) | 0x30;

		/* Include a few spaces to clear prior larger numbers */
		sprintf(str, "Gun @(%d, %d)      ", (int)lpavgx, (int)lpavgy);
		FNTstr(x, y, str, screen, blitflags, fnt,
		       (0x10 << 11) | (0x10 << 6) | 0x20 /* text color */,
		       bgcolor /* background color */);

		/* Wait for a new frame */
		oldticks = ticks;
		while (ticks == oldticks);
	}
}
