;===========================================================================
; load.asm 
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

	org  0h	

	jmp START		


%include	"hd.h"
%include	"format.asm"
%include	"partition.asm"
%include	"fs.h"
%include	"fs.asm"
%include	"copyfile.asm"
%include	"lib.asm"
;========
START:
;========

	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	
	mov	esp, stacktop

	mov	ax, es
	mov	word[cur_disp_es], ax

	mov	ax, mesg_load2
	mov	cx, MESG_LEN1
	mov	dh, 0
	call	disp_msg

	mov	ax, mesg_load3
	mov	cx, MESG_LEN1
	mov	dh, 1
	call	disp_msg

	mov	ax, mesg
	mov	cx, MESG_LEN
	mov	dh, 5
	call	disp_msg

	call	load_main

	mov	ax, word[cur_disp_es]
	mov	es, ax

	mov	ax, mesg_load1
	mov	cx, MESG_LEN
	mov	dh, 22
	call	disp_msg
	
	jmp	$

;========
load_main:
;========
	call 	format
	
	ret



	
;=======================================================================
mesg			db	"Loading........."
mesg_load1		db	"Setup complete.."
mesg_load2		db	"Dthas Setup Information"
mesg_load3		db	"======================="

MESG_LEN		equ	16
MESG_LEN1		equ	23

pos			dw	1920
cur_disp_es		dw	0

;save part info
hd_info			resb 	512
iden_info 		resb 	512
cd_info			resb 	2048
fs_info			resb 	4096
tmp_info		resb	4096
stack			resb 	2048
stacktop:
;=======================================================================
