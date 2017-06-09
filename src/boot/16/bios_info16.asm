;===========================================================================
; bios_info16.asm 
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

%include	"16/bios.h"
;===============================================================
bios_info16:
;===============================================================

	call	disp_bios_info

	ret

;===============================================================
disp_bios_info:
;===============================================================

	push	ebp
	push	edi
	push	esi
	push	edx
	push	ecx
	push 	ebx
	push	eax

.begin:
	mov	bx, 0xb800
	mov	es, bx

	mov	ax, 0x40
	mov	fs, ax
;-------------------------------------------------------------
; com
;-------------------------------------------------------------

	mov	word[pos], 1440			;(80 * 9) * 2

;head
;(com1:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 49
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
	mov	ax, word[fs:COM1_ADDR_OFFSET]
	call	disp_num16

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;,
	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2	

;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx

;head
;(com2:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
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
	mov	ax, word[fs:COM2_ADDR_OFFSET]
	call	disp_num16

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;,
	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2	

;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx

;head
;(com3:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 51
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
	mov	ax, word[fs:COM3_ADDR_OFFSET]
	call	disp_num16

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;,
	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2	

;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx

;head
;(com4:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 52
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
	mov	ax, word[fs:COM4_ADDR_OFFSET]
	call	disp_num16

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx
;-------------------------------------------------------------
; I/O port
;-------------------------------------------------------------

	mov	word[pos], 1600			;(80 * 10) * 2

;head
;(lpt1:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 108
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 112
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 49
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
	mov	ax, word[fs:LPT1_ADDR_OFFSET]
	call	disp_num16

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;,
	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2	

;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx

;head
;(lpt2:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 108
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 112
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
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
	mov	ax, word[fs:LPT2_ADDR_OFFSET]
	call	disp_num16

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;,
	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2	

;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx

;head
;(lpt3:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 108
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 112
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 51
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
	mov	ax, word[fs:LPT3_ADDR_OFFSET]
	call	disp_num16

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx

;-------------------------------------------------------------
; Adapter memory size
;-------------------------------------------------------------

	mov	word[pos], 1760			;(80 * 11) * 2

;head
;(adp:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 118
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 97
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 100
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 112
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

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2
	
;content
	mov	[pos], bx
	xor	eax, eax
	mov	ax, word[fs:ADPS_ADDR_OFFSET]
	call	disp_num16

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;,
	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2	

;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx


;head
;(vmode:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 118
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 109
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 100
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
	mov	[pos], bx
	xor	eax, eax
	mov	al, byte[fs:VMODE_ADDR_OFFSET]
	;call	disp_num16
	call	disp_vmode_info

;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

;,
	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2	

;space
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx

;head
;(vport:)
	
	mov	bx, [pos]
	xor	ax, ax

	mov	al, 118
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 112
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2
	
	mov	al, 116
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
	mov	ax, word[fs:VPORT_ADDR_OFFSET]
	call	disp_num16
	
;space
	mov	bx, [pos]
	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	mov	[pos], bx



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
disp_vmode_info:
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
	cmp	al, 3
	je	.30
	cmp	al, 4
	je	.40
	cmp	al, 5
	je	.50
	cmp	al, 6
	je	.60
	cmp	al, 7
	je	.70

.0:
;(40x25ts)	
	xor	ax, ax

	mov	al, 52
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 53
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
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
;(40x25tc)	
	xor	ax, ax

	mov	al, 52
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 53
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.20:
;(80x25ts)	
	xor	ax, ax

	mov	al, 56
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 53
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.30:
;(40x25)	
	xor	ax, ax

	mov	al, 56
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 53
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.40:
;(320x200ts)	
	xor	ax, ax

	mov	al, 51
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.50:
;(320x200tc)	
	xor	ax, ax

	mov	al, 51
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 99
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end



.60:
;(640x200ts)	
	xor	ax, ax

	mov	al, 54
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 52
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 115
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 41
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 32
	mov	byte[es:bx], al
	add	bx, 2

	jmp	.end

.70:
;(640x200tc)	
	xor	ax, ax

	mov	al, 54
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 52
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 120
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 50
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 48
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 40
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 99
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
