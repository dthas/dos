//===========================================================================
// dtsfs.h
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

#ifndef	_DTSFS_H_
#define	_DTSFS_H_


#define	SUPER_BLOCK_SIZE	48	//bytes
#define	INODE_SIZE		64	//bytes
#define	DIR_ENT_SIZE		64	//bytes
#define	FILENAME_LEN		12	//bytes, MAX_FILE_NAME is 12bytes
#define	SECTOR_SIZE		512	//bytes
#define	BLOCK_SIZE		4096	//sectors, 512 * 8 = 4096
#define	SECTOR_PER_BLOCK	8	//BLOCK_SIZE / SECTOR_SIZE = 4096 / 512 = 8
#define	INODE_PER_BLOCK		64	//BLOCK_SIZE / INODE_SIZE = 4096 / 64 = 64
#define	INODE_PER_SECTOR	8	//SECTOR_SIZE / INODE_SIZE = 512 / 64 = 8
#define	NUM_INODE_MAX_PER_PART	32768	//To each partition, the max inode number is 32768
#define	ROOT_DIR_INODE		1	//root directory inode
#define	DIR_ENT_NAME_LEN	60	//bytes


#define	MAGIC_CODE		0xCC11

#define	NUM_FS_OPERATION	13	//bytes



//===========================================================================
// structure:
//-------------------------------------------------------------------------
// block seq	|	block number	|	comment
//-------------------------------------------------------------------------
//	1	|	1		|	master boot block
//-------------------------------------------------------------------------
//	2	|	1		|	super block
//-------------------------------------------------------------------------
//	3	|	?		|	imap block
//-------------------------------------------------------------------------
//	4	|	?		|	bmap block
//===========================================================================

//-------------------------------------------------------------------------
// super block(total is 48 bytes)
//-------------------------------------------------------------------------
struct	s_dts_super_blk
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
	s32	root_inode;
	s32*	down_mnt_inode;		//pointer
	s32*	up_mnt_inode;		//pointer
};

//-------------------------------------------------------------------------
// inode structure(total is 64 bytes)
//-------------------------------------------------------------------------
struct s_dts_inode
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
	s32	i_num;
};

//-------------------------------------------------------------------------
// directory entry structure
//-------------------------------------------------------------------------
struct s_dts_dir_ent
{
	s32	dir_inode;			//dir_inode(inode=4bytes)
	s8	dir_name[DIR_ENT_NAME_LEN];	//dir_name(max=60bytes)
};	

#endif
