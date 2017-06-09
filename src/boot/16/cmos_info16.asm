;===========================================================================
; cmos_info16.asm 
;   Copyright (C) 2012 Free Software Foundation, Inc.
;   Originally by ZhaoFeng Liang <zhf.liang@outlook.com>
;
;This file is part of DTHAS.
;
;DTHAS is free software; you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation; either version 2 of the License, or 
;(at your option) any later version.
;
;DTHAS is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with DTHAS; If not, see <http://www.gnu.org/licenses/>.  
;===========================================================================

%include	"16/cmos.h"
;===============================================================
cmos_info16:
;===============================================================

	call	get_cmos_info

	call	disp_cmos_info
	
	call	save_cmos_info			;all the information saves in 0x500

	ret

;===============================================================
get_cmos_info:
;===============================================================

	push	edx
	push 	ebx
	push	eax
	
	xor	edx, edx
	xor	ebx, ebx
	xor	eax, eax
.0:
	mov	al, bl
	mov	dx, CMOS_ADDR
	out	dx, al

	mov	dx, CMOS_DATA
	in 	al, dx
	mov	byte[cmos_info+bx], al

	add	bx, 1

	cmp	bx, CMOS_LEN
	jb	.0	
	

.end:
	pop	eax
	pop	ebx
	pop	edx

	ret

;===============================================================
disp_cmos_info:
;===============================================================

	push	ebp
	push	edi
	push	esi
	push	edx
	push	ecx
	push 	ebx
	push	eax

;-------------------------------------------------------------
; Date
;-------------------------------------------------------------

	mov	word[pos], 480			;(80 * 3) * 2

;head
;(Date:)
	mov	bx, 0xb800
	mov	es, bx
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 68
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 97
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2
	
;content
;century
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+0x32]
	call	disp_str

	mov	bx, [pos]
	
;year
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+9]
	call	disp_str
	
;-
	mov	bx, [pos]
	mov	al, 45
	mov	byte[es:bx], al
	add	bx, 2

;month
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+8]
	call	disp_str

;-
	mov	bx, [pos]
	mov	al, 45
	mov	byte[es:bx], al
	add	bx, 2

;day
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+7]
	call	disp_str

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;hour
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+4]
	call	disp_str

;:
	mov	bx, [pos]
	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

;min
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+2]
	call	disp_str

;:
	mov	bx, [pos]
	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

;min
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+0]
	call	disp_str

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;-------------------------------------------------------------
; memory(normal memory)
;-------------------------------------------------------------

	mov	word[pos], 640			;(80 * 4) * 2

;head
;(mem size:)
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 122
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;content
;mem size
	mov	[pos], bx
	xor	eax, eax
	mov	ax, word[cmos_info+0x15]
	call	disp_num16

	mov	bx, [pos]
;h
	mov	al, 104
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;kb
	mov	al, 107
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 98
	mov	byte[es:bx], al
	add	bx, 2

	
;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;-------------------------------------------------------------
; memory(extend memory)
;-------------------------------------------------------------

	mov	word[pos], 800			;(80 * 5) * 2

;head
;(mem size(ex):)
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 122
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;content
;mem size
	mov	[pos], bx
	xor	eax, eax
	mov	ax, word[cmos_info+0x17]
	call	disp_num16

	mov	bx, [pos]
;h
	mov	al, 104
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;kb
	mov	al, 107
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 98
	mov	byte[es:bx], al
	add	bx, 2

	
;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;-------------------------------------------------------------
; floppy drive
;-------------------------------------------------------------

	mov	word[pos], 960			;(80 * 6) * 2
	
;head
;(driveA:)
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 118
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 65
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;content
	
;driveA
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+0x10]
	mov	dl, al
	mov	cl, 4
	shr	ax, cl
	call	disp_floppy_info
	
;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;(driveB:)
	;mov	bx, [pos]
	xor	ax, ax

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 118
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 66
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;driveB
	mov	[pos], bx
	xor	eax, eax
	mov	al, dl
	and	al, 0xF
	call	disp_floppy_info
	
;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;-------------------------------------------------------------
; harddisk drive C
;-------------------------------------------------------------	
	mov	word[pos], 1120			;(80 * 7) * 2

	xor	eax, eax
	mov	al, byte[cmos_info+0x12]
	mov	dl, al
	and	dl, 0xF				; dl=D drive
	mov	cl, 4
	shr	ax, cl
	mov	dh, al				; dh=C drive	
	
;head
;(driveC:)
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 118
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 67
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;content
	
