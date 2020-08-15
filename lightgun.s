
	.include	"jaguar.inc"

		.extern a_vdb
		.extern a_vde
		.extern a_hdb
		.extern a_hde
		.extern width
		.extern height

		.globl _readgun

		.text

; d0 = (full-line vertical position) << 16 | (full-line horizontal pixel count)
_readgun:
		move.w	LPV,d0		; d0.L16 = LPV
		andi.l	#$7ff,d0	; Mask off "field" bit of LPV
		lsr.w	#1,d0		; Divide by two to get full-line count
		swap	d0		; d0.H16 = vertical position

		move.w	LPH,d0		; d0.L16 = LPH
		move.w	d0,d1		; Make a copy of LPH value
		and.w	#$3ff,d0	; Mask off bit 10+ from return value
		btst.l	#10,d1		; If bit 10 was set, in 2nd half-line
		beq	.firsthalf
		move.w	a_hde,d1	; In 2nd half-line: Add half-line size
		andi.w	#$3ff,d1
		add.w	d1,d0
.firsthalf:
		lsr.w	#2,d0		; horiz_pos /= 4. XXX Assumes PWIDTH4!
		rts
