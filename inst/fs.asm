;===========================================================================
; fs.asm 
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
;
; boot_block + super_block + imap_block + bmap_block + inodes + blocks
;

;build_super_block
;build_inode_map
;build_block_map
;build_inodes
;add_root_dir
;fill_cmd
;add_inode_map
;add_block_map
;add_inode
;add_block
;add_entry
;set_1rdblock
;set_2rdblock
;is_dir
;zero_dir_ent
;zero_fs_info
;zero_tmp_info
;zero_cd_info




;=====================
filesys:
;=====================

	mov	ax, es
	mov	word[cur_disp_es], ax

	mov	ax, mesg_fs
	mov	cx, MESG_FS_LEN
	mov	dh, 10
	call	disp_msg

	call	filesys_main

	mov	ax, word[cur_disp_es]
	mov	es, ax

	mov	ax, mesg_fs1
	mov	cx, MESG_FS_LEN
	mov	dh, 11
	call	disp_msg

	ret

;=====================
filesys_main:
;=====================

	call	fs_main
		
	ret

;=====================
fs_main:
;=====================

	call	build_super_block
	call	build_inode_map
	call	build_block_map
	call	build_inodes

	call	add_root_dir
	
	ret

;=====================
build_super_block:
;=====================
;---------------------------------
;set each element of super_block 
;---------------------------------
	xor 	eax, eax
	xor	ecx, ecx

;num_inode
	mov	eax, NUM_INODE_MAX_PER_PART
	mov	dword[num_inode], eax

;num_inode_blocks
	mov	eax, NUM_INODE_MAX_PER_PART
	mov	cl, 6
	shl	eax, cl
	mov	cl, 12
	shr	eax, cl		; NUM_INODE_MAX_PER_PART * 64bytes(per inode) / 4096bytes(per block) = 512 blocks 
	mov	dword[num_inode_blocks], eax

;num_imap_blocks
	mov	eax, NUM_INODE_MAX_PER_PART
	mov	cl, 15
	shr	eax, cl
	add	eax, 1		; NUM_INODE_MAX_PER_PART / 4096(bytes/blocks) / 8(bits/per byte) + 1
	mov	dword[num_imap_blocks], eax

;num_blocks
	mov	eax, dword[inst_part_sec]
	mov	cl , 3
	shr	eax, cl			;[inst_part_sec] / 8 = num_sectors / 8 = num_blocks(each blocks is 4kb)
	mov	dword[num_blocks], eax

;num_bmap_blocks
	mov	eax, dword[inst_part_sec]
	mov	cl, 15
	shr	eax, cl
	add	eax, 1			;[inst_part_sec] / 4096(bytes/blocks) / 8(bits/per byte) + 1
	mov	dword[num_bmap_blocks], eax

;block_size
	mov	eax, BLOCK_SIZE
	mov	dword[block_size], eax

;inode_size
	mov	eax, INODE_SIZE
	mov	dword[inode_size], eax

;flag_magic
	mov	eax, MAGIC_CODE
	mov	dword[flag_magic], eax

;n_1st_dbblock
	xor	eax, eax
	add	eax, 1				; 0:boot block; 1:superblock
	add 	eax, dword[num_imap_blocks]	;
	add 	eax, dword[num_bmap_blocks]	;
	add	eax, dword[num_inode_blocks]	;
	add	eax, 1				;
	mov	dword[n_1st_dbblock], eax	; n_1st_dbblock = 0 + 1 + num_imap_blocks + num_bmap_blocks + num_inode_blocks + 1

;root_inode
	mov	eax, ROOT_DIR_INODE
	mov	dword[root_inode] , eax

;down_mnt_inode
	xor	eax, eax
	mov	dword[down_mnt_inode], eax

;up_mnt_inode
	xor	eax, eax
	mov	dword[up_mnt_inode], eax

