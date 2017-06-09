;===========================================================================
; gdt.asm 
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

%include	"gdt.h"
;===============================================================
gdt_desc_setting:
;===============================================================

	push	esi
	push	edi
	push 	ecx
	push	eax	

	xor	ecx, ecx

	sgdt	[gdtr]

	mov	edi, gdt_desc			; dest address
	mov	esi, dword[gdtr+2]		; src  address
	mov 	cx,  word[gdtr]			; length 

	push	ecx
	push	esi
	push	edi

	call	mcopy

	pop	edi
	pop	esi
	pop	ecx

;update gdtr register

	mov	ax, 1023			; 1024 - 1
	mov	word[gdtr], ax			; limit = 1023

	mov	eax, gdt_desc
	mov	dword[gdtr+2], eax		; base addr = the start address of GDT address within gdtr

	lgdt	[gdtr]
;------------------------------------------------------------
; enforce the gdt_desc_setting and jump into it
;------------------------------------------------------------
	jmp	SEL_CODE:update

update:

;------------------------------------------------------------
; init others desc in gdt_desc
;------------------------------------------------------------
	call	init_gdt_descs_sys

	call	init_gdt_descs_user

	pop	eax
	pop	ecx
	pop	edi
	pop	esi

	ret

;===============================================================
init_gdt_descs_sys:
;===============================================================
	push	ebp	
	push	esi
	push	edi
	push 	ecx
	push	eax	

	;init desc gdt_desc[5] ~ gdt_desc[13]
	mov	ecx, PROC_SYS_FIRST
	mov	esi, gdt_desc

.1:
	;Init code desc
	mov	ebp, esi
	add	ebp, ecx	
	mov	eax, 0
	mov	ebx, 0xFFFFFFFF
	mov	edi, ATTR_CODE_EXE
	or	edi, ATTR_PRIV_SYS
	call	desc_setting
	add	ecx, 8
	cmp	ecx, PROC_SYS_END
	jae	.2

	;Init data desc
	mov	ebp, esi
	add	ebp, ecx	
	mov	eax, 0
	mov	ebx, 0xFFFFFFFF

	mov	edi, ATTR_DATA_READ_WRITE
	or	edi, ATTR_PRIV_SYS
	call	desc_setting
	add	ecx, 8
	cmp	ecx, PROC_SYS_END
	jae	.2

	;Init stack desc
	mov	ebp, esi
	add	ebp, ecx	
	mov	eax, 0
	mov	ebx, 0xFFFFFFFF
	mov	edi, ATTR_DATA_READ_WRITE
	or	edi, ATTR_PRIV_SYS
	call	desc_setting
	add	ecx, 8
	cmp	ecx, PROC_SYS_END
	jb	.1
	

.2:

	pop	eax
	pop	ecx
	pop	edi
	pop	esi
	pop	ebp

	ret


;===============================================================
init_gdt_descs_user:
;===============================================================
	push	ebp	
	push	esi
	push	edi
	push 	ecx
	push	eax	

	;init desc gdt_desc[14] ~ gdt_desc[128]
	mov	ecx, PROC_USER_FIRST
	mov	esi, gdt_desc

.1:
	;Init code desc
	mov	ebp, esi
	add	ebp, ecx	
	mov	eax, 0
	mov	ebx, 0xFFFFFFFF
	mov	edi, ATTR_CODE_EXE
	or	edi, ATTR_PRIV_USER
	call	desc_setting
	add	ecx, 8
	cmp	ecx, PROC_USER_END
	jae	.2

	;Init data desc
	mov	ebp, esi
	add	ebp, ecx	
	mov	eax, 0
	mov	ebx, 0xFFFFFFFF
	mov	edi, ATTR_DATA_READ_WRITE
	or	edi, ATTR_PRIV_USER
	call	desc_setting
	add	ecx, 8
	cmp	ecx, PROC_USER_END
	jae	.2

	;Init stack desc
	mov	ebp, esi
	add	ebp, ecx	
	mov	eax, 0
	mov	ebx, 0xFFFFFFFF
	mov	edi, ATTR_DATA_READ_WRITE
	or	edi, ATTR_PRIV_USER
	call	desc_setting
	add	ecx, 8
	cmp	ecx, PROC_USER_END
	jb	.1
	

.2:

	pop	eax
	pop	ecx
	pop	edi
	pop	esi
	pop	ebp

	ret
