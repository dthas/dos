;===========================================================================
; copyfile.asm 
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
copyfile:
;===============
	
	mov	word[pos], 2880			; (80*18) * 2

	mov	ax, 0x900
	mov	es, ax

	mov	ax, mesg_copy
	mov	cx, MESG_COPY_LEN
	mov	dh, 17
	call	disp_msg

	call	copy_main

	mov	ax, word[load_seg]
	mov	es, ax

	mov	ax, mesg_copy1
	mov	cx, MESG_COPY_LEN
	mov	dh, 20
	call	disp_msg

	ret


;===============
copy_main:
;===============
	call	get_ptd
	call	get_dir

	ret

;========
get_ptd:
;========
	mov	ax, 0x1200
	mov	word[cur_seg], ax
	mov	ax, 4
	mov	word[cur_len], ax

	mov	ax, 0x900
	mov	word[load_seg], ax
	
	mov	byte[flag_file], 0		; default is a dir

	mov	eax, 0x10
	mov	cl, byte[cur_len]
	mov	dx, word[cur_seg]
      	call    read_sector

	mov	bx, 0x9e
	mov	eax, dword[es:bx]
	mov 	dword[dir_start_sector],eax

	mov	bx, 0xa6
	mov	eax, dword[es:bx]
	mov	dword[dir_size_sector], eax

	ret


;========================
get_dir:
;(search dir "src")
;========================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax
	push	word[cur_seg]
	push	word[cur_len]


	mov	ax, word[load_seg]
	mov	ds, ax

	mov	eax, dword[dir_size_sector]
	mov	cl, 0xb
	shr	eax, cl
	mov	byte[cur_len], al
	
	xor	eax, eax
	xor	ebp, ebp
	mov	al, byte[cur_len]
	mov	cl, 0xb
	shl	eax, cl			; eax = cur_len * 2kb
	mov	bp, word[cur_seg]
	mov	cl, 4
	shl	ebp, cl			; ebp = cur_seg * 16
	add	eax, ebp		; 
	shr	eax, cl			; eax 	  = (cur_seg * 16 + cur_len * 2kb) / 16
	mov	word[cur_seg], ax	; cur_seg = (cur_seg * 16 + cur_len * 2kb) / 16
	mov	dx, ax			; dx 	  = (cur_seg * 16 + cur_len * 2kb) / 16

	mov	ax, word[dir_start_sector]
	mov	cl, byte[cur_len]

	call    read_sector

	xor	esi, esi

	xor	edx, edx
	xor	ecx, ecx
	xor	edi, edi
	
.100:

	xor	edi, edi			;start an entry
	
	mov	cx, word[cur_seg]
	mov	es, cx

	xor	ecx, ecx
	mov	cl, byte[es:edx]		;edi = entry length

	cmp	cl, 0
	je	.is_next_sector	
	
	mov	edi, ecx

	mov	eax, edx
	add	eax, 0x20
	mov	cl, byte[es:eax]

	cmp	cl, byte[dir_list]
	jne	.103

	mov 	byte[file_name_len], cl

	mov	eax, edx
	add	eax, 2
	push	edi
	mov	edi, dword[es:eax]		;get file start sector
	mov	dword[cd_file_start_sector], edi	 
	pop	edi

	mov	eax, edx
	add	eax, 0xa
	mov	ecx, dword[es:eax]		;ecx is counted by bytes,no to change to sectors
	mov	eax, ecx
	mov	cl, 0xb				;should be 2kb/sector
	shr	eax, cl
	add	eax, 1
	mov 	dword[cd_file_len], eax		;get file length

	mov	eax, edx
	add	eax, 0xa
	mov	ecx, dword[es:eax]		;ecx is counted by bytes,no to change to block
	mov	eax, ecx
	mov	cl, 0xc				;should be 4kb/block
	shr	eax, cl
	add	eax, 1
	mov 	dword[hd_file_len], eax		;get file length
	
.101:
	xor	ax, ax	
	xor	bp, bp
	xor	bx, bx
	mov	ebp, edx			;start to compare "src",ecx = temp(total)
	add	ebp, 0x21	
	mov	cx, 1
	
.102:	
	mov	al, byte[dir_list + ecx]
	cmp	byte[es:ebp], al
	jne	.103
	add	ebp, 1
	add	ecx,1
	add	bl, 1

	cmp	bl, byte[dir_list]
	je	.found_root_dir	
	jmp	.102

