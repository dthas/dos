//===========================================================================
// partition.c
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

#include	"type.h"
#include	"kernel.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"prototype.h"

PRIVATE	struct s_part spart;

//===========================================================================
// print_part_info()
//===========================================================================
PUBLIC	void init_part()
{
	part = &spart;
}


//===========================================================================
// print_part_info()
//===========================================================================
PUBLIC	void print_part_info()
{
	disp_str("[part start sec: ");
	disp_num(part->part_start_sec);
	disp_str("] , [part size(sec): ");
	disp_num(part->part_size_in_sec);
	disp_str("], ");

	disp_str("[boot start sec: ");
	disp_num(part->boot_start_sec);
	disp_str("] , [boot size(sec): ");
	disp_num(part->boot_size_in_sec);
	disp_str("], ");
	
	disp_str("[spbk start sec: ");
	disp_num(part->spbk_start_sec);
	disp_str("] , [spbk size(sec): ");
	disp_num(part->spbk_size_in_sec);
	disp_str("], ");

	disp_str("[imap start sec: ");
	disp_num(part->imap_start_sec);
	disp_str("] , [imap size(sec): ");
	disp_num(part->imap_size_in_sec);
	disp_str("], ");

	disp_str("[bmap start sec: ");
	disp_num(part->bmap_start_sec);
	disp_str("] , [bmap size(sec): ");
	disp_num(part->bmap_size_in_sec);
	disp_str("], ");

	disp_str("[inode start sec: ");
	disp_num(part->inode_start_sec);
	disp_str("] , [inode size(sec): ");
	disp_num(part->inode_size_in_sec);
	disp_str("], ");

	disp_str("[data start sec: ");
	disp_num(part->data_start_sec);
	disp_str("] , [data size(sec): ");
	disp_num(part->data_size_in_sec);
	disp_str("], ");	
}
