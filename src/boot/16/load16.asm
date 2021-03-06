;===========================================================================
; load16.asm 
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


;=======================================================================
load_to_12:
;=======================================================================

	mov	ebp, LOG_SEC_START		; 0x4EFF
	add	ebp, SECTOR_PER_BLOCK		; the start block = super_block = boot_block + 1 = 0 + 1
	mov	ecx, SECTOR_PER_BLOCK		; sector count
	mov	edx, HD_READ			; command

	call	fill_cmd

;------------------------------
;fill super_block_structure
;------------------------------

	xor	ebp, ebp
	mov	dx, CBP_PORT0			; dx=port=0x1f0

.0:
	call	in_16
	mov	word[boot_info+ebp],ax
	add	ebp, 2

	cmp	ebp, SUPER_BLOCK_SIZE		; 48
	jne	.0

;------------------------------------------------
;get inode structure start address
;------------------------------------------------
;the start block = boot_block + super_block + imap_block + bmap_block + 1 = 0 + 1 + 1 + imap_block + bmap_block

	mov	eax, 2				; boot_block + super_block + 1
	add	eax, dword[boot_info + 8]	; imap_block
	add	eax, dword[boot_info + 16]	; bmap_block
	mov	cl, 3
	shl	eax, cl				; eax * 8
	mov	dword[inode_start_sector], eax	; eax = the start block = 0 + 1 + 1 + imap_block + bmap_block
	
;------------------------------------------------------------
;search from inode structure block(block by block)
;------------------------------------------------------------

	mov	eax, LOG_SEC_START		; 0x4EFF
	mov	esi, dword[inode_start_sector]	; get inode structure start sector
	add	esi, eax			; ebp = LOG_SEC_START + inode structure start sector

	xor	eax, eax
	mov	byte[flag_found] , 0
.1:
	cmp	byte[flag_found], 0
	jne	.10

	call	get_file
	add	esi, SECTOR_PER_BLOCK
	jmp	.1

.10:
	call	load_from_harddisk

	ret

;=======================================================================
get_file:
;parm is esi
;=======================================================================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	mov	ebp, esi			; 
	mov	ecx, SECTOR_PER_BLOCK		; sector count
	mov	edx, HD_READ			; command

	call	fill_cmd

	mov	dx, CBP_PORT0			; dx=port=0x1f0
	xor	edi, edi
	xor	ecx, ecx
.1:
	call	in_16
	mov	word[boot_info+ecx],ax
	add	ecx, 2
	cmp	ecx, INODE_SIZE
	jne	.1

	cmp	dword[boot_info], 0
	je	.entry_done	

	xor	bx, bx
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
	add	edi, INODE_SIZE
	cmp	edi, BLOCK_SIZE			;the size of super_block_structure is 48 bytes
	jne	.1

.entry_done:	
	jmp	.end

.found:
	mov	byte[flag_found] , 1
		
.end:
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;=======================================================================
load_from_harddisk:
;from	: harddisk
;to	: KERNEL_BIN_ADDR(0x12000)
;=======================================================================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

;if the inode of krl.bin is bigger than 0x40(>1 block),then re-compute it's start_block and offset
	xor	edx, edx
	xor	ecx, ecx
	mov	eax, dword[boot_info]		; ax = low_16_bit(dword[boot_info])
	mov	edx, eax
	mov	ecx, 16
	shr	edx, cl				; dx = high_16_bit(dword[boot_info])
	mov	cx, 0x40
	div	cx				; dword[boot_info] / 64 = [ax:shuang , dx:yu]
	mov	esi, edx			; esi = edx = offset from the block
	mov	ecx, 3
	shl	eax, cl				; eax = eax * 8
	mov	edx, eax			; save eax
	
;	mov	esi, dword[boot_info]

	sub	esi, 1
	mov	cl, 6
	shl	esi, cl				; esi * 64
	add	esi, 0x1c			; i_block_0

	mov	eax, LOG_SEC_START

	add	eax, edx

	mov	ebp, dword[inode_start_sector]	; get inode structure start sector
	add	ebp, eax			; ebp = LOG_SEC_START + inode structure start sector
	mov	ecx, SECTOR_PER_BLOCK		; sector count
	mov	edx, HD_READ			; command

	call	fill_cmd

	mov	dx, CBP_PORT0			; dx=port=0x1f0
	xor	ecx, ecx
	xor	ebp, ebp

