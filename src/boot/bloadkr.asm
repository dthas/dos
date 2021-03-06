;===========================================================================
; bloadkr.asm 
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
[BITS	16]

	jmp	START

;=======================================================================
mesg			db	"Loading into 16-bit real mode..."
mesg1			db	"Kernel has been put into 0x12000"
mesg2			db	"================================"
filename		db	"akrl.bin"
FILE_LEN		equ	7
MESG_LEN		equ	32
pos			dw	1920
;=======================================================================

;========
START:
;========
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	esp, stacktop

	mov	word[cur_disp_es], ax			; word[cur_disp_es] is in load16.asm

	call	clr_sc					; the function is in loadlib.asm 

	mov	ax, word[cur_disp_es]
	mov	es, ax
	mov	ax, mesg
	mov	cx, MESG_LEN
	mov	dh, 0
	call	disp_msg				; the function is in loadlib.asm 

	mov	ax, mesg2
	mov	cx, MESG_LEN
	mov	dh, 1
	call	disp_msg				; the function is in loadlib.asm 


	call	hw_info_main16

	call	load_to_12				; the function is in load16.asm
	
	mov	ax, word[cur_disp_es]			; word[cur_disp_es] is in load16.asm
	mov	es, ax
	mov	ax, mesg1
	mov	cx, MESG_LEN
	mov	dh, 2
	call	disp_msg

	call	clr_sc

	call	gdt16_main				; the function is in gdt16.asm
	
;-----------------------------------------------------------------------
; for lib in 16-bit
;-----------------------------------------------------------------------
%include	"16/loadlib.asm"

;-----------------------------------------------------------------------
; for hardware information in 16-bit real mode
;-----------------------------------------------------------------------
%include	"16/hw_info16.asm"

;-----------------------------------------------------------------------
; for loading kernel.bin into 0x12000
;-----------------------------------------------------------------------
%include	"16/load16.asm"

;-----------------------------------------------------------------------
; for gdt descriptor setting in bit 16
;-----------------------------------------------------------------------
%include	"loadkr.h"
%include	"16/gdt16.asm"


cmos_info		resb	64
;-----------------------------------------------------------------------
; stack used in loadkr.bin , which is in bit 16
;-----------------------------------------------------------------------
stack			resb 	512
stacktop:
;=======================================================================

[BITS	32]
;-----------------------------------------------------------------------
; All the code is in bit 32. 
; for moving the code within kernel.bin from 0x12000 to 0x100000
;-----------------------------------------------------------------------
%include	"32/gdt32.asm"

;-----------------------------------------------------------------------
; for testing the memory size
;-----------------------------------------------------------------------
%include	"32/memsize32.asm"

;-----------------------------------------------------------------------
; for displaying hardware information
;-----------------------------------------------------------------------
%include	"32/hw_info32.asm"

;-----------------------------------------------------------------------
; for setup kernel page
;-----------------------------------------------------------------------
%include	"32/page.h"
%include	"32/pagekrl.asm"

;-----------------------------------------------------------------------
; for lib in 32-bit
;-----------------------------------------------------------------------
%include	"32/loadlib32.asm"

iden_info 		resb 	512
stack32			resb 	256
stacktop32:
;=======================================================================
