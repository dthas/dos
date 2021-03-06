;===========================================================================
; hd_info32.asm 
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

%include	"32/hd_info.h"
;===============================================================
hd_info32:
;===============================================================

	call	disp_hd_heading

	call	iden_get

	call	iden_disp

	ret

;===============================================================
iden_get:
;===============================================================

;---------------
;send command
;---------------

;fill cmd
	;mov	byte[cmd32+0], 0
	mov	byte[cmd32+1], 0
	mov	byte[cmd32+2], 0
	mov	byte[cmd32+3], 0
	mov	byte[cmd32+4], 0
	mov	byte[cmd32+5], 0
	;mov	byte[cmd32+6], 0
	mov	byte[cmd32+6], 0xa0
	mov	byte[cmd32+7], HD_IDEN

	call	cmd_send32

;---------------
;get hd info
;---------------

;wait
	;call	wait_1000

;get info(info size: 1 sector)
	xor	eax, eax
	xor	bp, bp
	mov	dx, CBP_PORT0_		;dx=port=0x1f0
	
.0:
	call	in_16_
	mov	word[iden_info+bp],ax
	add	bp, 2

	cmp	bp, 512
	jne	.0	

	ret

;=====================
iden_disp:
;=====================

	push	edi	
	push	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	xor	bp, bp

	call	disp_seqnum_heading

	
;content
	xor	eax, eax
	mov	bp, NUM_SEQ_START
.10:
	mov	bx, word[iden_info+bp]

	mov	ah, 0x7				; fore-color is white
	mov	al, bh
	call	disp_sym32

	mov	ah, 0x7				; fore-color is white
	mov	al, bl
	call	disp_sym32
	
	add 	bp, 2
	cmp	bp, NUM_SEQ_END
	jne	.10

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;-----------------
;type
;-----------------
;head
	call	disp_type_heading

;content
	xor	eax, eax
	mov	bp, TYPE_START
.20:
	mov	bx, word[iden_info+bp]
	
	mov	ah, 0x7				; fore-color is white
	mov	al, bh
	call	disp_sym32

	mov	ah, 0x7				; fore-color is white
	mov	al, bl
	call	disp_sym32

	add 	bp, 2
	cmp	bp, TYPE_END
	jne	.20

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;-----------------
;sector count
;-----------------
	call	disp_seccnt_heading

;content

	xor 	ax, ax
	xor	cx, cx
	mov	bp, NUM_SEC_START
	
	mov	eax, dword[iden_info+bp]
	mov	dx, ax		; dx = lower 16 bit
	mov	cl, 16
	shr	eax,cl
	mov	cx, ax		; cx = higher 16 bit

	mov	ax, cx
	mov	cl, 16
	shl	eax, cl
	mov	ax, dx

	call	disp_num32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;---------------
;lba support
;---------------
;head
	call	disp_lba_heading

;content
	mov	bp, LBA_SUP_START

	xor	ax, ax
	mov	ax, word[iden_info+bp]
	and	ax, 0x200
	cmp	ax, 0
	je	.40
	xor	eax, eax
	mov	al, 89
	jmp	.41
.40:
	xor	eax, eax
	mov	al, 78
.41:
	mov	ah, 0x7	
	call	disp_sym32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;
	
;---------------
;instruction set
;---------------
;head
	call	disp_iset_heading

;content
	mov	bp, INS_SET_START

	xor	ax, ax
	mov	ax, word[iden_info+bp]
	and	ax, 0x400
	cmp	ax, 0
	je	.50
	xor	eax, eax
	mov	al, 89
	jmp	.51
.50:
	xor	eax, eax
	mov	al, 78
.51:
	mov	ah, 0x7	
	call	disp_sym32

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

;-------------
;restore 
;-------------
	
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop	esi
	pop	edi

	ret

;===============================================================
disp_hd_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'H'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'a'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'r'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'd'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'd'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'i'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 's'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'k'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ':'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;


	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret


;===============================================================
disp_seqnum_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'S'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'q'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ':'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret

;===============================================================
disp_type_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'T'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'y'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'p'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ':'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret

;===============================================================
disp_seccnt_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'S'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'c'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 's'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ':'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret

;===============================================================
disp_lba_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'L'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'b'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'a'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ':'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret

;===============================================================
disp_iset_heading:
;===============================================================

	push	eax

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'I'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'S'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 'e'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, 't'
	call	disp_sym32			; 

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ':'
	call	disp_sym32			;

	xor	eax, eax
	mov	ah, 0x7				; fore-color is white
	mov	al, ' '
	call	disp_sym32			;

	pop	eax
		
	ret
