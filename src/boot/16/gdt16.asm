
;===========================================================================
; gdt16.asm
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

%include	"16/gdt.h"

;===============================================================
gdt16_main:
;===============================================================

	call	gdt_setting

	lgdt	[gdtr]

	cli

	xor	eax, eax
	
	in 	al, 0x92
	or	al, 00000010b
	out	0x92, al

	mov	eax, cr0
	or	eax, 1
	mov	cr0, eax

	jmp	dword SEL_CODE:(LOAD_ADDR + CODE32_START)

	ret


;===============================================================
gdt_setting
;===============================================================
	push	ebp
	push	edi
	push 	ebx
	push	eax

;------------------------------------------------------------
;eax: base addr; ebx: limit; edi: attribute; ebp: GDT index
;------------------------------------------------------------
	;Init GDT_CODE
	mov	ebp, GDT_CODE
	mov	eax, 0
	mov	ebx, 0xFFFFF
	mov	edi, ATTR_CODE_READ_EXE
	or	edi, ATTR_CODE_32
	or	edi, ATTR_UNIT_4K
	call	desc_setting

	;Init GDT_DATA
	mov	ebp, GDT_DATA
	mov	eax, 0
	mov	ebx, 0xFFFFF
	mov	edi, ATTR_DATA_READ_WRITE
	or	edi, ATTR_CODE_32
	or	edi, ATTR_UNIT_4K
	call	desc_setting

	;Init GDT_DISP
	mov	ebp, GDT_DISP
	mov	eax, 0xB8000
	mov	ebx, 0xFFFFF
	mov	edi, ATTR_DATA_READ_WRITE
	or	edi, ATTR_DPL3
	call	desc_setting

	pop	eax
	pop	ebx
	pop	edi
	pop	ebp

	ret

;===============================================================
desc_setting
;eax: base addr; ebx: limit; edi: attribute; ebp: GDT index
;===============================================================

	push	ecx
	push 	edx
	
	mov	word[ebp + 0], bx	; 0, 1; limit1

	mov	edx, eax
	and	edx, 0xFFFFFF		; base1
	mov	word[ebp + 2], dx	; 2, 3
	mov	cl, 16
	shr	edx, cl
	mov	byte[ebp + 4], dl	; 4

	mov	edx, ebx
	mov	cl, 16
	shr	edx, cl
	mov	cl, 8
	shl	dx, cl			; limit2
	or	dx, di			; attribute
	mov	word[ebp + 5], dx	; 5, 6

	mov	cl, 24
	shr	eax, cl			; base2
	mov	byte[ebp + 7], al	; 7
		
	pop 	edx
	pop	ecx

	ret