;---------------------------------
;fill super_block structure 
;---------------------------------

	mov	bx, word[load_seg]
	mov	es, bx

	mov	eax, dword[num_inode]
	mov	dword[super_blk + 0],eax 
	mov	dword[fs_info + 0], eax

	mov	eax, dword[num_inode_blocks]
	mov	dword[super_blk + 4],eax 
	mov	dword[fs_info + 4], eax

	mov	eax, dword[num_imap_blocks]
	mov	dword[super_blk + 8],eax 
	mov	dword[fs_info + 8], eax

	mov	eax, dword[num_blocks]
	mov	dword[super_blk + 12],eax 
	mov	dword[fs_info + 12], eax

	mov	eax, dword[num_bmap_blocks]
	mov	dword[super_blk + 16],eax 
	mov	dword[fs_info + 16], eax

	mov	eax, dword[block_size]
	mov	dword[super_blk + 20],eax 
	mov	dword[fs_info + 20], eax

	mov	eax, dword[inode_size]
	mov	dword[super_blk + 24],eax 
	mov	dword[fs_info + 24], eax

	mov	eax, dword[flag_magic]
	mov	dword[super_blk + 28],eax 
	mov	dword[fs_info + 28], eax

	mov	eax, dword[n_1st_dbblock]
	mov	dword[super_blk + 32],eax 
	mov	dword[fs_info + 32], eax

	mov	eax, dword[root_inode]
	mov	dword[super_blk + 36],eax 
	mov	dword[fs_info + 36], eax

	mov	eax, dword[down_mnt_inode]
	mov	dword[super_blk + 40],eax 
	mov	dword[fs_info + 40], eax

	mov	eax, dword[up_mnt_inode]
	mov	dword[super_blk + 44],eax
	mov	dword[fs_info + 44], eax 

;call fill cmd	
	mov	ebp, dword[inst_part_start]	; the start sector

	add	ebp, SECTOR_PER_BLOCK		; the start block = super_block = boot_block + 1 = 0 + 1
	mov	ecx, SECTOR_PER_BLOCK		; sector count
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	call	wait_1000

;
	xor	eax, eax
	xor	bp, bp
	mov	dx, CBP_PORT0		;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax

.1000:
	mov	ax, word[fs_info+bp]
	call	out_16
	add	bp, 2

	cmp	bp, BLOCK_SIZE
	jne	.1000	

	
	ret

;=====================
build_inode_map:
;=====================
;----------------
;fill command 
;----------------

	mov	bx, word[load_seg]
	mov	es, bx
	

;call fill cmd	
	xor	eax, eax
	add	eax, 2				; the start block = boot_block + super_block + 1 = 0 + 1 + 1
	mov 	cl, SECTOR_PER_BLOCK
	mul	cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 
	mov	ebp, eax

	mov	eax, dword[num_imap_blocks]
	mov	cl, SECTOR_PER_BLOCK
	mul	cl				; sector count = eax = num_imap_blocks * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;file fs_info
	xor	bp, bp
	mov	ax, 0x0

.1001:
	mov	[fs_info + bp], ax
	add	bp, 2
	cmp	bp, BLOCK_SIZE
	jne	.1001


;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	call	wait_1000

	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax

.1002:
	xor	bp, bp
.1010:
	mov	ax, word[fs_info+bp]
	call	out_16
	add	bp, 2

	cmp	bp, BLOCK_SIZE
	jne	.1010	
	
	add	ecx, 1
	cmp	ecx, dword[num_imap_blocks]

	jne	.1002
	
	ret

;=====================
build_block_map:
;=====================
;--------------
;fill command
;--------------

	mov	bx, word[load_seg]
	mov	es, bx
	
;call fill cmd	
	xor	eax, eax
	add	eax, 2				
	add	eax, dword[num_imap_blocks]	; the start block = boot_block + super_block + num_imap_blocks + 1 = 0 + 1 + num_imap_blocks + 1
	mov 	cl, SECTOR_PER_BLOCK
	mul	cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 
	mov	ebp, eax

	mov	eax, dword[num_bmap_blocks]
	mov	cl, SECTOR_PER_BLOCK
	mul	cl				; sector count = eax = num_bmap_blocks * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;file fs_info
	xor	bp, bp
	mov	ax, 0x0

.1011:
	mov	[fs_info + bp], ax
	add	bp, 2
	cmp	bp, BLOCK_SIZE
	jne	.1011


;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	call	wait_1000

	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax

.1012:
	xor	bp, bp
.1020:
	mov	ax, word[fs_info+bp]
	call	out_16
	add	bp, 2

	cmp	bp, BLOCK_SIZE
	jne	.1020	
	
	add	ecx, 1
	cmp	ecx, dword[num_bmap_blocks]

	jne	.1012

	ret

;=====================
build_inodes:
;=====================
;--------------
;fill command
;--------------

	mov	bx, word[load_seg]
	mov	es, bx
	
;call fill cmd	
	xor	eax, eax
	add	eax, 2				
	add	eax, dword[num_imap_blocks]	
	add	eax, dword[num_bmap_blocks]	; the start block = boot_block + super_block + num_imap_blocks + num_bmap_blocks + 1 = 0 + 1 + num_imap_blocks + num_bmap_blocks + 1
	mov 	cl, SECTOR_PER_BLOCK
	mul	cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 
	mov	ebp, eax

	mov	eax, dword[num_inode_blocks]
	mov	cl, SECTOR_PER_BLOCK
	mul	cl				; sector count = eax = num_bmap_blocks * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;file fs_info
	xor	bp, bp
	mov	ax, 0x0

