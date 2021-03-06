;===========================================================================
; boot.asm
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

	org	07c00h

	jmp short START		
	nop	

;========
START:
;========
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 07c00h

	call	clr_sc

	mov	ax, mesg
	mov	cx, MESG_LEN
	mov	dh, 3
	call	disp_msg

	call	read_sect_main

	;jmp	$
	jmp	0900h:0

;========
disp_msg:
;========
	xor	bp, bp
	mov	es, bp
	mov	bp, ax
	mov	ax, 01301h
	mov	bx, 0007h
	mov	dl, 0
	int	10h

	add	word[pos],32
	
	ret

;========
clr_sc:
;========
	mov	word[pos]	,0
	mov	ax, 0600h		
	mov	bx, 0700h		
	mov	cx, 0			
	mov	dx, 0184fh		
	int	10h			
	ret

;===============
read_sect_main:
;===============
	call	get_ptd
	call	get_root_dir
	call	get_dr

	call	load_file

	
	ret

;========
get_ptd:
;========

	mov	eax, 0x10
	mov	cl, 4
      	call    read_sector

	mov	bx, 0x9e
	mov	eax, dword[es:bx]
	mov 	dword[dir_start_sector],eax

	mov	bx, 0xa6
	mov	eax, dword[es:bx]
	mov	dword[dir_size_sector], eax

	ret


;================
get_root_dir:
;================	
	
	xor	eax, eax
	mov	ax, word[dir_start_sector]

	mov	cl, 4
	call    read_sector
	
;edx=total length; edi=each entry length;

	mov	dx, 0
	mov	ds, dx

	xor	edx, edx

.0:
	xor	edi, edi			;start an entry
	xor	ecx, ecx
	mov	cl, byte[es:edx]		;edi = entry length

	cmp	cl, 0

	je	.is_next_sector	

	mov	edi, ecx

	mov	eax, edx
	add	eax, 0x20
	mov	cl, byte[es:eax]
	mov 	byte[file_name_len], cl		;get file name length

	cmp	cl, BOOT_DIR_LEN		
	jne	.3

	mov	eax, edx
	add	eax, 2
	push	edi
	mov	edi, dword[es:eax]		;get file start sector
	mov	dword[file_start_sector], edi	 
	pop	edi

	mov	eax, edx
	add	eax, 0xa
	mov	ecx, dword[es:eax]		;ecx is counted by bytes
	mov	dword[file_len], ecx

.1:
	xor	ax, ax	
	xor	bp, bp
	mov	ecx, edx			;start to compare "load.bin",ecx = temp(total)
	add	ecx, 0x21	
	mov	si, boot_dir_name
	
.2:	
	mov	al, byte[ds:si]
	cmp	byte[es:ecx], al
	jne	.3
	add	si, 1
	add	bp, 1
	add	ecx,1

	cmp	bp, BOOT_DIR_LEN
	je	.found_root_dir	
	jmp	.2

.3:	
	add	edx, edi
	jmp	.0

.is_next_sector:	
	mov	eax, edx
	and	eax, 0x800
	add	eax, 0x800
	mov	edx, eax
	cmp	edx, dword[dir_size_sector]
	jb	.0
	
.no_found_root_dir:
	mov	ax, mesg5
	mov	cx, MESG_LEN
	mov	dh, 3
	call	disp_msg
	jmp	.4
	
.found_root_dir:
	mov	ax, mesg6
	mov	cx, MESG_LEN
	mov	dh, 4
	call	disp_msg

.4:
	ret



;================
get_dr:
;================	
	mov	eax, dword[file_len]
	mov	dword[dir_size_sector], eax
	mov	cl, 0xb
	shr	eax, cl
	mov	cl, al

	xor	eax, eax	
	mov	ax, word[file_start_sector]
	
	call    read_sector
	
;edx=total length; edi=each entry length;

	mov	dx, 0
	mov	ds, dx
	
	xor	edx, edx

.0:
	xor	edi, edi			;start an entry
	xor	ecx, ecx
	mov	cl, byte[es:edx]		;edi = entry length

	cmp	cl, 0
	
	je	.is_next_sector	

	mov	edi, ecx

	mov	eax, edx
	add	eax, 0x20
	mov	cl, byte[es:eax]
	mov 	byte[file_name_len], cl		;get file name length

	cmp	cl, LOAD_FILE_NAME_LEN		
	jne	.3

	mov	eax, edx
	add	eax, 2
	push	edi
	mov	edi, dword[es:eax]		;get file start sector
	mov	dword[file_start_sector], edi	 
	pop	edi

	mov	eax, edx
	add	eax, 0xa
	mov	ecx, dword[es:eax]		;ecx is counted by bytes,no to change to sectors
	mov	eax, ecx
	mov	cl, 0xb				;should be 2kb/sector
	shr	eax, cl
	add	eax, 1
	mov 	dword[file_len], eax		;get file length

