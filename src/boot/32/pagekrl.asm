;===========================================================================
; pagekrl.asm 
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
kernel_page_set:
;===============================================================
	
	push	ebp
	push	ecx
	push 	ebx
	push	eax

	mov	eax, PAGE_TABLE_KERNEL		; from physical address 0x200000(PAGE_TABLE_KERNEL)
	or	eax, PG_EXIST
	or	eax, PG_RW_W
	or	eax, PG_US_USER

	mov	ebp, PAGE_DIR_KERNEL
	mov	ecx, 1024

;-------------------------------------------------------------
;Init page dir
;-------------------------------------------------------------
.0:
	mov	dword[es:ebp], eax
	add	eax, 4096
	add	ebp, 4
	
	sub	ecx, 1
	cmp	ecx, 0
	jne	.0


;-------------------------------------------------------------
;Init page table
;-------------------------------------------------------------

;user space init	
	mov	ebp, PAGE_TABLE_KERNEL_S

	mov	ecx, USER_TABLE_PAGE_COUNT	; 1024 * 1024 / 4 * 3

	xor	eax, eax			; from physical address 0
	or	eax, PG_EXIST
	or	eax, PG_RW_W
	or	eax, PG_US_USER
	
.10:
	mov	dword[es:ebp], eax
	add	eax, 4096
	add	ebp, 4

	sub	ecx, 1
	cmp	ecx, 0
	jne	.10


;kernel space init
	mov	ebp, PAGE_TABLE_KERNEL_K	
	mov	ecx, KERNEL_TABLE_PAGE_COUNT	; 1024 * 1024 / 4 * 1

	xor	eax, eax			; from physical address 0
	or	eax, PG_EXIST
	or	eax, PG_RW_W
	or	eax, PG_US_USER
	or	eax, PG_PG
	or	eax, KERNEL_ADDR		; 0x100000
	
.20:
	mov	dword[es:ebp], eax
	add	eax, 4096
	add	ebp, 4

	sub	ecx, 1
	cmp	ecx, 0
	jne	.20

;-------------------------------------------------------------
;enable page
;-------------------------------------------------------------

	mov	ebx, PAGE_DIR_KERNEL
	mov	cr3, ebx

	mov	ebx, cr0
	or	ebx, 0x80000000
	mov	cr0, ebx

;-------------------------------------------------------------
;page setup suscess message(option)
;-------------------------------------------------------------

	call	page_suscess_disp


	pop	eax
	pop	ebx
	pop	ecx
	pop	ebp
		

	ret


;===============================================================
page_suscess_disp:
;===============================================================

	push	eax
	push	ecx

	xor	eax, eax

	mov	ah, 0x7				; fore-color is white
	mov	al, 'P'
	call	disp_sym32			; 'P'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'a'
	call	disp_sym32			; 'a'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'g'
	call	disp_sym32			; 'g'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 'e'

	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; ' '

	mov	ah, 0x7				; fore-color is white
	mov	al, 's'
	call	disp_sym32			; 's'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 'e'

	mov	ah, 0x7				; fore-color is white
	mov	al, 't'
	call	disp_sym32			; 't'

	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'i'
	call	disp_sym32			; 'i'

	mov	ah, 0x7	
	mov	al, 's'
	call	disp_sym32			; 's'

	mov	ah, 0x7	
	mov	al, ':'
	call	disp_sym32			; ':'

	mov	ah, 0x7	
	mov	al, ' '
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, '0'
	call	disp_sym32			; '0'

	mov	ah, 0x7	
	mov	al, ' '
	call	disp_sym32			; ' '
	
	mov	ah, 0x7	
	mov	al, '-'
	call	disp_sym32			; '-'

	mov	ah, 0x7	
	mov	al, ' '
	call	disp_sym32			; ' '

; target address
	mov	eax, 1024
	mov	ecx, 12
	shl	eax, cl				; the page table size is 4M 

	add	eax, PAGE_TABLE_KERNEL
	sub	eax, 4	

	mov	eax, dword[es:eax]
	call	disp_num32


	pop	ecx
	pop	eax
		
	ret