.10:
	call	in_16
	
	cmp	ecx, esi
	jb	.20

	mov	word[i_node + bp + 0x1c], ax
	add	bp, 2
	cmp	bp, 32
	je	.50

.20:	
	add	ecx, 2	
	cmp	ecx, BLOCK_SIZE
	jb	.10

;------------------------------------------------
;load krl.bin to 0x1200:0 
;------------------------------------------------

.50:

	mov	cx, KERNEL_BIN_SEG
	mov	word[cur_seg], cx			; word[cur_seg] = 0x1200
	xor	esi, esi

.60:	
	cmp	esi, 24
	je	.read1lrd

	cmp	esi, 28
	je	.read2lrd
	ja	.end

	mov	edi, dword[i_node + esi + 0x1c]		; edi is the parameter of read_block
	call	read_block

	add	esi, 4
	cmp	dword[i_node + esi + 0x1c], 0
	jne	.60

	jmp	.end

.read1lrd:
	mov	edi, dword[i_node + esi + 0x1c]		; edi is the parameter of read_1lrd
	call	read_1lrd
	jmp	.end	

.read2lrd:
	mov	edi, dword[i_node + esi + 0x1c]		; edi is the parameter of read_1lrd
	call	read_2lrd
	jmp	.end

.end:

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;===============================================================
read_block:
;parameter : edi: the block number to be read into 0x12000
;===============================================================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	mov	ebp, edi			; start sector of krl.bin(passed from the parameter of edi) 
	mov	ecx, SECTOR_PER_BLOCK		; sector count
	mov	edx, HD_READ			; command

	call	fill_cmd

	mov	cx, word[cur_seg]
	mov	es, cx

	mov	dx, CBP_PORT0			; dx=port=0x1f0
	xor	ecx, ecx
.100:
	call	in_16
	mov	word[es:ecx],ax
	add	ecx, 2
	cmp	ecx, BLOCK_SIZE
	jne	.100
	
	mov	eax, BLOCK_SIZE
	mov	cl, 4
	shr	eax, cl				; eax = 4096 / 16 = 256
	mov	cx, word[cur_seg]
	add	cx, ax
	mov	word[cur_seg], cx


	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;===============================================================
read_1lrd:
;parameter : edi: the block number of 1 level block
;===============================================================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

;-----------------------------------------------------------------		
;read 1 level block into 0x42000
;-----------------------------------------------------------------
	mov	ebp, edi			; start sector of 1 level block
	mov	ecx, SECTOR_PER_BLOCK		; sector count
	mov	edx, HD_READ			; command

	call	fill_cmd

	mov	cx, TEMP_SEG_1RD		; load 1 level block into 0x42000
	mov	es, cx

	mov	dx, CBP_PORT0			; dx=port=0x1f0
	xor	ecx, ecx
.0:
	call	in_16
	mov	word[es:ecx],ax
	add	ecx, 2
	cmp	ecx, BLOCK_SIZE
	jne	.0

;-----------------------------------------------------------------		
;read each block number in 1 level block and load it to 0x12000
;-----------------------------------------------------------------
	xor 	ecx, ecx

.10:
	mov	bx, TEMP_SEG_1RD		; load 1 level block into 0x42000
	mov	es, bx

	mov	eax, dword[es:ecx]
	cmp	eax, 0
	je	.end

	mov	edi, eax
	call	read_block			; load the block of krl.bin into 0x12000

	add	ecx, 4
	cmp	ecx, BLOCK_SIZE
	jne	.10
	

.end:

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;===============================================================
read_2lrd:
;parameter : edi: the block number of 2 level block
;===============================================================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

;-----------------------------------------------------------------		
;read 2 level block into 0x52000
;-----------------------------------------------------------------
	mov	ebp, edi			; start sector of 2 level block
	mov	ecx, SECTOR_PER_BLOCK		; sector count
	mov	edx, HD_READ			; command

	call	fill_cmd

	mov	cx, TEMP_SEG_2RD			; load 1 level block into 0x42000
	mov	es, cx

	mov	dx, CBP_PORT0			; dx=port=0x1f0
	xor	ecx, ecx