.1021:
	mov	[fs_info + bp], ax
	add	bp, 2
	cmp	bp, BLOCK_SIZE
	jne	.1021


;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	call	wait_1000

	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax

.1032:
	xor	bp, bp
.1030:
	mov	ax, word[fs_info+bp]
	call	out_16
	add	bp, 2

	cmp	bp, BLOCK_SIZE
	jne	.1030	
	
	add	ecx, 1
	cmp	ecx, dword[num_inode_blocks]

	jne	.1032
	
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

	mov	eax, ebp

	mov	cl, 24
	shr	eax, cl
	and	al, 0xF
	or	al, 0xA0	; (lba_higest & 0xF | 0xA0) = (LOG_TAB_START >>24 & 0xF | 0xA0)
	or	al, 0x40	; lba<<6 = (1<<6)
	
	mov	byte[cmd+6], al	; Device
	
	mov	byte[cmd+7], dl	; Command

	call	cmd_send

	ret

;================================
add_inode_map:
;================================
	
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	call	zero_fs_info

	mov	dword[block_count], 0	; init dword[block_count]

;call fill cmd	
	xor	eax, eax
	add	eax, 2				; the start block = boot_block + super_block + 1 = 0 + 1 + 1
	mov 	cl, 3
	shl	eax, cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 

	mov	dword[start_addr], eax

	mov	dword[temp_count], 0

.211:


;----------------
;fill command(read) 
;----------------

	mov	eax, dword[start_addr]
	mov	ebp, dword[block_count]
	mov	cl, 3
	shl	ebp, cl
	add	eax, ebp
	mov	ebp, eax

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = num_imap_blocks * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_READ			; command

	call	fill_cmd


;---------------
;get result
;---------------
	xor	ebp, ebp
	xor	ecx, ecx
	xor	eax, eax
	mov	dx, CBP_PORT0		;dx=port=0x1f0

	mov	esi, 1

.220:
	call	in_16
	
;---------------
;set imap
;---------------	
	
.ss1:
	cmp	esi, 1
	jb	.ss20

	cmp	ax, 0xFFFF
	je	.ss10

	cmp	ax, 0xFFFE
	je	.ss12
	
	cmp	ax, 0
	jne	.ss11
	
	mov	ax, 1
	mov	cl, 1

.ss101:
	mov	bx, ax
	shl	ax, 1
	or	ax, bx
	
	sub	esi, 1
	cmp	esi, 1
	jb	.ss10

	cmp	ax, 0xFFFF
	jne	.ss101

	jmp	.ss10


.ss12:
	cmp	dword[temp_count], 0
	je	.ss20
	jmp	.ss11


.ss11:
	mov	cl, 1
.ss2:
	mov	bx, ax
	shl	ax, cl
	or	ax, bx
	sub	esi, 1
	cmp	esi, 1
	jb	.ss10

	cmp	ax, 0xFFFF
	jne	.ss2

.ss10:

.ss20:
	mov	word[fs_info+ebp],ax

	mov	eax, dword[temp_count]
	add	eax, 1
	mov	dword[temp_count], eax

	add	ebp, 2
	cmp	ebp, BLOCK_SIZE
	jne	.220	
	
	cmp	dword[new_inode_id], ROOT_DIR_INODE
	jne	.ss30
	mov	ax, 0x2
	mov	word[fs_info], ax
.ss30:

;----------------
;fill command(write) 
;----------------

	mov	eax, dword[start_addr]
	mov	ebp, dword[block_count]
	mov	cl, 3
	shl	ebp, cl
	add	eax, ebp
	mov	ebp, eax

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = num_imap_blocks * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

.262:
	xor	ebp, ebp
.260:
	mov	ax, word[fs_info+ebp]
	call	out_16
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.260	

	cmp	esi, 1
	jb	.252
	
	mov	ecx, dword[block_count]
	add	ecx, 1
	mov	dword[block_count], ecx
	cmp	ecx, dword[num_imap_blocks]
	jne	.211

	jmp	.252

.251:
	mov	ax, mesg5			;imap is full
	mov	cx, MESG_LEN
	mov	dh, 3
	call	disp_msg


.252:
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret



;================================
add_block_map:
;================================
	
	push 	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	mov	dword[block_count], 0	; init dword[block_count]

;call fill cmd	
	xor	eax, eax
	add	eax, 2				; the start block = boot_block + super_block + imap_block + 1 = 0 + 1 + 1 + imap_block
	add	eax, dword[num_imap_blocks]
	mov 	cl, 3
	shl	eax, cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 

	mov	dword[start_addr], eax

	mov	dword[temp_count1], 0

