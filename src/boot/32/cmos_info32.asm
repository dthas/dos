;===========================================================================
; cmos_info32.asm 
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

;===============================================================
cmos_info32:
;===============================================================

	; counting by double word in memory
	call	sysdate_resave

	
	ret


;===============================================================
sysdate_resave:
; from 0x500 to 0xFFFFA0
;===============================================================

	push	edx
	push	ecx
	push 	ebx
	push	eax
	
	xor	edx, edx
	xor	ecx, ecx
	xor	ebx, ebx
	xor	eax, eax
.0:
	mov	ebx, 0x500			;src: ebx = 0x500
	mov	edx, 0xFFFFA0			;dst: edx = 0xFFFFA0
	
	
	mov	al, byte[ebx]
	mov	byte[edx], al

	add	edx, 1
	add	ebx, 1
	add	ecx, 1

	cmp	ecx, 64
	jb	.0	
	

.end:
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx

