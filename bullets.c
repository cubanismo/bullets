#include "jaguar.h"
#include "font.h"

extern FNThead fnt[];
extern unsigned char screen[];
extern volatile unsigned long ticks;
extern int sprintf(char *, const char *, ...);
extern unsigned long readgun(void);

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
	short lph[16];
	short lpv[16];
	long lpavgh;
	long lpavgv;
	int lpi = 0;
	int i;

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
		lph[lpi] = (unsigned short)(lp & 0xffff);
		lpv[lpi] = (unsigned short)(lp >> 16);
		lpi = (lpi + 1) % 16;
		lpavgh = lpavgv = 0;
		for (i = 0; i < 16; i++) {
			lpavgh += lph[i];
			lpavgv += lpv[i];
		}
		lpavgh >>= 4;
		lpavgv >>= 4;

		/* Include a few spaces to clear prior larger numbers */
		sprintf(str, "Hello @(%d, %d)      ", (int)lpavgh, (int)lpavgv);
		FNTstr(x, y, str, screen, blitflags, fnt,
		       (0x10 << 11) | (0x10 << 6) | 0x20 /* text color */,
		       bgcolor /* background color */);

		/* Wait for a new frame */
		oldticks = ticks;
		while (ticks == oldticks);
	}
}