.111:

	call	zero_fs_info
;----------------
;fill command(read) 
;----------------

	;mov	ebp, eax
	mov	eax, dword[start_addr]
	mov	ebp, dword[block_count]
	mov	cl, 3
	shl	ebp, cl
	add	eax, ebp
	mov	ebp, eax

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = 1 * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_READ			; command

	call	fill_cmd


;---------------
;get result
;---------------


;wait
	
	xor	ebp, ebp
	xor	ecx, ecx
	xor	eax, eax
	mov	dx, CBP_PORT0		;dx=port=0x1f0

	mov	esi, dword[hd_file_len]

.120:
	call	in_16


;---------------
;set imap
;---------------
	
.s1:
	cmp	esi, 1
	jb	.s20

	cmp	ax, 0xFFFF
	je	.s10

	cmp	ax, 0xFFFE
	je	.s12
	
	cmp	ax, 0
	jne	.s11
	
	mov	ax, 1
	mov	cl, 1

.s101:
	mov	bx, ax
	shl	ax, cl
	or	ax, bx
	
	sub	esi, 1
	cmp	esi, 1
	jb	.s10

	cmp	ax, 0xFFFF
	jne	.s101

	jmp	.s10

.s12:
	cmp	dword[temp_count1], 0
	je	.s20
	jmp	.s11


.s11:
	mov	cl, 1
.s2:
	mov	bx, ax
	shl	ax, cl
	or	ax, bx
	sub	esi, 1
	cmp	esi, 1
	jb	.s10

	cmp	ax, 0xFFFF
	jne	.s2

.s10:
	
.s20:
	mov	word[fs_info+ebp],ax

	mov	eax, dword[temp_count1]
	add	eax, 1
	mov	dword[temp_count1], eax

	add	ebp, 2
	cmp	ebp, BLOCK_SIZE
	jne	.120	

	cmp	dword[new_inode_id], ROOT_DIR_INODE
	jne	.s30
	mov	ax, 0x2
	mov	word[fs_info], ax
.s30:

;----------------
;fill command(write) 
;----------------

;call fill cmd	
	mov	eax, dword[start_addr]
	mov	ebp, dword[block_count]
	mov	cl, 3
	shl	ebp, cl
	add	eax, ebp
	mov	ebp, eax

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = 1 * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax


.162:
	xor	ebp, ebp
.160:
	mov	ax, word[fs_info+ebp]
	call	out_16
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.160	

	cmp	esi, 1
	jb	.152
	
	mov	ecx, dword[block_count]
	add	ecx, 1
	mov	dword[block_count], ecx
	cmp	ecx, dword[num_bmap_blocks]
	jne	.111

	jmp	.152

.151:

	mov	ax, mesg6			;bmap is full
	mov	cx, MESG_LEN
	mov	dh, 3
	call	disp_msg


.152:
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret



;================================
add_inode:
;================================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax


	call	zero_fs_info

;setting start address
	xor	eax, eax
	add	eax, 2				; the start block = boot_block + super_block + imap_block + bmap_block + 1 = 0 + 1 + 1 + imap_block + bmap_block
	add	eax, dword[num_imap_blocks]
	add	eax, dword[num_bmap_blocks]
	mov 	cl, 3
	shl	eax, cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 

	mov	ebp, dword[inode_num_offset]
	mov	cl, 3				; INODE_PER_BLOCK
	shr	ebp, cl				; dword[inode_num_offset] / 64 * 8
	
	add	eax, ebp			; the start sector= the start block * 8 + (dword[inode_num_offset])/64) * 8
	mov	dword[start_addr], eax


;---------------------------------------------------------------------
;just output one dot on the screen by writing one file into the harddisk
	
	push	edx
	push	ebx
	push	eax
	

	mov	bx, 0xb800
	mov	es, bx
	mov	bx, [pos]

	mov	al, 46
	mov	byte[es:bx], al
	add	bx, 2		;dot
	
	mov	[pos] , bx

	mov	dx, word[cur_seg]
	mov	es, dx

	pop	eax
	pop	ebx
	pop 	edx
;---------------------------------------------------------------------

;--------------------------------
;fill command(read) 
;--------------------------------
	
;call fill cmd	
	mov	ebp, dword[start_addr]

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = 1 * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_READ			; command

	call	fill_cmd


;---------------
;fill fs_info
;---------------

	xor	ebp, ebp
	mov	dx, CBP_PORT0			;dx=port=0x1f0