.103:
	add	edx, edi
	jmp	.100

.104:
	
.is_next_sector:
	
	mov	eax, edx
	and	eax, 0x800
	add	eax, 0x800
	mov	edx, eax
	cmp	edx, dword[dir_size_sector]
	jb	.100

.no_found_root_dir:
	mov	ax, word[load_seg]
	mov	es, ax
	
	mov	ax, mesg_copy3
	mov	cx, MESG_LENG
	mov	dh, 3
	call	disp_msg
	jmp	.120

.found_root_dir:

	mov	eax, dword[block_num_offset]
	sub	eax, 1
	push	eax				; push (dword[block_num_offset] -1); for the former has add 1
	
	call	get_file
	
	mov	cx, word[cur_seg]
	mov	es, cx

	mov	eax, edx
	add	eax, 0x20
	mov	cl, byte[es:eax]
	mov 	byte[file_name_len], cl

;fill dir entry (file name)
	mov 	cl, byte[file_name_len]
	xor	ebp, ebp
	mov	eax, edx
	add	eax, 0x21
.100_1:
	mov	ch, byte[es:eax]
	mov	byte[dir_ent+4+ebp],ch
	add	eax, 1
	add	ebp, 1
	sub	cl, 1
	cmp	cl, 0
	jne	.100_1

	mov	eax, dword[block_num_offset]
	pop	dword[block_num_offset]

	mov	word[block_in_offset], 1		; the former is '/'
	
	call	copy_dir

	mov	dword[block_num_offset], eax

.end_of_dir_list:

		
.120:
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
;========================
get_file:
;========================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax
	push	word[cur_seg]
	push	word[cur_len]
	push	dword[hd_file_len]
	push	word[num_entry_in_dir]
	push	dword[hd_file_len]
	
	xor	eax, eax
	xor	ebp, ebp
	mov	al, byte[cur_len]
	mov	cl, 0xb
	shl	eax, cl				; eax = cur_len * 2kb
	mov	bp, word[cur_seg]
	mov	cl, 4
	shl	ebp, cl				; ebp = cur_seg * 16
	add	eax, ebp			; 
	shr	eax, cl				; eax 	  = (cur_seg * 16 + cur_len * 2kb) / 16
	mov	word[cur_seg], ax		; cur_seg = (cur_seg * 16 + cur_len * 2kb) / 16
	mov	dx, ax				; dx 	  = (cur_seg * 16 + cur_len * 2kb) / 16

	mov	eax, dword[cd_file_start_sector]
	mov	ecx, dword[cd_file_len]
	mov	byte[cur_len], cl
	
	call    read_sector
	
	mov	dx, word[load_seg]
	mov	ds, dx
	
	mov	word[num_entry_in_dir], 0
	
	mov	word[block_in_offset], 0

	xor	edx, edx

.100:

	xor	edi, edi			;start an entry	
	
	mov	cx, word[cur_seg]
	mov	es, cx

	xor	ecx, ecx
	mov	cl, byte[es:edx]		;edi = entry length

	cmp	cl, 0
	je	.end_of_dr

	mov	edi, ecx

	mov	eax, edx
	add	eax, 0x21
	mov	cl, byte[es:eax]		;if filename is "." or ".."
	cmp	cl, 0x0				;"."
	je	.103
	cmp	cl, 0x1				;".."
	je	.103

;have updated in fs.asm/add block
	call	is_dir
	cmp	byte[flag_file], 1
; file
	je	.102				
; dir
	mov	eax, edx
	add	eax, 2
	push	edi
	mov	edi, dword[es:eax]		;get file start sector
	mov	dword[cd_file_start_sector], edi	
	pop	edi

	mov	eax, edx
	add	eax, 0xa
	mov	ecx, dword[es:eax]		;ecx is counted by bytes,no to change to sectors
	mov	eax, ecx

	mov	cl, 0xb				;should be 2kb/sector
	shr	eax, cl
	add	eax, 1
	mov 	dword[cd_file_len], eax		;get file length
	
	mov	eax, edx
	add	eax, 0xa
	mov	ecx, dword[es:eax]		;ecx is counted by bytes,no to change to block
	mov	eax, ecx
	mov	cl, 0xc				;should be 4kb/block
	shr	eax, cl
	add	eax, 1
	mov 	dword[hd_file_len], eax		;get file length

	cmp	word[num_entry_in_dir], 1
	jae	.100_1
	jmp	.100_2