.0:
	call	in_16
	mov	word[es:ecx],ax
	add	ecx, 2
	cmp	ecx, BLOCK_SIZE
	jne	.0

;-----------------------------------------------------------------------------		
;read each 1 level block number in 2 level block and load it to 0x42000
;-----------------------------------------------------------------------------
	xor 	ecx, ecx

.10:
	mov	bx, TEMP_SEG_2RD			; load 2 level block into 0x52000
	mov	es, bx

	mov	eax, dword[es:ecx]
	cmp	eax, 0
	je	.end

	mov	edi, eax
	call	read_1lrd				; load 1 level block into 0x42000

	add	ecx, 4
	cmp	ecx, BLOCK_SIZE
	jne	.10
	

.end:

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

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

	call	cmd_send

	ret

;=======================================================================
cmd_send:
;=======================================================================

;send write to control block register
	mov	al, 0
	mov	dx, CBTP_PORT0
	call	out_8

;send command block registers
	
	mov	al, byte[cmd+1]
	mov	dx, CBP_PORT1
	call	out_8

	mov	al, byte[cmd+2]
	mov	dx, CBP_PORT2
	call	out_8

	mov	al, byte[cmd+3]
	mov	dx, CBP_PORT3
	call	out_8

	mov	al, byte[cmd+4]
	mov	dx, CBP_PORT4
	call	out_8

	mov	al, byte[cmd+5]
	mov	dx, CBP_PORT5
	call	out_8

	mov	al, byte[cmd+6]
	mov	dx, CBP_PORT6
	call	out_8

	mov	al, byte[cmd+7]
	mov	dx, CBP_PORT7
	call	out_8

	ret

;=======================================================================
out_8:
;(port:dx; value:al)
;=======================================================================
	out	dx, al
	nop
	nop
	nop
	ret
;=======================================================================
in_16:
;(port:dx; value:al)
;=======================================================================
	in	ax, dx
	nop
	nop
	nop
	ret

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

;command block ports(primary)
;------------------------------------------------------
CBP_PORT0	equ	0x1f0		;data
CBP_PORT1	equ	0x1f1		;err/feature
CBP_PORT2	equ	0x1f2		;sector count
CBP_PORT3	equ	0x1f3		;lba low
CBP_PORT4	equ	0x1f4		;lba mid
CBP_PORT5	equ	0x1f5		;lba high
CBP_PORT6	equ	0x1f6		;device/command
CBP_PORT7	equ	0x1f7		;status
;------------------------------------------------------

;control block registers
;------------------------------------------------------
cbr:		db	0		;alternate/device
		db	0		;status/control
;------------------------------------------------------

;control block ports
;------------------------------------------------------
CBTP_PORT0	equ	0X3F6		;alternate/device,status/control
;------------------------------------------------------

;------------------------------------------------------
boot_info:	dd	0	
		dd	0	
		dd	0
		dd	0
		dd	0
		dd	0
		dd	0	
		dd	0
		dd	0
		dd	0
		dd	0
		dd	0	
		dd	0
		dd	0
		dd	0
		dd	0

;------------------------------------------------------
i_node:		dd	0	;i_mode
		dd	0	;i_size
		dd	0	;i_uid
		dd	0	;i_gid
		dd	0	;i_acc_tm
		dd	0	;i_modi_tm
		dd	0	;i_link_count
		dd	0	;i_block_0
		dd	0	;i_block_1
		dd	0	;i_block_2
		dd	0	;i_block_3
		dd	0	;i_block_4
		dd	0	;i_block_5
		dd	0	;i_block_1lvl_ind
		dd	0	;i_block_2lvl_ind
		dd	0	;reserve
				
;------------------------------------------------------

HD_READ		equ	0x20

LOG_SEC_START	equ	0x4EFF	;(0x4EC0+0x3F)

SECTOR_PER_BLOCK	equ	8
BLOCK_SIZE		equ	4096
INODE_SIZE		equ	64	; bytes
SUPER_BLOCK_SIZE	equ	48	; bytes
;-----------------------------------------------------------------------
inode_start_sector	dd	0
flag_found		db	0	; just a flag 
cur_seg			dw	0
cur_disp_es		dw	0	; just for disp
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

