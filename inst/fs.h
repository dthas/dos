;===========================================================================
; fs.h 
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

;=======================================================
;File System parameters
;=======================================================

;super block structure
;------------------------------------------------------
super_blk:	dd	0	;num_inode
		dd	0	;num_inode_blocks
		dd	0	;num_imap_blocks
		dd	0	;num_blocks
		dd	0	;num_bmap_blocks
		dd	0	;block_size
		dd	0	;inode_size
		dd	0	;flag_magic
		dd	0	;n_1st_dbblock
		dd	0	;root_inode
		dd	0	;down_mnt_inode, pointer
		dd	0	;up_mnt_inode, pointer
;------------------------------------------------------


;inode structure
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

;directory entry structure
;------------------------------------------------------
dir_ent:	dd	0	;dir_inode(inode=4bytes)
		dd	0	;dir_name(max=60bytes)
		dd	0
		dd	0
		dd	0
		dd	0
		dd	0	
		dd	0
		dd	0
		dd	0
		dd	0
		dd	0	;
		dd	0
		dd	0
		dd	0
		dd	0
				
;------------------------------------------------------

MAGIC_CODE		equ	0xCC11
SUPER_BLOCK_SIZE	equ	48	; bytes
INODE_SIZE		equ	64	; bytes
DIR_ENT_SIZE		equ	64	; bytes
FILENAME_LEN		equ	12	; bytes, MAX_FILE_NAME is 12bytes
SECTOR_SIZE		equ	512	; bytes
BLOCK_SIZE		equ	4096	; sectors, 512 * 8 = 4096
SECTOR_PER_BLOCK	equ	8	; BLOCK_SIZE / SECTOR_SIZE = 4096 / 512 = 8
INODE_PER_BLOCK		equ	64	; BLOCK_SIZE / INODE_SIZE = 4096 / 64 = 64
INODE_PER_SECTOR	equ	8	; SECTOR_SIZE / INODE_SIZE = 512 / 64 = 8
NUM_INODE_MAX_PER_PART	equ	32768	; To each partition, the max inode number is 32768
ROOT_DIR_INODE		equ	1	; root directory inode
DIR_ENT_NAME_LEN	equ	60	; bytes
