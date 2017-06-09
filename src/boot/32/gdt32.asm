;===========================================================================
; gdt32.asm
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

[SECTION .gdt32]

ALIGN	32

[BITS	32]

CODE32_START:

	mov	ax, SEL_DISP
	mov	gs, ax

	mov	ax, SEL_DATA
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	ss, ax
	mov	esp, stacktop32

	;welcome message
	mov	word[pos32], 0				; from the 4th line, (80*0)*2
	call	load_into_32_disp
	
	;= message
	mov	bl, '='
	mov	ecx, 32					; 32 of '='
	mov	word[pos32], 160			; from the 4th line, (80*1)*2
	call	limit_disp

	;hardware information display(option)
	call	hw_info_main32


	mov	word[pos32], 800			; from the 5th line, (80*5)*2
	call	kernel_page_set

	mov	word[pos32], 320			; from the 2th line, (80*2)*2
	call	load_krl_12_to_10
	call	load_suscess_disp
	
;------------------------------------------------
;jmp into kernel(0x100000)
;------------------------------------------------
	
	jmp	SEL_CODE:KERNEL_ADDR_V


;===============================================================
load_krl_12_to_10:
;(load kernel.bin from KERNEL_BIN_ADDR to KERNEL_ADDR)
;from	: KERNEL_BIN_ADDR(0x12000)
;to	; KERNEL_ADDR(0x100000)
;===============================================================

	mov	esi, KERNEL_BIN_ADDR			; 0x12000

	mov	ebp, 24
	add	ebp, esi
	mov	eax, dword[es:ebp]
	mov	dword[prog_entry], eax			; default is 0

	mov	ebp, 28
	add	ebp, esi	
	mov	eax, dword[es:ebp]
	mov	dword[prog_header_offset], eax		; default is 0x34

	mov	ebp, 42
	add	ebp, esi
	mov	ax, word[es:ebp]
	mov	word[prog_header_size], ax		; default is 0x20

	mov	ebp, 44
	add	ebp, esi
	mov	ax, word[es:ebp]
	mov	word[prog_header_num], ax		; how many entries in program header table

	mov	word[prog_header_count], 0	

.0:
;-----------------------------------------------------------------------------------
;formula:
;	prog_start_addr = file_offset
;	prog_size	= file_length
;	targe_address	= vir_addr
;-----------------------------------------------------------------------------------
	mov	esi, KERNEL_BIN_ADDR			; 0x12000

	mov	ebp, dword[prog_header_offset]
	xor	eax, eax
	mov	ax, word[prog_header_size]
	mov	cx, word[prog_header_count]
	mul	cl					; eax = prog_header_size * prog_header_count
	add 	eax, ebp				; eax = current program header start address
	mov	dword[cur_prog_header_offset], eax

	mov	ebp, 4
	add	ebp, dword[cur_prog_header_offset]
	add	ebp, esi
	mov	ebx, dword[es:ebp]			; ebx = file_offset
	mov	dword[file_offset], ebx

	mov	ebp, 8
	add	ebp, dword[cur_prog_header_offset]
	add	ebp, esi
	mov	ebx, dword[es:ebp]			; ebx = vir_addr
	mov	dword[vir_addr], ebx

	mov	ebp, 16
	add	ebp, dword[cur_prog_header_offset]
	add	ebp, esi
	mov	ebx, dword[es:ebp]
	mov	dword[file_length], ebx			; ebx = file_length

	mov	eax, dword[file_length]
	mov	dword[prog_size], eax

	mov	eax, dword[file_offset]
	add	eax, esi
	mov	dword[prog_start_addr], eax

	mov	edi, dword[vir_addr]			; target address

	mov	esi, dword[prog_start_addr]
	mov	ecx, dword[prog_size]
	
	xor	edx, edx		
.1:
	mov	al, byte[es:esi]

	mov	byte[edi], al

	add	esi, 1
	add	edi, 1
	
	add	edx, 1
	cmp	edx, ecx
	jb	.1

	xor	ebp, ebp
	mov	bp, word[prog_header_count]
	add	bp, 1
	mov	word[prog_header_count], bp

	cmp	bp, word[prog_header_num]
	jne	.0

	ret

;===============================================================
load_suscess_disp:
;===============================================================

	push	eax

	xor	eax, eax

	mov	ah, 0x7				; fore-color is white
	mov	al, 'K'
	call	disp_sym32			; 'K'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 'e'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'r'
	call	disp_sym32			; 'r'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'n'
	call	disp_sym32			; 'n'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 'e'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'l'
	call	disp_sym32			; 'l'

	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; ' '

	mov	ah, 0x7				; fore-color is white
	mov	al, 'i'
	call	disp_sym32			; 'i'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'n'
	call	disp_sym32			; 'n'

	mov	ah, 0x7	
	mov	al, ' '
	call	disp_sym32			; ' '

	mov	eax, KERNEL_ADDR
	call	disp_num32



	pop	eax
		
	ret


;===============================================================
load_into_32_disp:
;===============================================================

	push	eax

	xor	eax, eax

	mov	ah, 0x7				; fore-color is white
	mov	al, 'L'
	call	disp_sym32			; 'K'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'o'
	call	disp_sym32			; 'e'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'a'
	call	disp_sym32			; 'r'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'd'
	call	disp_sym32			; 'n'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'i'
	call	disp_sym32			; 'e'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'n'
	call	disp_sym32			; 'l'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'g'
	call	disp_sym32			; ' '

	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			; 'i'

	mov	ah, 0x7				; fore-color is white
	mov	al, 'i'
	call	disp_sym32			; 'n'

	mov	ah, 0x7	
	mov	al, 'n'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 't'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'o'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, ' '
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, '3'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, '2'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, '-'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'b'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'i'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 't'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, ' '
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'p'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'r'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'o'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 't'
	call	disp_sym32			; ' '
	
	mov	ah, 0x7	
	mov	al, 'e'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'c'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 't'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, ' '
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'm'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'o'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'd'
	call	disp_sym32			; ' '

	mov	ah, 0x7	
	mov	al, 'e'
	call	disp_sym32			; ' '

	pop	eax
		
	ret

;===============================================================
limit_disp:
; param1: ecx is number
; param2: bl is the symbol
;===============================================================

	push	eax
	
	xor	eax, eax
	
	mov	al, bl
	mov	ah, 0x7				; fore-color is white

.0:
	call	disp_sym32			; '='

	sub	ecx, 1
	cmp	ecx, 0
	jne	.0

	pop	eax
		
	ret


[SECTION .data32]

ALIGN	32

;-----------------------------------------------------------------------
prog_entry		dd	0
prog_header_offset	dd	0
elf_header_size		dd	0
prog_header_size	dw	0
prog_header_num		dw	0
prog_header_count	dw	0
prog_start_addr		dd	0
prog_size		dd	0
file_offset		dd	0
file_length		dd	0
vir_addr		dd	0
cur_prog_header_offset	dd	0

pos32			dw	0
;=======================================================================