.100_1:
	
	mov	eax, dword[block_num_offset]
	sub	eax, 1
	push	eax				; push (dword[block_num_offset] -1); for the former has add 1
	push 	word[block_in_offset]

	cmp	word[num_entry_in_dir], 1
	jbe	.100_2
	mov	eax, dword[tm_block_num_offset]
	mov	dword[block_num_offset], eax

.100_2:
	call	get_file
	
	mov	cx, word[cur_seg]
	mov	es, cx

	mov	eax, edx
	add	eax, 0x20
	mov	cl, byte[es:eax]
	mov 	byte[file_name_len], cl


;fill dir entry (file name)
	mov 	cl, byte[file_name_len]
	xor	ebp, ebp
	mov	eax, edx
	add	eax, 0x21
.101:
	mov	ch, byte[es:eax]
	mov	byte[dir_ent+4+ebp],ch
	add	eax, 1
	add	ebp, 1
	sub	cl, 1
	cmp	cl, 0
	jne	.101

	
	cmp	word[num_entry_in_dir], 1
	jae	.101_1
	jmp	.101_2
.101_1:
	pop	word[block_in_offset]
	
	mov	eax, dword[block_num_offset]
	pop	dword[block_num_offset]
	jmp	.101_3

	jmp	.101_21

.101_2:	
	mov	word[block_in_offset], 0


.101_21:
	


.101_3:
	
	call	copy_dir
	
	cmp	word[num_entry_in_dir], 1
	jae	.103_1
	jmp	.103_2
.103_1:		
	mov	dword[tm_block_num_offset], eax

.103_2:	
	add	word[num_entry_in_dir], 1
	
	jmp	.103
	
.102:
	call	copy_file	
	jmp	.104

.103:
	add	edx, edi
	jmp	.100


.end_of_dr:
	cmp	word[num_entry_in_dir], 1
	jbe	.103i
	mov	eax, dword[tm_block_num_offset]
	mov	dword[block_num_offset], eax

.103i:
	
.104:
	pop	dword[hd_file_len]
	pop	word[num_entry_in_dir]
	pop	dword[hd_file_len]
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

;============
copy_file:
;============

	call	add_block_map
	call	add_block
	
	ret

;============
copy_dir:
;============
	push	eax

	call	fill_inode
	call	add_inode_map
	call	add_inode
	call	add_block_map
	call	fill_dir_entry	
	call	add_entry

	mov	eax, dword[new_inode_id]	; add inode id to dir_ent
	add	eax, 1
	mov	dword[new_inode_id], eax

	pop	eax
	
	ret
