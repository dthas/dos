;===========================================================================
; shutdown.asm 
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
[SECTION .shutdown32]

ALIGN	32

[BITS	32]

;===========================================================================
shutdown:
;===========================================================================
	call	poweroff32

	ret

;===========================================================================
poweroff32:
;===========================================================================
	mov al, 7h
	mov dx, 0CF9h
	out dx, al
	jmp $

;===========================================================================
back_to_realmode:
;===========================================================================
	xor	ax, ax
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	mov	eax, cr0
	and	al, 11111110b
	mov	cr0, eax

	jmp	0:REAL_MODE_16


[SECTION .shutdown16]

ALIGN	16

[BITS	16]

REAL_MODE_16:

	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax

	mov	sp, 0x7c00

	in	al, 92h		
	and	al, 11111101b	
	out	92h, al		

	sti			

	call	poweroff

	hlt

;===========================================================================
poweroff:
;===========================================================================
	mov	ax, 0x5301
	xor	bx, bx
	int	0x15
	
	mov	ax, 0x530e
	xor	bx, bx
	mov	cx, 0x102
	int	0x15

	mov	ax, 0x5307
	mov	bx, 1
	mov	cx, 3
	int	0x15
	
	ret
