//===========================================================================
// vfs.h
//   Copyright (C) 2012 Free Software Foundation, Inc.
//   Originally by ZhaoFeng Liang <zhf.liang@outlook.com>
//
//This file is part of DTHAS.
//
//DTHAS is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or 
//(at your option) any later version.
//
//DTHAS is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with DTHAS; If not, see <http://www.gnu.org/licenses/>.  
//===========================================================================

#ifndef	_X_VFS_H_
#define	_X_VFS_H_

//-------------------------------------------------------------------------
// vfs super block(total is 48 bytes)
//-------------------------------------------------------------------------
struct	s_v_super_blk
{
	s32	num_inode;
	s32	num_inode_blocks;
	s32	num_imap_blocks;
	s32	num_blocks;
	s32	num_bmap_blocks;
	s32	block_size;
	s32	inode_size;
	s32	flag_magic;
	s32	n_1st_dbblock;
};

//-------------------------------------------------------------------------
// inode structure(total is 64 bytes)
//-------------------------------------------------------------------------
struct s_v_inode
{
	s32	i_mode;
	s32	i_size;
	s32	i_uid;
	s32	i_gid;
	s32	i_acc_tm;
	s32	i_modi_tm;
	s32	i_link_count;
	s32	i_block_0;
	s32	i_block_1;
	s32	i_block_2;
	s32	i_block_3;
	s32	i_block_4;
	s32	i_block_5;
	s32	i_block_1lvl_ind;
	s32	i_block_2lvl_ind;
	s32	reserve;
};

//-------------------------------------------------------------------------
// directory entry structure
//-------------------------------------------------------------------------
struct s_v_dir_ent
{
	s32	dir_inode;			//dir_inode(inode=4bytes)
	s8	dir_name[DIR_ENT_NAME_LEN];	//dir_name(max=60bytes)
};

//-------------------------------------------------------------------------
// vfs functions
//-------------------------------------------------------------------------
struct s_vfs {
	fs_op5	add_inode;
	fs_op6	add_block;
	fs_op	add_imap_bit;
	fs_op5	add_bmap_bit;
	fs_op4	add_dir_ent;

	fs_op5	del_inode;
	fs_op6	del_block;
	fs_op	del_imap_bit;
	fs_op5	del_bmap_bit;
	fs_op4	del_dir_ent;

	fs_op7	get_inode;
	
	fs_op3	get_super_blk;
	fs_op8	update_inode;

	struct	s_v_super_blk spk;
};


PUBLIC struct s_vfs *vfs;

//-------------------------------------------------------------------------
// file descriptors
//-------------------------------------------------------------------------
struct f_desc
{
	u32	mode;		// the mode(read/write) that the process access the file
	u32	cur_pos;	// the current position of the file that the process processed
	u32	nr_links;	// the number of processes that opened this file
	struct s_v_inode *inode;// point to the file
};

#endif
