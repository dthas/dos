;===========================================================================
; tss.asm 
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
[BITS	32]

;===============================================================
tss_desc_setting:
;===============================================================
	push	ebp
	push	edi
	push 	ebx
	push	eax

;------------------------------------------------------------
;tss structure setting
;------------------------------------------------------------
	mov	ebp, tss
	mov	dword[ebp + 8], SEL_DATA	; ss0 
	MOV	word[ebp + 98], tss_len		; no I/O permission

;------------------------------------------------------------
;eax: base addr; ebx: limit; edi: attribute; ebp: GDT index
;------------------------------------------------------------
	;Init GDT_TSS
	mov	ebp, gdt_desc
	add	ebp, GDT_TSS

	mov	eax, tss
	mov	ebx, tss_len
	mov	edi, ATTR_TSS
	
	;this function is in klib.asm
	call	desc_setting

;------------------------------------------------------------
;start function of tss
;------------------------------------------------------------
	xor	eax, eax
	mov	ax, SEL_TSS
	ltr	ax
	
	pop	eax
	pop	ebx
	pop	edi
	pop	ebp

	ret
