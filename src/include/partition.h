//===========================================================================
// partition.h
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

#ifndef	_X_PARTITION_H_
#define	_X_PARTITION_H_

//-------------------------------------------------------------------------
// partition structure
//-------------------------------------------------------------------------
struct s_part
{
	s32	part_start_sec;
	s32	boot_start_sec;
	s32	spbk_start_sec;
	s32	imap_start_sec;
	s32	bmap_start_sec;
	s32	inode_start_sec;
	s32	data_start_sec;

	s32	part_size_in_sec;
	s32	boot_size_in_sec;
	s32	spbk_size_in_sec;
	s32	imap_size_in_sec;
	s32	bmap_size_in_sec;
	s32	inode_size_in_sec;
	s32	data_size_in_sec;
};

PUBLIC struct s_part * part;

#endif