;driveC
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+0x19]
	call	disp_num16
	
;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	si, word[cmos_info+0x1D]		; cylinders of drive C

	mov	di, word[cmos_info+0x1F]		; heads of drive C
	and	di, 0xFF

	mov	bp, word[cmos_info+0x24]		; secters / cylinders
	and	bp, 0xFF

	mov	al, dh
	call	disp_hd_info

;-------------------------------------------------------------
; harddisk drive D
;-------------------------------------------------------------
;(driveD:)

	mov	word[pos], 1280			;(80 * 8) * 2

	mov	bx, [pos]
	xor	ax, ax

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 118
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 68
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;driveD
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[cmos_info+0x1A]
	call	disp_num16
	
;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	si, word[cmos_info+0x25]		; cylinders of drive D

	mov	di, word[cmos_info+0x27]		; heads of drive D
	and	di, 0xFF

	mov	bp, word[cmos_info+0x2C]		; secters / cylinders
	and	bp, 0xFF

	mov	al, dl
	call	disp_hd_info


.end:
	mov	[pos], bx
	mov	bx, 0x900
	mov	es, bx

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	esi
	pop	edi
	pop	ebp

	ret

;===============================================================
disp_floppy_info:
;al is the param
;===============================================================
	
	push	ebx

	mov	bx, [pos]

	cmp	al, 0
	je	.0
	cmp	al, 1
	je	.10
	cmp	al, 2
	je	.20
	cmp	al, 4
	je	.30
	cmp	al, 6
	je	.40

.0:
;(none)	
	xor	ax, ax

	mov	al, 110
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 110
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.10:
;(360kb)	
	xor	ax, ax

	mov	al, 51
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 54
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 107
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 98
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.20:
;(1.2mb)	
	xor	ax, ax

	mov	al, 49
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 46
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 98
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.30:
;(1.44mb)	
	xor	ax, ax

	mov	al, 49
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 46
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 52
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 52
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 98
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.40:
;(720b)	
	xor	ax, ax

	mov	al, 55
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 107
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 98
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end



.end:
	mov	[pos], bx

	pop	ebx

	ret

;===============================================================
disp_hd_info:
;param1: si : word, cylinders of drive C
;param2: di : byte, heads of drive C
;param3: bp : byte, secters / cylinders	
;param4: al : type of harddisk drive
;===============================================================
	
	push	ebx

	mov	bx, [pos]

	cmp	al, 0
	je	.0
	cmp	al, 0xF
	je	.10
	jmp	.20
	

.0:
;(none)	
	xor	ax, ax

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 110
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 110
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.10:
;(user def)	
	xor	ax, ax

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 117
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 102
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx

	call	disp_hd_param

	jmp	.end

.20:
;(sys def)	
	xor	ax, ax

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 121
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 102
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end


.end:
	mov	[pos], bx

	pop	ebx

	ret

;===============================================================
disp_hd_param:
;param1: si : word, cylinders of drive C
;param2: di : byte, heads of drive C
;param3: bp : byte, secters / cylinders	
;===============================================================
	
	push	ebx

	mov	bx, [pos]

;-------------------------------------------------------------
; cylinder
;-------------------------------------------------------------
;head
	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 121
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 108
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 110
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;content
	mov	[pos], bx
	xor	eax, eax
	mov	ax, si
	call	disp_num16
	
;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;-------------------------------------------------------------
; heads
;-------------------------------------------------------------
;head
	mov	al, 104
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 97
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;content
	mov	[pos], bx
	xor	eax, eax
	mov	ax, di
	call	disp_num16
	
;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;-------------------------------------------------------------
; sectors / cylinder
;-------------------------------------------------------------
;head
	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;content
	mov	[pos], bx
	xor	eax, eax
	mov	ax, bp
	call	disp_num16
	
;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

.end:
	mov	[pos], bx

	pop	ebx

	ret



;===============================================================
;(save cmos data into 0x500)
save_cmos_info:
;===============================================================

	push	edx
	push	ecx
	push 	ebx
	push	eax
	
	xor	edx, edx
	xor	ecx, ecx
	xor	ebx, ebx
	xor	eax, eax

	mov	edx, CMOS_SAVE_ADDR		;edx=0x500

	mov	cx, es		; save es
	mov	es, ax		; es = 0
.0:
	mov	al, byte[cmos_info+bx]
	mov	byte[es:edx], al

	add	edx, 1
	add	ebx, 1

	cmp	ebx, CMOS_LEN
	jb	.0	
	
	mov	es, cx		; restore es

.end:
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx

	ret
	


