//===========================================================================
// fs.h
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

#ifndef	_X_FS_H_
#define	_X_FS_H_

#define	NUM_FS_TYPE_SUPPORT	2	//how many fs does DTHAS support

#define	FS_TYPE_DTS		1	//
#define	FS_DTS_MAGIC_CODE	0xCC11

#define	FS_TYPE_FAT32		2	//
#define	FS_FAT32_MAGIC_CODE	0x222

#define	NO_SUCH_ENTRY		0
#define	DIR_NOT_FOUND		1
#define	ENTRY_EXIST		2
#define	FILE_NOT_FOUND		3
#define	ROOT_DIRECTORY		4
#define	OPERATION_SUCCESS	99

#define	INODE_R_MODE		0
#define	INODE_W_MODE		1
#define	INODE_E_MODE		2
#define	INODE_R_W_MODE		3	
#define	INODE_R_W_E_MODE	4	
#define	INODE_CHAR_R_MODE	5
#define	INODE_CHAR_W_MODE	6

PUBLIC s8 fsbuf[BLOCK_SIZE];

struct fstype 
{
	u32	type;
	u32	magic_code;
};

union u_fstype
{
	struct s_dts_super_blk dts;
} u_fs;

PUBLIC 	u8	*p_fs_stack;

#endif
