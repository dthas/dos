//===========================================================================
// hd_manager.c
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
#include	"global.h"
#include	"kernel.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"prototype.h"

//variable
PRIVATE	struct s_hdm freelist;

//===========================================================================
// init_freelist
//===========================================================================
PUBLIC	void	init_hd_freelist()
{
	struct s_hdm *pflist = &freelist;

	u32	total_blocks	= part->part_size_in_sec / SECTOR_PER_BLOCK;
		
	// start_sec = 0x4EFF + (0 + 1 + 2 + 6 + 1) * 8 = 0x4F4F	
	u32	start_sec	= part->inode_start_sec;
	
	// start_address_of_inode_block = 0x4F4F * 0x200 = 0x9E9E00
	u32	start_addr	= start_sec * SECTOR_SIZE;	

	// end_sec   = 0x4EFF + (0 + 1 + 2 + 6 + 1 + 0x200) * 8 = 0x5F4F	
	u32	end_sec	 = part->inode_start_sec + part->inode_size_in_sec;

	// end_address_of_inode_block = 0x5F4F * 0x200 = 0xBE9E00
	u32	end_addr	= end_sec * SECTOR_SIZE;

	//-----------------------------------------------------------------
	// loop to compute the start address of free blocks
	//-----------------------------------------------------------------
	struct s_v_inode * v_inode;

	u32	inode_size = vfs->spk.inode_size;
		
	u32 sec;

	u32 blk = 0;

	u32	inodes_in_blk = vfs->spk.block_size / vfs->spk.inode_size;

	u32	block_size = vfs->spk.block_size;

	s8 	flg = 0;

	u32 	i ;

	for(sec = start_sec; sec < end_sec; sec = sec + SECTOR_PER_BLOCK)
	{		
		hd_read_blk(sec, (s16*)&hdbuf, 1);

		v_inode = &hdbuf;		

		for(i = 0; i < inodes_in_blk; i++, v_inode++)
		{
			if(v_inode->i_block_0 == 0)
			{
				flg = 1;
				break;
			}
		
			if(v_inode->i_block_2lvl_ind > blk)
			{
				blk = v_inode->i_block_2lvl_ind;
			}

			if(v_inode->i_block_1lvl_ind > blk)
			{
				blk = v_inode->i_block_1lvl_ind;
			}

			if(v_inode->i_block_5 > blk)
			{
				blk = v_inode->i_block_5;
			}

			if(v_inode->i_block_4 > blk)
			{
				blk = v_inode->i_block_4;
			}

			if(v_inode->i_block_3 > blk)
			{
				blk = v_inode->i_block_3;
			}

			if(v_inode->i_block_2 > blk)
			{
				blk = v_inode->i_block_2;
			}

			if(v_inode->i_block_1 > blk)
			{
				blk = v_inode->i_block_1;
			}

			if(v_inode->i_block_0 > blk)
			{
				blk = v_inode->i_block_0;
			}		
		}

		if(flg)
		{
			break;
		}
	}

	u32 used_sec, left_sec ,left_blk, address ;

	if(blk != 0)
	{		
		used_sec	= blk - part->part_start_sec;				// blk - 0x4EFF		
		left_sec	= part->part_size_in_sec - used_sec;			// 0x2D050 - used_sec
		left_blk	= left_sec / SECTOR_PER_BLOCK;

		address 	= blk * SECTOR_SIZE + vfs->spk.block_size ;
		
		pflist->nr_blk	= left_blk;
		pflist->addr	= address;
		pflist->head 	= NULL;
		pflist->tail 	= NULL;
	}

	//-----------------------------------------------------------------
	// print freelist
	//-----------------------------------------------------------------
//	printflist();
}

//===========================================================================
// alloc_block
//===========================================================================
PUBLIC	u32	alloc_block(u32 nr_block)
{
	struct s_hdm *pflist = &freelist;
	s8 flg = 0;
	s8 *str = "no free harddisk space";

	for(; pflist != NULL ; pflist = pflist->tail)
	{
		if(pflist->nr_blk >= nr_block)
		{
			flg = 1;
			break;
		}
	}

	if(flg)
	{
		if(pflist->nr_blk == nr_block)
		{
			struct s_hdm *prev = pflist->head;
			struct s_hdm *next = pflist->tail;

			if(prev && next)
			{
				prev->tail = next;
			}
			else if(!prev)
			{
				next = &freelist;
				next->head = NULL;
			}
			else if(!next)
			{
				prev->tail = NULL;
			}
			
			return pflist->addr;
		}
		else
		{
			u32 ret_addr 	= pflist->addr;
			u32 new_addr 	= pflist->addr + nr_block * vfs->spk.block_size;
			u32 new_blks 	= pflist->nr_blk - nr_block;

			pflist->addr 	= new_addr;
			pflist->nr_blk	= new_blks;

			return ret_addr;
		}
	}
	else
	{
		disp_str(str);
	}

}


//===========================================================================
// free_block (add to the tail of freelist)
//===========================================================================
PUBLIC	u32	free_block(u32 addr, u32 nr_block)
{
	struct s_hdm *pflist, *prev;
	struct s_hdm next;

	for(pflist = &freelist; pflist != NULL; pflist = pflist->tail)
	{
		prev = pflist;
	}

	pflist		= prev->tail	= &next;
	pflist->addr	= addr;
	pflist->nr_blk 	= nr_block;
	pflist->tail	= NULL;
	pflist->head	= prev;
}


//===========================================================================
// print freelist
//===========================================================================
PUBLIC	void	printflist()
{
	struct s_hdm *pflist = &freelist;
	
	disp_str("HD freelist:");

	disp_str("(");
	for(; pflist != NULL ; pflist = pflist->tail)
	{
		disp_str("[");
		disp_num(pflist->addr);
		disp_str(",");
		disp_num(pflist->nr_blk);
		disp_str("]");		
	}
	disp_str(")");	
}
