;===========================================================================
; format.asm 
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

;=====================
format:
;=====================
	mov	ax, mesg_format
	mov	cx, MESG_FORMAT_LEN
	mov	dh, 6
	call	disp_msg

	call	format_main

	mov	ax, mesg_format1
	mov	cx, MESG_FORMAT_LEN
	mov	dh, 7
	call	disp_msg

	ret

;=====================
format_main:
;=====================

	call 	init_hd
	call	iden_get
	call	iden_disp
	call	partition
	call	filesys
	call	copyfile
	
	ret
;=====================
init_hd:
;get number of drive(0x475)
;=====================
	
;head
	mov	bx, 0xb800
	mov	es, bx
	mov	bx, [pos]
	xor	ax, ax

;nr drive: 
	mov	al, 110
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

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx
	mov	bx, 0x900
	mov	es, bx

;content
	xor 	eax, eax

	mov	bx, 0
	mov	ds, bx		;ds=00

	mov	al, byte[0x475]	;al=0:0x475

	mov	bx, cs
	mov	ds, bx		;ds=0x9000

	call	disp_str

	ret

;=====================
iden_get:
;=====================
;---------------
;send command
;---------------

;fill cmd
	;mov	byte[cmd+0], 0
	mov	byte[cmd+1], 0
	mov	byte[cmd+2], 0
	mov	byte[cmd+3], 0
	mov	byte[cmd+4], 0
	mov	byte[cmd+5], 0	
	mov	byte[cmd+6], 0xa0
	mov	byte[cmd+7], HD_IDEN

	call	cmd_send

;---------------
;get hd status
;---------------
	

;---------------
;get hd info
;---------------

;wait
	call	wait_1000

;get info(info size: 1 sector)
	xor	eax, eax
	xor	bp, bp
	mov	dx, CBP_PORT0		;dx=port=0x1f0

	mov	ax, 0x900
	mov	es, ax

.0:
	call	in_16
	mov	word[iden_info+bp],ax
	add	bp, 2

	cmp	bp, 512
	jne	.0	

	mov	ax, 0xb800
	mov	es, ax

	ret

;=====================
iden_disp:
;=====================

	push	ebp

	xor	bp, bp

	mov	bx, 0xb800
	mov	es, bx
	mov	bx, [pos]
;-----------------
;sequence number
;-----------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 113
	mov	byte[es:bx], al
	add	bx, 2
	
	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2
	
;content
	mov	bp, NUM_SEQ_START
.10:
	xor	ax, ax
	mov	ax, word[iden_info+bp]
	mov	byte[es:bx], ah
	add	bx, 2
	mov	byte[es:bx], al
	add	bx, 2
	
	
	add 	bp, 2
	cmp	bp, NUM_SEQ_END
	jne	.10

;-----------------
;type
;-----------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 121
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 112
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2
	
	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

;content
	mov	bp, TYPE_START
.20:
	xor	ax, ax
	mov	ax, word[iden_info+bp]
	mov	byte[es:bx], ah
	add	bx, 2
	mov	byte[es:bx], al
	add	bx, 2

	add 	bp, 2
	cmp	bp, TYPE_END
	jne	.20

;-----------------
;sector count
;-----------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 110
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2
	
	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

;content

	xor 	ax, ax
	xor	cx, cx
	mov	bp, NUM_SEC_START
	mov	[pos] , bx

	mov	eax, dword[iden_info+bp]
	mov	dx, ax		; dx = lower 16 bit
	mov	cl, 16
	shr	eax,cl
	mov	cx, ax		; cx = higher 16 bit

	xor	ah, ah

	mov	al, ch
	call	disp_str
	mov	al, cl
	call	disp_str
	mov	al, dh
	call	disp_str
	mov	al, dl
	call	disp_str

	mov	bx, [pos]

;for example, (0x63m)
;(
	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

;0
	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

;x
	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2


	mov	[pos] , bx

;convert to megabytes
	xor	eax, eax
	mov	ax, cx
	mov	cl, 16
	shl	eax, cl
	mov	ax, dx
	mov	cl, 11
	shr	eax, cl		;eax = eax * 512 / (1024*1024)

	mov	dx, ax		; dx = lower 16 bit
	mov	cl, 16
	shr	eax,cl
	mov	cx, ax		; cx = higher 16 bit

	xor	ah, ah

	mov	al, ch
	call	disp_str
	mov	al, cl
	call	disp_str
	mov	al, dh
	call	disp_str
	mov	al, dl
	call	disp_str

	mov	bx, [pos]

;m
	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

;)
	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2
	
	mov	[pos] , bx

;---------------
;lba support
;---------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 108
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 98
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 97
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

;content
	mov	bp, LBA_SUP_START

	xor	ax, ax
	mov	ax, word[iden_info+bp]
	and	ax, 0x200
	cmp	ax, 0
	je	.40
	mov	al, 89
	jmp	.41
.40:
	mov	al, 78
.41:
	mov	byte[es:bx], al

	add	bx, 2
	

;---------------
;instruction set
;---------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

;content
	mov	bp, INS_SET_START

	xor	ax, ax
	mov	ax, word[iden_info+bp]
	and	ax, 0x400
	cmp	ax, 0
	je	.50
	mov	al, 89
	jmp	.51
.50:
	mov	al, 78
.51:
	mov	byte[es:bx], al

	add	bx, 2


;-------------
;restore 
;-------------
	
	mov	word[pos], bx

	mov	bx, 0x900
	mov	es, bx
	
	pop	ebp
	
	ret

	
;=======================================================================
mesg_format:		db	"Format begin...."
mesg_format1:		db	"Format end......"
mesg_format2:		db	"Iden disp begin."
mesg_format3:		db	"Iden disp end..."
MESG_FORMAT_LEN		equ	16
;=======================================================================
