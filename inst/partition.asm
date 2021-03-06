;===========================================================================
; partition.asm
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


;===============
partition:
;===============	
	mov	ax, mesg_part
	mov	cx, MESG_PART_LEN
	mov	dh, 8
	call	disp_msg

	call	partition_main

	mov	ax, mesg_part1
	mov	cx, MESG_PART_LEN
	mov	dh, 9
	call	disp_msg

	ret

;===============
partition_main:
;===============
	
	call	part_main
	call	disp_main

	ret
;===============
part_main:
;===============

	call	fill_data
	call	exec_part

	ret

;===============
;fill hd_info
fill_data:
;===============
;----------------
;primary table
;----------------
	mov	dword[part_tab_sec], 0

	mov	bp, TAB_OFFSET		;offset 0x1BE
			
	mov	byte[hd_info+bp+0],0x0
	mov	byte[hd_info+bp+1],0x1 
	mov	byte[hd_info+bp+2],0x1 
	mov	byte[hd_info+bp+3],0x0
	mov	byte[hd_info+bp+4],0x83
	mov	byte[hd_info+bp+5],0xF
	mov	byte[hd_info+bp+6],0x3F
	mov	byte[hd_info+bp+7],0x13

	mov	dword[hd_info+bp+8],0x3F
	mov	dword[hd_info+bp+12],0x4E81

	add	bp, 16
	mov	byte[hd_info+bp+0],0x0
	mov	byte[hd_info+bp+1],0x0 
	mov	byte[hd_info+bp+2],0x1 
	mov	byte[hd_info+bp+3],0x14
	mov	byte[hd_info+bp+4],0x5
	mov	byte[hd_info+bp+5],0xF
	mov	byte[hd_info+bp+6],0x3F
	mov	byte[hd_info+bp+7],0xCA

	mov	dword[hd_info+bp+8],0x4EC0
	mov	dword[hd_info+bp+12],0x02D090

	mov	bp, TAB_AA55
	mov	word[hd_info+bp],0xAA55
	
	call	exec_part

;----------------
;Logical table
;----------------
	mov	dword[part_tab_sec], 0x4EC0

	mov	bp, TAB_OFFSET		;offset 0x1BE
			
	mov	byte[hd_info+bp+0],0x80
	mov	byte[hd_info+bp+1],0x1 
	mov	byte[hd_info+bp+2],0x1 
	mov	byte[hd_info+bp+3],0x14
	mov	byte[hd_info+bp+4],0xCC
	mov	byte[hd_info+bp+5],0xF
	mov	byte[hd_info+bp+6],0x3F
	mov	byte[hd_info+bp+7],0xCA

	mov	dword[hd_info+bp+8],0x3F
	mov	dword[hd_info+bp+12],0x02D051

	add	bp, 16
	mov	byte[hd_info+bp+0],0x0
	mov	byte[hd_info+bp+1],0x0 
	mov	byte[hd_info+bp+2],0x0 
	mov	byte[hd_info+bp+3],0x0
	mov	byte[hd_info+bp+4],0x0
	mov	byte[hd_info+bp+5],0x0
	mov	byte[hd_info+bp+6],0x0
	mov	byte[hd_info+bp+7],0x0

	mov	dword[hd_info+bp+8],0x0
	mov	dword[hd_info+bp+12],0x0


	mov	bp, TAB_AA55
	mov	word[hd_info+bp],0xAA55
	
	call	exec_part

	ret

;===============
;send command
exec_part:
;===============

;fill cmd
	mov	byte[cmd+1], 0
	mov	byte[cmd+2], 1

	mov	eax, dword[part_tab_sec]
	mov	byte[cmd+3], al

	mov	cl, 8
	shr	eax, cl
	mov	byte[cmd+4], al

	mov	eax, dword[part_tab_sec]
	mov	cl,16
	shr	eax, cl
	mov	byte[cmd+5], al

	mov	eax, dword[part_tab_sec]
	mov	cl, 24
	shr	eax, cl
	and	al, 0xF
	or	al, 0xA0	; (lba_higest & 0xF | 0xA0) = (LOG_TAB_START >>24 & 0xF | 0xA0)
	or	al, 0x40	; lba<<6 = (1<<6)
	
	mov	byte[cmd+6], al
	
	mov	byte[cmd+7], HD_WRITE

	call	cmd_send

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	call	wait_1000