;========================
fill_dir_entry:
;========================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax
	
	;fill new inode id(dir_ent[0~3]
	mov	eax, dword[new_inode_id]
	mov	dword[dir_ent], eax

	;fill new inode id(dir_ent[4~63]
	;have filled in get_file

	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret
;========================
fill_inode:
;========================

	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax

	mov	eax, dword[hd_file_len]

	mov	dword[i_node + 4], eax		; i_size = hd_file_len

	mov	ebp, 28
	mov	esi, dword[hd_file_start_sector]
	mov	ecx, 6
.i0:
	mov	dword[i_node + ebp], esi
	add	ebp, 4
	add	esi, SECTOR_PER_BLOCK
	sub	eax, 1
	sub	ecx, 1
	cmp	eax, 0
	je	.i12

	cmp	ecx, 0
	jne	.i0

	cmp	eax, 1024
	ja	.i11

.i10:
	mov	edx, eax
	mov	ecx, 3
	shl	edx, cl					;edx = num_left * 8
	add	edx, esi				;esi not change
	mov	dword[i_node + ebp], edx
	add	ebp, 4
	
	mov	ebx, esi

	; eax is the parameter , which saves the remainder of hd_file_len
	; esi is current hd_file_start_sector
	; ebx is the parameter , which saves block number
	call	set_1rdblock

	jmp	.i12
	
.i11:	
	mov	dword[actual_num_left_2rd], eax

	mov	edx, eax
	mov	ecx, 3
	shl	edx, cl
	add	edx, esi
	mov	dword[actual_2rd_1rd_start_addr], edx
	
	xor	edx, edx
	push 	eax
	mov	ecx, 0xa
	shr	eax, cl
	mov	edx, eax
	add	edx, 1					;number of 1rd_block	
	pop	eax

	add	edx, eax
	mov	ecx, 3
	shl	edx, cl					;edx = num_left * 8
	add	edx, esi				;esi not change
	
	add	ebp, 4
	mov	dword[i_node + ebp], edx
	
	; eax, esi are the parameters, the same as set_1rdblock
	call	set_2rdblock

.i12:
	
;update hd_file_start_sector
	xor	eax, eax
	add	eax, 2				; the start block = boot_block + super_block + imap_block + bmap_block + num_inode_blocks + block_num_offset + 1 = 0 + 1 + 1 + imap_block + bmap_block + num_inode_blocks + block_num_offset 
	add	eax, dword[num_imap_blocks]
	add	eax, dword[num_bmap_blocks]
	add	eax, dword[num_inode_blocks]
	add	eax, dword[block_num_offset]
	mov 	cl, 3
	shl	eax, cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 

	mov	dword[hd_file_start_sector], eax	
	
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret

;========================
fill_root_inode:
;========================
	push	edi
	push 	esi
	push	ebp
	push	edx
	push	ecx
	push 	ebx
	push	eax
	
;update hd_file_start_sector
	xor	eax, eax
	add	eax, 2				; the start block = boot_block + super_block + imap_block + bmap_block + num_inode_blocks + block_num_offset + 1 = 0 + 1 + 1 + imap_block + bmap_block + num_inode_blocks + block_num_offset 
	add	eax, dword[num_imap_blocks]
	add	eax, dword[num_bmap_blocks]
	add	eax, dword[num_inode_blocks]
	add	eax, dword[block_num_offset]
	mov 	cl, 3
	shl	eax, cl				; the start sector= the start block * 8
	add	eax, dword[inst_part_start]	; 

	mov	dword[hd_file_start_sector], eax	

	mov	eax, dword[hd_file_len]
	cmp	eax, 6
	ja	.10

	mov	esi, dword[hd_file_start_sector]
	mov	ebp, 28

	mov	dword[i_node + ebp], esi

	jmp	.11

.10:
	call	set_1rdblock

.11:
	
	pop	eax
	pop	ebx
	pop	ecx
	pop	edx
	pop	ebp
	pop 	esi
	pop	edi

	ret
;=======================================================================
dap:			dw	0;	0x10		; [ 0] Packet size in bytes.
			dw	0;	0x4		; [ 2] Nr of blocks to transfer.
			dw	0;	0		; [ 4] Offset
			dw	0;	0x1200		; [ 6] Segment
			dd	0;	0x63		; [ 8] start sector number in iso.
			dd	0;	0		; [12] Block number
;-----------------------------------------------------------------------
dir_list		db	3
			db	"src            "
			db	0
			

;-----------------------------------------------------------------------

mesg1			db	"no found dir!..."
mesg2			db	"found dir!......"
mesg3			db	"no found filess."
mesg4			db	"found file ....."
mesg_copy3		db	"no find root dir"
mesg_copy4		db	"find root dir..."
mesg_copy5		db	"end of dir list."

dir_start_sector	dw	0
dir_size_sector		dw	0
pt_start_sector		dd	0
cd_file_start_sector	dd	0
cd_file_len		dd	0
file_name_len		db	0
dir_name_len		db	0
cur_seg			dw	0
cur_len			dw	0
load_seg		dw	0
num_entry_in_dir	dw	0
prev_hd_file_len	dd	0
actual_2rd_1rd_start_addr		dd	0
actual_num_left_2rd	dd	0
tm_block_num_offset	dd	0

BOOT_DIR_LEN		equ	3		; the length of the name of src is 3
LOAD_FILE_NAME_LEN	equ	8
MESG_LENG		equ	16

mesg_copy:		db	"Copy begin......"
mesg_copy1:		db	"Copy end........"
MESG_COPY_LEN		equ	16

new_inode_id		dd	0
new_inode_name		dd	0

copy_dir_name		db	"src"
COPY_DIR_NAME_LEN	equ	3

MESG1_LEN		equ	13
MESG2_LEN		equ	10
MESG3_LEN		equ	13
MESG4_LEN		equ	10

;=======================================================================

