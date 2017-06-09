//===========================================================================
// mem_manage.c
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
#include	"stdio.h"
#include	"signal.h"
#include	"kernel.h"
#include	"global.h"
#include	"sys.h"
#include	"tty.h"
#include	"keyboard.h"
#include	"mesg.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"proc.h"
#include	"uproc.h"
#include	"elf_c.h"
#include	"prototype.h"


//variable
PRIVATE	struct s_memm freelist;

//===========================================================================
// init_freelist
//===========================================================================
PUBLIC	void	init_mem_freelist()
{
	struct s_memm *pflist = &freelist;

	//*(0xFFFFF0)
	s32	total_bytes	= *(s32*)(MEM_SIZE_ADDR);

	// 0x1000000(16MB)
	u32	start_addr	= FREE_START_ADDR;	

	s32	left_bytes	= total_bytes - start_addr;	

	pflist->nr_bytes= left_bytes;
	pflist->addr	= start_addr;
	pflist->head 	= NULL;
	pflist->tail 	= NULL;	

	//-----------------------------------------------------------------
	// print freelist
	//-----------------------------------------------------------------
	//print_memflist();
}

//===========================================================================
// print freelist
//===========================================================================
PUBLIC	void	print_memflist()
{
	struct s_memm *pflist = &freelist;	
	
	printk("{MEM freelist:");

	for(; pflist != NULL ; pflist = pflist->tail)
	{		
		printk("[%x,%x],",pflist->addr,pflist->nr_bytes);
	}
	
	printk("}\n");
}

//===========================================================================
// m_alloc
//===========================================================================
PUBLIC	s32	sys_alloc(s32 size_in_bytes)
{
	struct s_memm *pflist = &freelist;
	s8 flg = 0;
	s8 *str = "no free memory space";

	for(; pflist != NULL ; pflist = pflist->tail)
	{
		if(pflist->nr_bytes >= size_in_bytes)
		{
			flg = 1;
			break;
		}
	}

	if(flg)
	{
		if(pflist->nr_bytes == size_in_bytes)
		{
			struct s_memm *prev = pflist->head;
			struct s_memm *next = pflist->tail;

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
			u32 new_addr 	= pflist->addr + size_in_bytes;
			u32 new_bytes 	= pflist->nr_bytes - size_in_bytes;

			pflist->addr 	= new_addr;
			pflist->nr_bytes= new_bytes;

			return ret_addr;
		}
	}
	else
	{		
		printk("%s\n",str);
	}
}

//===========================================================================
// sys_free (add to the tail of freelist)
//===========================================================================
PUBLIC	s32	sys_free(u32 addr, s32 nr_bytes)
{
	struct s_memm *pflist;
	s8 flg = FALSE;

	s32 end_addr = addr + nr_bytes;
		
	//if can merge, then update the exist node
	for(pflist = &freelist; pflist != NULL; pflist = pflist->tail)
	{
		if(pflist->addr == end_addr)
		{
			pflist->addr	= addr;
			pflist->nr_bytes += nr_bytes;
			flg = TRUE;
			break;
		}
	}

	//if can not merge, add a node which is for new free block and put it in the end of freelist
	if(flg == FALSE)
	{
		struct s_memm *prev;
		struct s_memm next;

		for(pflist = &freelist; pflist != NULL; pflist = pflist->tail)
		{
			prev = pflist;
		}
	
		pflist		= prev->tail	= &next;
		pflist->addr	= addr;
		pflist->nr_bytes= nr_bytes;
		pflist->tail	= NULL;
		pflist->head	= prev;
	}
}

//===========================================================================
// sys_valloc(s32 nr), nr is the numbers of pages that require
//===========================================================================
PUBLIC	s32	sys_valloc(s32 nr)
{
	u8 *vmap = hd_vem_map;
	s8 flg = 0, i;
	
	s8 *str = "no free virtual memory space";

	for(i=0; i<VMEM_HD_MAP_NUM ; i++)
	{
		if(vmap[i] != 0xFF)
		{
			flg = 1;
			break;
		}
	}

	if(flg)
	{
		u8 j,k;
		j=k=0;
		do
		{
			if(vmap[i] == k)
			{
				break;
			}
			k = k + (1<<j);
			j++;
		}while(j<8);

		vmap[i]	       |= (1<<j);

		s32 sec_nr	= VMEM_HD_START_SEC + (i * 8 + j) * 8; //each bytes has 8 bit, each bit present 1 sector

		return sec_nr;
	}
	else
	{		
		printk("%s\n",str);
	}
}
