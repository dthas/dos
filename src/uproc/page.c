//===========================================================================
// page.c
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


//===========================================================================
// page_driver(u32 page_laddr), page_laddr is the linear address of the page 
// that need to be loaded
//===========================================================================
PUBLIC	u32	page_driver(u32 page_laddr)
{
	swapin(page_laddr);
}

//===========================================================================
// PUBLIC	s32	swapin(u32 page_laddr)
//===========================================================================
PUBLIC	s32	swapin(u32 page_laddr)
{
	struct proc *new_p = proc_ready;

	struct s_vmem_map * vmap	= new_p->vmem_paddr;

	s8 flg = FALSE;
	//--------------------------------------------------------------------------------
	// load page from harddisk to memory
	//--------------------------------------------------------------------------------
	
	//totally, there 1MB virtual memory(256 pages) are mapped to harddisk, so the vmap[] size is 
	// sizeof(struct s_vmem_map) * 256 = 8 bytes * 256 = 2048 = 0x800
	s32 i;
	page_laddr = page_laddr & 0xFFFFF000;	//align to 4k

	for(i=0; i<USER_PAGE_VMEM_SIZE ; i++)
	{
		if(vmap[i].laddr == page_laddr)
		{
			flg = TRUE;
			break;
		}
	}

	if(!flg)
	{		
		printk("no page mapping");

		while(1)
		{};
	}

	//--------------------------------------------------------------------------------
	// update page tables
	//--------------------------------------------------------------------------------
	u32 base_addr	= new_p->page_dir_base;
	u32 offset	= page_laddr - base_addr;

	u32 *page_table = base_addr + USER_PAGE_DIR_SIZE;
	
	s32 j;
	j = page_laddr / PAGE_SIZE;
	
	page_table[j] |= PG_EXIST;

	readb((s16*)page_laddr, vmap[i].hd_sec, 1);	
}

//===========================================================================
// swapout()
//
// move the pages from memory to harddisk
//===========================================================================
PUBLIC	s32	swapout(struct s_vmem_map * vmap)
{
	write(vmap->laddr, vmap->hd_sec, 1);
}