.1:
	xor	ax, ax	
	xor	bp, bp
	mov	ecx, edx			;start to compare "load.bin",ecx = temp(total)
	add	ecx, 0x21	
	mov	si, load_name
	
.2:	
	mov	al, byte[ds:si]
	cmp	byte[es:ecx], al
	jne	.3
	add	si, 1
	add	bp, 1
	add	ecx,1

	cmp	bp, LOAD_FILE_NAME_LEN
	je	.found_file	
	jmp	.2

.3:	
	add	edx, edi
	jmp	.0

.is_next_sector:	
	mov	eax, edx
	and	eax, 0x800
	add	eax, 0x800
	mov	edx, eax
	cmp	edx, dword[dir_size_sector]
	jb	.0
	
.no_found_file:
	mov	ax, mesg3
	mov	cx, MESG3_LEN
	mov	dh, 5
	call	disp_msg
	jmp	.4
	
.found_file:
	mov	ax, mesg4
	mov	cx, MESG4_LEN
	mov	dh, 6
	call	disp_msg

.4:
	ret

;============
load_file:
;============

	mov	eax, dword[file_start_sector]
	mov	ecx, dword[file_len]
	
	call    read_sector
	ret


;============
read_sector:
;
;parm: eax, cl
;============
	mov   	bx, 0900h               ; Reading to segment 0x900
   	mov   	es, bx
   	xor   	bx, bx                  ; Offset 0

	mov	byte [dap +  0], 0x10
	mov	byte [dap +  1], 0	
	mov	byte [dap +  2], cl
	mov	byte [dap +  3], 0
	mov	word [dap +  4], 0
	mov	word [dap +  6], 0x900
	mov	dword[dap +  8], eax
	mov	dword[dap + 12], 0      	

	mov   	dl, 0e0h
	mov   	ah, 042h

      	mov	si, dap
      	int   	13h  

      	ret

;========
disp_str:
;========
	push	eax
	push	ebx
	push	ecx
	push	edx

	mov	bx, 0xb800
	mov	es, bx
	mov	bx, [pos]

	mov	edx,eax
	mov	cl,0x10
	shr	edx,cl
	mov	cl, 0x10
.1:	
	div	cl
	cmp	al, 0
	je	.3
	cmp	al, 9
	ja	.2

;0~9
	add	al, 0x30
	mov	byte[es:bx],al
	add	bx, 2
	mov	al, ah
	mov	ah, 0
	jmp	.1

;a~f
.2:
	add	al, 0x57
	mov	byte[es:bx],al
	add	bx, 2
	mov	al, ah
	mov	ah, 0
	jmp	.1


.3:
	cmp	ah, 9
	ja	.4

	add	ah, 0x30
	mov	byte[es:bx],ah
	add	bx, 2
	jmp	.5

.4:
	add	ah, 0x57
	mov	byte[es:bx],ah
	add	bx, 2

.5:
	mov	[pos], bx

	pop	edx
	pop	ecx
	pop	ebx
	pop	eax
		
	ret



	

;=======================================================================
dap:			dw	0;	0x10		; [ 0] Packet size in bytes.
			dw	0;	0x4		; [ 2] Nr of blocks to transfer.
			dw	0;	0		; [ 4] Offset
			dw	0;	0x900		; [ 6] Segment
			dd	0;	0x63		; [ 8] start sector number in iso.
			dd	0;	0		; [12] Block number
;-----------------------------------------------------------------------

mesg			db	"Booting........."
mesg1			db	"no found dir! "
mesg2			db	"found dir! "
mesg3			db	"no found file "
mesg4			db	"found file "
mesg5			db	"no find root dir"
mesg6			db	"find root dir..."
boot_dir_name		db	"inst"
load_name		db	"load.bin"
pos			dw	0
dir_start_sector	dw	0
dir_size_sector		dw	0
pt_start_sector		dd	0
file_start_sector	dd	0
file_len		dd	0
file_name_len		db	0

BOOT_DIR_LEN		equ	4		; the length of the name of inst is 4
LOAD_FILE_NAME_LEN	equ	8
MESG_LEN		equ	16
MESG1_LEN		equ	13
MESG2_LEN		equ	10
MESG3_LEN		equ	13
MESG4_LEN		equ	10
;=======================================================================

times 	2046-($-$$)	db	0
