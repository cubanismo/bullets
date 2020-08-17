
	.include	"jaguar.inc"

		.extern a_vdb
		.extern a_vde
		.extern a_hdb
		.extern a_hde
		.extern width
		.extern height

		.globl	_guninit
		.globl	_readgun

		.text

; void guninit(unsigned fb_width, unsigned fb_height);
_guninit:
		move.w	6(sp),fb_height
		move.w	4(sp),d0	; fb_width

		move.w	CONFIG,d1
		andi.w	#VIDTYPE,d1
		beq	.palvals

		move.w	#NTSC_HMID,hmid
		move.w	#NTSC_VMID,vmid
		move.w	#NTSC_WIDTH,d1
		bra	.calcxoff
.palvals:
		move.w	#PAL_HMID,hmid
		move.w	#PAL_VMID,vmid
		move.w	#PAL_WIDTH,d1
.calcxoff:
		; XXX Below Assumes PWIDTH4!
		; obj proc XPOS = (((screen width in clocks) / 4) - fb_width) / 2
		; obj proc XPOS * 4 = ((screen width in clocks) / 2) - (fb_width * 2)
		asr.w	#1,d1		; d1 = width in clocks / 2
		lsl.w	#1,d0		; d0 = fb_width * 2
		sub.w	d0,d1		; d1 = XPOS * 4 XXX d1 = 64

		; FB X coord = (LPH - a_hdb - ((obj proc XPOS) * 4)) / 4)
		; xoff4 = a_hdb + ((obj proc XPOS) * 4)
		add.w	a_hdb,d1

		; XXX Callibration Hack?
		add.w	#170,d1
		; XXX End CallibrationHack

		move.w	d1,xoff4

		rts

; d0 = (Framebuffer Y coordinate) << 16 | (Framebuffer X coordinate)
_readgun:
		move.w	LPV,d0		; d0.L16 = LPV
		andi.l	#$7ff,d0	; Mask off "field" bit of LPV
		add.w	fb_height,d0	; Add "half" FB height in halflines
		sub.w	vmid,d0		; Subtract [NTSC|PAL]_VMID (halflines)
		lsr.w	#1,d0		; halflines->scanlinesto get FB y coord
		swap	d0		; d0.H16 = vertical position

		move.w	LPH,d0		; d0.L16 = LPH
		move.w	d0,d1		; Make a copy of LPH value
		and.w	#$3ff,d0	; Mask off bit 10+ from return value
		btst.l	#10,d1		; If bit 10 was set, in 2nd half-line
		beq	.firsthalf
		add.w	hmid,d0		; In 2nd half-line: Add half-line size
.firsthalf:
		; FB X coord = (LPH - a_hdb - ((obj proc XPOS) * 4)) / 4
		sub.w	xoff4,d0
		lsr.w	#2,d0

		rts

		.bss
		.long

xoff4:		.ds.w	1
fb_height:	.ds.w	1
hmid:		.ds.w	1
vmid:		.ds.w	1