.300:
	call	in_16
	mov	word[fs_info+ebp],ax
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.300


	xor	ebp, ebp
	xor	ecx, ecx

	mov 	eax, dword[inode_num_offset]

	mov	cl, INODE_PER_SECTOR	; 8

	div	cl
	xor	ecx, ecx
	mov	cl, ah			; remainder
	mov	ebp, ecx

	mov	cl, 6
	shl	ebp, cl			; ebp = ebp * 64bytes
	
	
	xor	eax, eax
	
	mov	ecx, ebp
	xor	ebp, ebp
			
.320:
	mov	ax, word[i_node+ebp]

	mov	word[fs_info+ecx+ebp], ax
	add	ebp, 2

	cmp	ebp, INODE_SIZE

	jne	.320	

;--------------------------------
;fill command(write) 
;--------------------------------

;call fill cmd	
	
	mov	ebp, dword[start_addr]

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = 1 * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

	xor	eax, eax
	mov	dx, CBP_PORT0		;dx=port=0x1f0

.362:
	xor	ebp, ebp
.360:
	mov	ax, word[fs_info+ebp]
	call	out_16
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.360

	call	zero_i_node

	mov	eax, dword[inode_num_offset]
	add	eax, 1
	mov	dword[inode_num_offset], eax
	cmp	eax, NUM_INODE_MAX_PER_PART
	ja	.351
	
	jmp	.352

.351:

	mov	ax, mesg7			;inode is full
	mov	cx, MESG_LEN
	mov	dh, 3
	call	disp_msg


.352:
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret


;================================
add_block:
;================================

	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax
	push	word[cur_seg]
	push	word[cur_len]
	
;init fs_info
	
	call	zero_cd_info
	
	mov	dword[cd_sector_count], 0	; init dword[cd_sector_count]

;setting start address
	xor	eax, eax
	add	eax, 2				; the start block = boot_block + super_block + imap_block + bmap_block + 1 = 0 + 1 + 1 + imap_block + bmap_block
	add	eax, dword[num_imap_blocks]
	add	eax, dword[num_bmap_blocks]
	add	eax, dword[num_inode_blocks]
	add	eax, dword[block_num_offset]
	mov 	cl, 3
	shl	eax, cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 

	mov	dword[start_addr], eax
	
	mov	dword[hd_file_start_sector], eax

;init [cur_len]	
	mov	ecx, dword[cd_file_len]	
	mov	byte[cur_len], cl

;init [cur_seg]
	xor	eax, eax
	xor	ebp, ebp
	mov	al, byte[cur_len]
	mov	cl, 0xb
	shl	eax, cl			; eax = cur_len * 2kb
	mov	bp, word[cur_seg]
	mov	cl, 4
	shl	ebp, cl			; ebp = cur_seg * 16
	add	eax, ebp		; 
	shr	eax, cl			; eax	  = (cur_seg * 16 + cur_len * 2kb) / 16
	mov	word[cur_seg], ax	; cur_seg = (cur_seg * 16 + cur_len * 2kb) / 16
	
	mov	dx, word[cur_seg]
	mov	word[tm_cur_seg], dx

	mov	eax, dword[cd_file_start_sector]
	mov	dword[tm_cd_file_start_sector], eax

.411:

	mov	eax, dword[tm_cd_file_start_sector]
		
	mov	cl, 1
	mov	dx, word[tm_cur_seg]

	call	read_sector


;---------------
;fill cd_info
;---------------

	mov	dx, word[load_seg]
	mov	ds, dx

	mov	dx, word[tm_cur_seg]
	mov	es, dx

	xor	ebp, ebp
	xor	ecx, ecx
	xor	eax, eax
	
.420:
	mov	cx, word[es:eax]
	mov	word[cd_info+ebp],cx
	add	ebp, 2
	add	eax, 2

	cmp	ebp, 2048
	jne	.420	

.f430:


;--------------------------------
;fill command(write) 
;--------------------------------

	mov	eax, dword[start_addr]

	mov	ebp, dword[cd_sector_count]
	mov	cl, 2
	shl	ebp, cl				; ebp = dword[cd_sector_count] * 4
	add	eax, ebp			; eax = dword[start_addr] + dword[cd_sector_count] * 4

	mov	ebp, eax

	mov	eax, 1
	mov	cl, 2
	shl	eax, cl				; sector count = eax = 1 * 4(sectors/cd block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax

.462:
	xor	ebp, ebp
.460:
	mov	ax, word[cd_info+ebp]
	call	out_16
	add	ebp, 2

	cmp	ebp, 2048
	jne	.460	
	
	mov	ecx, dword[cd_sector_count]
	add	ecx, 1
	mov	dword[cd_sector_count], ecx

	mov	ecx, dword[tm_cd_file_start_sector]
	add	ecx, 1
	mov	dword[tm_cd_file_start_sector], ecx

.463:
	mov	ecx, dword[cd_sector_count]
	cmp	ecx, dword[cd_file_len]
	jne	.411

	mov	ecx, dword[block_num_offset]
	mov	eax, dword[hd_file_len]
	add	eax, ecx
	mov	dword[block_num_offset], eax

	cmp	dword[hd_file_len], 6
	jbe	.550
.500:
	cmp	dword[hd_file_len], 1030	; 1030 = 1024 + 6
	ja	.510
	mov	ecx, dword[block_num_offset]
	add	ecx, 1
	mov	dword[block_num_offset], ecx
	jmp	.550				; jmp the end
.510:
	mov	eax, dword[hd_file_len]
	mov	ecx, 0xC
	shr	eax, cl				; eax / 4096
	add	eax, 1				; number of 1rd_block
	add	eax, 1				; 1 of 2rd_block
	mov	dword[block_num_offset], eax
	

.550:
	call	zero_dir_ent

	jmp	.452


.451:

	mov	ax, mesg8			;bmap is full
	mov	cx, MESG_LEN
	mov	dh, 3
	call	disp_msg


.452:
	pop	word[cur_len]
	pop	word[cur_seg]
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret



;================================
add_root_dir:
;================================

	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax


	call	zero_fs_info

	xor	eax, eax
	mov	dword[block_num_offset], eax

	mov	dword[inode_num_offset], eax

	mov	eax, 1
	mov	dword[hd_file_len], eax

	mov	eax, ROOT_DIR_INODE
	mov	dword[new_inode_id], eax

	call	fill_root_inode
	call	add_inode_map
	call	add_inode
	call	add_block_map
	
	mov	dword[parm_inode_id], ROOT_DIR_INODE

	mov	eax, ROOT_DIR_INODE
	mov	dword[dir_ent], eax
	mov	byte[dir_ent+4], 47		; / is root dir name

	call	add_entry

	mov	eax, dword[new_inode_id]	; add inode id to dir_ent
	add	eax, 1
	mov	dword[new_inode_id], eax

.452:
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;==============
add_entry:
;==============
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	call	zero_fs_info

	mov	dword[block_count], 0	; init dword[block_count]

;setting start address
	mov	eax, dword[hd_file_start_sector]
	mov	dword[start_addr], eax

.411:

;--------------------------------
;fill command(read) 
;--------------------------------

	mov	eax, dword[start_addr]

	mov	ebp, eax

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = 1 * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_READ			; command

	call	fill_cmd

;---------------
;fill fs_info
;---------------

;wait
	call	wait_1000

;get info(info size: 1 sector)
	xor	eax, eax
	xor	bp, bp
	mov	dx, CBP_PORT0		;dx=port=0x1f0

.g0:
	call	in_16
	mov	word[fs_info+bp],ax
	add	bp, 2

	cmp	bp, BLOCK_SIZE
	jne	.g0	
	
	xor	eax, eax	
	mov	ax, word[block_in_offset]

	mov	cl, 6
	shl	eax, cl			; inode_id * 64bytes/inode

	mov	ebp, eax
	xor	ecx, ecx
	
.g1:
	mov	eax, dword[dir_ent+ecx]
	mov	dword[fs_info+ebp],eax

	add	ebp, 4
	add	ecx, 4

	cmp	cx, DIR_ENT_SIZE
	jne	.g1

;--------------------------------
;fill command(write) 
;--------------------------------

	mov	eax, dword[start_addr]
		
	mov	ebp, eax

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = 1 * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

;wait
	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax

.462:
	xor	ebp, ebp
.460:
	mov	ax, word[fs_info+ebp]
	call	out_16
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.460	
	
	xor	ecx, ecx
	mov	cx, word[block_in_offset]
	add	cx, 1
	
	mov	word[block_in_offset], cx
	
	cmp	cx, INODE_PER_BLOCK
	jb	.465
	mov	word[block_in_offset], 0	; if block_in_offset >= 64 then {block_in_offset = 0; block_count++;}
	mov	ecx, dword[block_count]
	add	ecx, 1
	mov	dword[block_count], ecx
	
.465:	
	mov	ecx, dword[block_count]
	add	ecx, 1
	mov	dword[block_count], ecx

	mov	ecx, dword[block_num_offset]
	mov	eax, dword[block_count]
	add	eax, ecx
	mov	dword[block_num_offset], eax


	call	zero_dir_ent

	jmp	.452

.451:
	mov	ax, mesg8			;bmap is full
	mov	cx, MESG_LEN
	mov	dh, 3
	call	disp_msg

.452:
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;==============================================================================
set_1rdblock:
; eax is the parameter , which saves the remainder of hd_file_len
; esi is the parameter , which saves hd_file_start_sector
; ebx is the parameter , which saves block number
;==============================================================================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

; save the parameter
	mov 	dword[num_left_1rd], eax
	mov	dword[tm_hd_file_start_sector], esi
	mov	dword[num_start_1rd], ebx

; add a block_map
	call	add_block_map

; add a block
;init fs_info
	call	zero_fs_info

	mov	esi, dword[tm_hd_file_start_sector]
	mov	dword[start_addr_1rd], esi
	
;---------------
;fill fs_info
;---------------
	mov	dx, word[load_seg]
	mov	ds, dx

	mov	dx, word[cur_seg]
	mov	es, dx

	xor	ebp, ebp
	mov 	ecx, dword[num_left_1rd]

	mov	ebx, dword[num_start_1rd]

		
.420:
	mov	dword[fs_info+ebp],ebx

	add	ebp, 4

	add	ebx, SECTOR_PER_BLOCK

	sub	ecx, 1

	cmp	ecx, 0
	je	.430

	cmp	ebp, BLOCK_SIZE
	jne	.420	

.430:

	mov	dword[tm_num_left], ecx			; save num_left_1rd;

;--------------------------------
;fill command(write) 
;--------------------------------

	mov	eax, dword[start_addr_1rd]

	mov	ebp, dword[num_left_1rd]
	mov	ecx, 3
	shl	ebp, cl				; ebp = num_left_1rd * 8
	add	eax, ebp

	mov	ebp, eax

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = 1 * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax

.462:
	xor	ebp, ebp
.460:
	mov	ax, word[fs_info+ebp]
	call	out_16
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.460	
	

;update block_num_offset

	mov	ecx, dword[tm_num_left]
	mov	dword[num_left_1rd], ecx		; restore num_left_1rd	
	
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;==============================================================================
set_2rdblock:
; esi is the parameter , which saves hd_file_start_sector
;==============================================================================

	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

; save the parameter
	mov	dword[start_addr_g], esi
	mov 	dword[num_left_2rd], eax
	mov	dword[start_addr_2rd], edx

; add a block_map
	call	add_block_map

; add a block
;init fs_info
	call	zero_tmp_info
	
;---------------
;fill fs_info
;---------------
	mov	dx, word[load_seg]
	mov	ds, dx

	mov	dx, word[cur_seg]
	mov	es, dx

	xor	ebp, ebp
	
	mov	ecx, dword[actual_num_left_2rd]

	mov	esi, dword[start_addr_g]
	mov	ebx, esi
	
	mov	edi, dword[actual_2rd_1rd_start_addr]

			
.420:
	mov	eax, ecx				;one parameter is eax, the other is esi

	mov	dword[tmp_info+ebp],edi
	add	edi, SECTOR_PER_BLOCK

.424:
	;2 parameters, one is eax, the other is esi
	call	set_1rdblock

	add	ebp, 4
	add	esi, 8192				; 1024 * SECTOR_PER_BLOCK
	add	esi, SECTOR_PER_BLOCK			; SECTOR_PER_BLOCK
	
	add	ebx, 8192

	cmp	dword[num_left_1rd], 0
	je	.430
		
	sub	ecx, 1024				; 1 block contain 1024 block number 

	cmp	ebp, BLOCK_SIZE
	jne	.420	

.430:
;--------------------------------
;fill command(write) 
;--------------------------------

	mov	ebp, dword[start_addr_2rd]

	mov	eax, 1
	mov	cl, 3
	shl	eax, cl				; sector count = eax = 1 * 8(sectors/block)
	mov	ecx, eax
	
	mov	edx, HD_WRITE			; command

	call	fill_cmd

;----------------------------------------------------
;write data from hd_info into the right sector
;----------------------------------------------------

	xor	eax, eax
	xor	ecx, ecx
	mov	dx, CBP_PORT0			;dx=port=0x1f0

	mov	ax, word[load_seg]
	mov	es, ax

.462:
	xor	ebp, ebp
.460:
	mov	ax, word[tmp_info+ebp]
	call	out_16
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.460	
	
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;================================
is_dir:
;parm: 
;edx:[es:eax]
;return: edx: 1->file; 0->dir
;================================
	push	ebx
	push	ecx
	push	edx

	mov	cx, word[cur_seg]
	mov	es, cx

	xor	ebx, ebx
	mov	ebx, dword[es:edx]	
	and	ebx, 0xFFFFFF00		 
	cmp	ebx, 0x464C4500		;FLE0, judge whether is ELF
	je	.i4201 

	mov	ecx, 0x20
	add	ecx, edx
	mov	bh,  byte[es:ecx]
	
	cmp	bh, 0x1f
	jb	.i4202


.i4201:
	mov	byte[flag_file], 1	; file(file name length>=0x20) && (flags == 0)
	jmp	.i4203


.i4202:
	mov	byte[flag_file], 0	; dir (file name length<0x20) && (flags != 0)
	

.i4203:
	pop	edx
	pop	ecx
	pop	ebx

	ret


;================
zero_i_node:
;================

	push	ebp
	push	eax

.zero_inode1:
	xor	ebp, ebp
	xor	eax, eax
.zero_inode2:
	mov	word[i_node+ebp],ax
	add	ebp, 2

	cmp	ebp, INODE_SIZE
	jne	.zero_inode2

	pop	eax
	pop	ebp

	ret

;================
zero_dir_ent:
;================

	push	ebp
	push	eax

.zero_dir_ent1:
	xor	ebp, ebp
	xor	eax, eax
.zero_dir_ent2:
	mov	word[dir_ent+ebp],ax
	add	ebp, 2

	cmp	ebp, INODE_SIZE
	jne	.zero_dir_ent2

	pop	eax
	pop	ebp

	ret

;================
zero_fs_info:
;================

	push	ebp
	push	eax
.zero1:
	xor	ebp, ebp
	xor	eax, eax
.zero2:
	mov	word[fs_info+ebp],ax
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.zero2	

	pop	eax
	pop	ebp

	ret

;================
zero_tmp_info:
;================

	push	ebp
	push	eax
.zero11:
	xor	ebp, ebp
	xor	eax, eax
.zero22:
	mov	word[tmp_info+ebp],ax
	add	ebp, 2

	cmp	ebp, BLOCK_SIZE
	jne	.zero22	

	pop	eax
	pop	ebp

	ret

;================
zero_cd_info:
;================

	push	ebp
	push	eax
.zero13:
	xor	ebp, ebp
	xor	eax, eax
.zero24:
	mov	word[cd_info+ebp],ax
	add	ebp, 2

	cmp	ebp, 2048
	jne	.zero24	

	pop	eax
	pop	ebp

	ret

;=======================================================================
mesg_fs:		db	"File Sys begin.."
mesg_fs1:		db	"File Sys end...."

mesg5			db	"imap is full...."
mesg6			db	"bmap is full...."
mesg7			db	"inode is full..."
mesg8			db	"block is full..."

MESG_FS_LEN		equ	16

block_count		dd	0
hd_file_start_sector	dd	0
hd_file_len		dd	0
inode_num_offset	dd	0	;inode_num_offset  * 64bytes
block_num_offset	dd	0	;block_num_offset  * 4kb
parm_inode_id		dd	0	;just a parameter
start_addr		dd	0	;just a temp value 
offset_addr		dd	0	;just a temp value
flag_file		db	0	;just a temp value
block_in_offset		dw	0	;bytes, just for entry
num_left		dd	0	;just a temp value
tm_hd_file_start_sector dd	0	;just a temp value
tm_num_left		dd	0	;just a temp value
tm_cur_seg		dw	0	;just a temp value
start_addr_2rd		dd	0	;just a temp value
num_left_1rd		dd	0	;just a temp value
num_left_2rd		dd	0	;just a temp value
start_addr_g		dd	0	;just a temp value
start_addr_1rd		dd	0	;just a temp value
tm_cd_file_start_sector	dd	0	;just a temp value
num_start_1rd		dd	0	;just a temp value
cd_sector_count		dd	0	;just a temp value
temp_count		dd	0	;just a temp value
temp_count1		dd	0	;just a temp value

;super block
;------------------------------------------------------
num_inode		dd	0
num_inode_blocks	dd	0
num_imap_blocks		dd	0
num_blocks		dd	0
num_bmap_blocks		dd	0
block_size		dd	0
inode_size		dd	0
flag_magic		dd	0
n_1st_dbblock		dd	0
root_inode		dd	0
down_mnt_inode		dd	0
up_mnt_inode		dd	0
;------------------------------------------------------

;inode
;------------------------------------------------------
i_mode			dd	0
i_size			dd	0
i_uid			dd	0
i_gid			dd	0
i_acc_tm		dd	0
i_modi_tm		dd	0
i_link_count		dd	0
i_block_0		dd	0
i_block_1		dd	0
i_block_2		dd	0
i_block_3		dd	0
i_block_4		dd	0
i_block_5		dd	0
i_block_1lvl_ind	dd	0
i_block_2lvl_ind	dd	0
;------------------------------------------------------

;directory entry
;------------------------------------------------------
dir_inode		dd	0
dir_name		dd	0
;------------------------------------------------------