;get info(info size: 1 sector)
	xor	eax, eax
	xor	bp, bp
	mov	dx, CBP_PORT0		;dx=port=0x1f0

	mov	ax, 0x900
	mov	es, ax

.0:
	mov	ax, word[hd_info+bp]
	call	out_16
	add	bp, 2

	cmp	bp, 512
	jne	.0	

	mov	ax, 0xb800
	mov	es, ax

	ret

;===============
disp_main:
;===============

	mov	dword[pos], 2240	; 14 * 160
	
	mov	dword[part_tab_sec], 0

.0:
	mov	word[part_offset], 0
	mov	byte[part_entry_time], 0

.1:
	call	part_get
	call	part_disp

	mov	ax, word[part_offset]
	add	ax, 16
	mov	word[part_offset], ax

	add	byte[part_entry_time], 1
	cmp	byte[part_entry_time], TAB_ENTRY
	jne	.1
	
	mov	eax, dword[part_tab_sec]
	cmp	eax, 0
	jne	.0

	ret

;===============
part_get:
;===============
;---------------
;send command
;---------------

;fill cmd
	mov	byte[cmd+1], 0
	mov	byte[cmd+2], 1

	mov	eax, dword[part_tab_sec]
	mov	byte[cmd+3], al

	mov	cl, 8
	shr	eax, cl
	mov	byte[cmd+4], al

	mov	eax, dword[part_tab_sec]
	mov	cl,16
	shr	eax, cl
	mov	byte[cmd+5], al

	mov	eax, dword[part_tab_sec]
	mov	cl, 24
	shr	eax, cl
	and	al, 0xF
	or	al, 0xA0	; (lba_higest & 0xF | 0xA0) = (LOG_TAB_START >>24 & 0xF | 0xA0)
	or	al, 0x40	; lba<<6 = (1<<6)

	mov	byte[cmd+6], al
	mov	byte[cmd+7], HD_READ

	call	cmd_send

;---------------
;get result
;---------------

;wait
	call	wait_1000

;get info(info size: 1 sector)
	xor	eax, eax
	xor	bp, bp
	mov	dx, CBP_PORT0		;dx=port=0x1f0

	mov	ax, 0x900
	mov	es, ax

.0:
	call	in_16
	mov	word[hd_info+bp],ax
	add	bp, 2

	cmp	bp, 512
	jne	.0	

	mov	ax, 0xb800
	mov	es, ax

	ret
;=====================
part_disp:
;=====================

	push	ebp
	xor	bp, bp

	mov	bx, 0xb800
	mov	es, bx
	mov	bx, [pos]

;---------------
;bootable
;---------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 66
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 111
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

;content
	mov	bp, TAB_OFFSET		;offset 0x1BE
	add	bp, word[part_offset]	;entry 
	add	bp, TAB_BOOT		;offset (0x1BE + 0x0)

	xor	ax, ax
	mov	al, byte[hd_info+bp]
	cmp	al, 0x80
	jne	.40
	mov	al, 89			;yes
	jmp	.41
.40:
	mov	al, 78			; no
.41:
	mov	byte[es:bx], al

	add	bx, 2

;update [inst_part_sec]

	mov	bp, TAB_OFFSET		;offset 0x1BE
	add	bp, word[part_offset]	;entry 
	add	bp, TAB_BOOT		;offset (0x1BE + 0x0)

	xor	ax, ax
	mov	al, byte[hd_info+bp]
	cmp	al, 0x80
	jne	.2

	mov	bp, TAB_OFFSET		;offset 0x1BE
	add	bp, word[part_offset]	;entry 
	add	bp, TAB_SIZE_START	;offset (0x1BE + 0x11)
	mov	eax, dword[hd_info+bp]

	mov	dword[inst_part_sec], eax

