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
	jmp	short	START
;=======================================================================
skip_head_mesg		db	"Skip head............................."
;=======================================================================


;================
START:
;================
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, 07c00h	

	mov	esi, 0x5F4F		; DATA_SEC_START

.0:
	call	get_file
	add	esi, 8			; SECTOR_PER_BLOCK
	jmp	.0	
		

;===============================================================
fill_cmd:
;parameter : ebp: the start sector ; ecx: sector count; edx: command
;===============================================================

;fill cmd
	mov	byte[cmd+1], 0	; Features
	mov	byte[cmd+2], cl	; Count

	mov	eax, ebp	; eax : the start sector
	mov	byte[cmd+3], al	; LBA low

	mov	cl, 8
	shr	eax, cl
	mov	byte[cmd+4], al	; LBA middle

	mov	eax, ecx
	mov	cl,16
	shr	eax, cl
	mov	byte[cmd+5], al	; LBA high

	mov	eax, ecx
	mov	cl, 24
	shr	eax, cl
	and	al, 0xF
	or	al, 0xA0	; (lba_higest & 0xF | 0xA0) = (LOG_TAB_START >>24 & 0xF | 0xA0)
	or	al, 0x40	; lba<<6 = (1<<6)
	
	mov	byte[cmd+6], al	; Device
	
	mov	byte[cmd+7], dl	; Command

;------------------------	
;cmd_send:
;------------------------

;send write to control block register
	mov	al, 0
	mov	dx, 0X3F6		;CBTP_PORT0
	out	dx, al			;call	out_8
	;nop				

;send command block registers
	
	mov	al, byte[cmd+1]
	mov	dx, 0x1f1		;CBP_PORT1
	out	dx, al			;call	out_8
	
	mov	al, byte[cmd+2]
	mov	dx, 0x1f2		;CBP_PORT2
	out	dx, al			;call	out_8
	
	mov	al, byte[cmd+3]
	mov	dx, 0x1f3		;CBP_PORT3
	out	dx, al			;call	out_8
	
	mov	al, byte[cmd+4]
	mov	dx, 0x1f4		;CBP_PORT4
	out	dx, al			;call	out_8
	
	mov	al, byte[cmd+5]
	mov	dx, 0x1f5		;CBP_PORT5
	out	dx, al			;call	out_8
	
	mov	al, byte[cmd+6]
	mov	dx, 0x1f6		;CBP_PORT6
	out	dx, al			;call	out_8
	
	mov	al, byte[cmd+7]
	mov	dx, 0x1f7		;CBP_PORT7
	out	dx, al			;call	out_8
	
	ret

;================
get_file:
;parm is esi
;================
	push	esi
	
	mov	ebp, esi			; 
	mov	ecx, 8				; SECTOR_PER_BLOCK, sector count
	mov	edx, 0x20			; HD_READ, command

	call	fill_cmd

	mov	dx, 0x1f0			; CBP_PORT0

	xor	edi, edi
	xor	ecx, ecx
.1:
	in	ax, dx				; call	in_16	
	cmp	ecx, 15				; 4(inode_id) + 11(filename)
	ja	.2
	mov	word[boot_info+ecx],ax
.2:	
	add	ecx, 2
	cmp	ecx, 64				; INODE_SIZE
	jne	.1

	cmp	dword[boot_info], 0
	je	.entry_done	

	xor	esi, esi
.10:
	mov	bh, byte[boot_info + esi + 4]
	cmp	bh, byte[filename + esi]
	jne	.20
	add	esi, 1
	cmp	esi, FILE_LEN
	je	.found
	jne	.10

.20:
	xor	ecx, ecx
	add	edi, 64					; INODE_SIZE
	cmp	edi, 4096				; BLOCK_SIZE, the size of super_block_structure is 48 bytes
	jne	.1

.entry_done:

	jmp	.end

.found:

	jmp	load_file
	
.end:
	pop	esi
	ret

;================
load_file:
;================
	mov	esi, dword[boot_info]
	sub	esi, 1
	mov	cl, 6
	shl	esi, cl				; esi * 64
	add	esi, 0x1c			; i_block_0

;------------------------------------------------
;get inode structure start address
;------------------------------------------------
;the start block = boot_block + super_block + imap_block + bmap_block + 1 = 0 + 1 + 1 + imap_block + bmap_block

	mov	ebp, 0x4F4F			; 0x4F4F = 0x4EFF + 0x50
	mov	ecx, 24				; SECTOR_PER_BLOCK, sector count(read 3 block)
	mov	edx, 0x20			; HD_READ, command

	call	fill_cmd

	xor	ecx, ecx
	mov	dx, 0x1f0			; CBP_PORT0
.0:
	in	ax, dx				; call	in_16
	;nop
	
	cmp	ecx, esi
	je	.10
	
	add	ecx, 2	
	cmp	ecx, 0x3000			; 3 * BLOCK_SIZE(read 3 block)
	jne	.0

.10:
	xor	ebp, ebp
	mov	bp,  ax				; start sector of load.bin 
	mov	ecx, 24				; SECTOR_PER_BLOCK, sector count(read 3 block)
	mov	edx, 0x20			; HD_READ, command

	call	fill_cmd

	mov	cx, 0x900
	mov	es, cx

	xor	ecx, ecx
	mov	dx, 0x1f0			; CBP_PORT0
.20:
	in	ax, dx				; call	in_16
	mov	word[es:ecx],ax
	add	ecx, 2
	cmp	ecx, 0x3000			; 3 * BLOCK_SIZE(read 3 block)
	jne	.20



;------------------------------------------------
;jmp load.bin
;------------------------------------------------
	jmp	0900h:0
	
;=======================================================================
mesg			db	"Boot"
filename		db	"bloadkr.bin"

MESG_LEN		equ	5
FILE_LEN		equ	11
;=======================================================================
;command block registers
;------------------------------------------------------
cmd:		db	0	;data
		db	0	;err/feature
		db	0	;sector count
		db	0	;lba low
		db	0	;lba mid
		db	0	;lba high
		db	0	;device
		db	0	;status/command
;------------------------------------------------------
boot_info:	dd	0	
		dd	0	
		dd	0
;------------------------------------------------------
;=======================================================================
times 	446-($-$$)	db	0
;dw 	0xaa55				
