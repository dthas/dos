;===========================================================================
; ulib.asm 
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

%include "ulib.h"

[section .text]

global	u_desc_baseaddr_get

;===============================================================
u_desc_baseaddr_get:
;PUBLIC u32 u_desc_baseaddr_get(u32 addr, u32 gdt_desc)
;eax: base addr(return); ebp: addr
;===============================================================

	push	ecx
	push 	edx
	push	ebp
	
	xor	eax, eax

;ebp = gdt_desc + PROC_SYS_FIRST
	mov	ecx, DESC_SYS_FIRST
	mov	ebp, [esp + 20]		; gdt_desc
	add	ebp, ecx	
	
;ebp = gdt_desc + PROC_SYS_FIRST + index * 24 bytes
	mov	eax, [esp + 16]		; index, at the begining, there are 3 push, which is 4 bytes each
	mov	cl, 24			; 8 * 3
	mul	cl
	add	ebp, eax

	xor	edx, edx

	mov	al, byte[ebp + 7]	; base3
	mov	cl, 24
	shl	eax, cl

	mov	dl, byte[ebp + 4]	; base2
	mov	cl, 16
	shl	edx, cl
	or	eax, edx

	mov	ax, word[ebp + 2]	; base1

	pop	ebp
	pop 	edx
	pop	ecx

	ret