;update [inst_part_start]
	mov	bp, TAB_OFFSET		;offset 0x1BE
	add	bp, word[part_offset]	;entry 
	add	bp, TAB_SEC_START	;offset (0x1BE + 0x8)
	mov	eax, dword[hd_info+bp]	;eax = [TAB_SEC_START]
	mov	ebp, dword[part_tab_sec]
	add	eax, ebp
	mov	dword[inst_part_start], eax

.2:
;---------------
;type
;---------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 84
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 121
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 112
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2

;content
	mov	bp, TAB_OFFSET		;offset 0x1BE
	add	bp, word[part_offset]	;entry 
	add	bp, TAB_TYPE	;offset (0x1BE + 0x4)
	
	mov	[pos] , bx

	mov	al, byte[hd_info+bp]
	xor	ah, ah

	call	disp_str

	mov	bx, [pos]

;-----------------
;Start sector
;-----------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 83
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 97
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 114
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 116
	mov	byte[es:bx], al
	add	bx, 2
	
	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2
	
;content

	xor 	ax, ax
	xor	cx, cx
	
	mov	bp, TAB_OFFSET		;offset 0x1BE
	add	bp, word[part_offset]	;entry 
	add	bp, TAB_SEC_START	;offset (0x1BE + 0x8)
	mov	[pos] , bx

	mov	eax, dword[hd_info+bp]
	mov	dx, ax		; dx = lower 16 bit
	mov	cl, 16
	shr	eax,cl
	mov	cx, ax		; cx = higher 16 bit

	xor	ah, ah

	mov	al, ch
	call	disp_str
	mov	al, cl
	call	disp_str
	mov	al, dh
	call	disp_str
	mov	al, dl
	call	disp_str

	mov	bx, [pos]

;update [part_tab_sec]

	mov	al, byte[part_entry_time]
	cmp	al, 1
	jne	.3

	mov	bp, TAB_OFFSET		;offset 0x1BE
	add	bp, word[part_offset]	;entry 
	add	bp, TAB_SEC_START	;offset (0x1BE + 0x8)
	mov	eax, dword[hd_info+bp]

	mov	dword[part_tab_sec], eax

.3:

;-----------------
;Size(sectors)
;-----------------
;head
	xor	ax, ax

	mov	al, 44
	mov	byte[es:bx], al
	add	bx, 2		;comma
	add	bx, 2		;space

	mov	al, 83
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 105
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 122
	mov	byte[es:bx], al
	add	bx, 2

	mov	al, 101
	mov	byte[es:bx], al
	add	bx, 2
	
	mov	al, 58
	mov	byte[es:bx], al
	add	bx, 2
	
;content

	xor 	ax, ax
	xor	cx, cx
	
	mov	bp, TAB_OFFSET		;offset 0x1BE
	add	bp, word[part_offset]	;entry 
	add	bp, TAB_SIZE_START	;offset (0x1BE + 0x12)
	mov	[pos] , bx

	mov	eax, dword[hd_info+bp]
	mov	dx, ax		; dx = lower 16 bit
	mov	cl, 16
	shr	eax,cl
	mov	cx, ax		; cx = higher 16 bit

	xor	ah, ah

	mov	al, ch
	call	disp_str
	mov	al, cl
	call	disp_str
	mov	al, dh
	call	disp_str
	mov	al, dl
	call	disp_str

	mov	bx, [pos]

;-------------
;restore 
;-------------
	
	mov	word[pos], bx

	mov	bx, 0x900
	mov	es, bx
	
	pop	ebp
	
	ret

;=======================================================================
mesg_part:		db	"Partition begin."
mesg_part1:		db	"Partition end..."

part_tab_sec:		dd	0
part_offset:		dw	0
part_entry_time:	db	0

inst_part_sec		dd	0	; the size of partition for installing, which will be used in fs.asm
inst_part_start		dd	0	; the start sector of partition for installing

MESG_PART_LEN		equ	16
;=======================================================================
