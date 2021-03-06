;===========================================================================
; memsize32.asm 
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
mem_size_test32:
; 1) read [x] -> eax
; 2) eax + 1 -> temp1
; 3) write temp1 -> [x]
; 4) read [x] -> temp2
; 5) cmp temp1, temp2(if equal, go on test(go back to 1); 
;		      if not, x is the memory size in bytes) 
;===============================================================

	; counting by double word in memory
	;call	mem_size_normal

	; counting by MB in memory
	call	mem_size_quick

	ret

;===============================================================
mem_size_normal:
; 1) read [x] -> eax
; 2) eax + 1 -> temp1
; 3) write temp1 -> [x]
; 4) read [x] -> temp2
; 5) cmp temp1, temp2(if equal, go on test(go back to 1); 
;		      if not, x is the memory size in bytes)
;
; test by Byte 
;===============================================================

	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	mov	ebp, 0x100000			; test the address above 1M

	;can not test from address 0
	;xor	ebp, ebp			; test the address from 0
	
.0:
	mov	eax, dword[ebp]
	mov	ebx, eax			; save the original value of dword[ebp]
	
	add	eax, 1
	mov	ecx, eax			; save (dword[ebp] + 1)
	
	mov	dword[ebp], eax			; write (dword[ebp] + 1) to dword[ebp]
	
	mov	edx, dword[ebp]			; read (dword[ebp] + 1) to edx
	
	mov	dword[ebp], ebx			; restore dword[ebp]
	
	cmp	edx, ecx
	jne	.10
	
	add	ebp, 4				; counting by double word in memory
	jmp	.0

.10:
	mov	dword[mem_size], ebp

	mov	dword[0xFFFFF0], ebp		; for fetching in mem_manage.c


	call	heading_disp			;
	call	content_disp

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	
	ret



;===============================================================
mem_size_quick:
; 1) read [x] -> eax
; 2) eax + 1 -> temp1
; 3) write temp1 -> [x]
; 4) read [x] -> temp2
; 5) cmp temp1, temp2(if equal, go on test(go back to 1); 
;		      if not, x is the memory size in bytes) 
;
; test by MB
;===============================================================

	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	mov	ebp, 0x100000			; test the address above 1M

	;can not test from address 0
	;xor	ebp, ebp			; test the address from 0
	
.0:
	mov	eax, dword[ebp]
	mov	ebx, eax			; save the original value of dword[ebp]
	
	add	eax, 1
	mov	ecx, eax			; save (dword[ebp] + 1)
	
	mov	dword[ebp], eax			; write (dword[ebp] + 1) to dword[ebp]
	
	mov	edx, dword[ebp]			; read (dword[ebp] + 1) to edx
	
	mov	dword[ebp], ebx			; restore dword[ebp]
	
	cmp	edx, ecx
	jne	.10
	
	add	ebp, 0x100000			; counting by MB in memory
	jmp	.0

.10:
	mov	dword[mem_size], ebp

	mov	dword[0xFFFFF0], ebp		; for fetching in mem_manage.c


	call	heading_disp			;
	call	content_disp

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	
	ret

;===============================================================
heading_disp:
;===============================================================

	push	eax

	xor	eax, eax

	mov	ah, 0x7				; fore-color is white
	mov	al, 'M'
	call	disp_sym32			; 'M'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 'e'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'm'
	call	disp_sym32			; 'm'

	mov	al, ' '
	call	disp_sym32			; ' '

	mov	ah, 0x7				; fore-color is white
	mov	al, 'S'
	call	disp_sym32			; 'S'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'i'
	call	disp_sym32			; 'i'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'z'
	call	disp_sym32			; 'z'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 'e'

	mov	ah, 0x7				; fore-color is white
	mov	al, ':'
	call	disp_sym32			; ':'

	mov	al, ' '
	call	disp_sym32			; ' '

	pop	eax
		
	ret

;=======================================================================
content_disp:
;=======================================================================
	
	push	ecx
	push	eax

	mov	eax, dword[mem_size]
	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, '('
	call	disp_sym32			; '('

	mov	eax, dword[mem_size]
	mov	cl, 20
	shr	eax, cl				; dword[mem_size] / 1M

	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'M'
	call	disp_sym32			; 'M'

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ')'
	call	disp_sym32			; ')'

	pop	eax
	pop	ecx
		
	ret


;-----------------------------------------------------------------------
mem_size		dd	0
;=======================================================================

