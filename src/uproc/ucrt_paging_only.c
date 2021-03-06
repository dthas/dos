//===========================================================================
// ucrt.c
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
//
//
//
// Attentiion:
// This file is used for paging only. And all the "bin files"(such as echo, 
// cat, mkdir etc) are need to compile with -Ttext 0x600000   
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
// fork
//===========================================================================
PUBLIC	struct proc *	sys_fork()
{
	struct proc * ptab;
	s32 i;

	for(ptab=proc_tab, i = PROC_SYS_NUM + PROC_USER_TEST_NUM; i < PROC_NUM; i++, ptab++)
	{
		if (ptab->pid == NO_INIT)
		{
			break;
		}
	}

	return ptab;
}

//===========================================================================
// exit
//===========================================================================
PUBLIC	s32	sys_exit(s32 pid)
{
	struct proc * ptab;
	s32 i;

	for(ptab=proc_tab, i = 0; i < PROC_NUM; i++, ptab++)
	{
		if (ptab->pid == pid)
		{
			break;
		}
	}
	
	sys_free(ptab->mem_paddr, ptab->mem_size_bytes);
	ptab->mem_paddr	= 0;
	ptab->mem_size_bytes = 0;
	
	ptab->page_dir_base	= NO_INIT;
	ptab->page_tbl_base	= NO_INIT;
	ptab->pages_base	= NO_INIT;

	ptab->regs.eax 	= 0;
	ptab->regs.ebx 	= 0;
	ptab->regs.ecx 	= 0;
	ptab->regs.esp 	= 0;
	ptab->regs.eip 	= 0;
	ptab->regs.ebp 	= 0;
	ptab->pid	= NO_INIT;
	ptab->status	= NO_INIT;		// 0	
}


//===========================================================================
// execl
//===========================================================================
PUBLIC	s32	sys_execl(struct proc *new_p, s8 *filename, s32 argc, u32 argv_addr)
{
	//--------------------------------------------------------------------------------
	// ELF processing
	//--------------------------------------------------------------------------------
	
	s8 pathname[256]= "/src/bin/";
	
	s32 d_len	= strlen(pathname);

	s32 s_len	= strlen(filename) + 1;
	strcpy(&pathname[d_len], filename, s_len);	

	// read exe file into 0x800000(p_tmp_data);
	read(pathname, FS_READ);
	
	struct elf_header *phelf = p_tmp_data;

	s32	prog_entry		= phelf->entry;
	s32	prog_header_offset	= phelf->phtaboff;
	s32	prog_header_size	= phelf->phtabsize;
	s32	prog_header_num		= phelf->phtnrentry;
	
	s32	cur_prog_header_offset, total_size, code_size;
	s32 	i, mem_length, file_length;
	s8	flg_code = FALSE;
	for(i=0, cur_prog_header_offset	= 0, total_size =0, code_size = 0; i<prog_header_num; i++)
	{
		cur_prog_header_offset	= prog_header_offset + prog_header_size * i;	
		file_length		= *(s32*)(p_tmp_data + cur_prog_header_offset + 16);

		if((file_length == 0) && (flg_code == FALSE))
		{
			code_size 	= total_size;
			flg_code	= TRUE;
		}

		mem_length		= *(s32*)(p_tmp_data + cur_prog_header_offset + 20);
		total_size		= total_size + mem_length;
	}

	s32 new_p_base		= sys_alloc(USER_PROC_RESERVE + total_size);	

	s32 file_offset, vir_addr, file_len, prog_start_addr, mem_len;
	for(i=0, cur_prog_header_offset	= 0; i<prog_header_num; i++)
	{
		cur_prog_header_offset	= prog_header_offset + prog_header_size * i;
	
		file_offset		= *(s32*)(p_tmp_data + cur_prog_header_offset + 4);
		vir_addr		= *(s32*)(p_tmp_data + cur_prog_header_offset + 8);
		file_len		= *(s32*)(p_tmp_data + cur_prog_header_offset + 16);		
		
		mem_len			= *(s32*)(p_tmp_data + cur_prog_header_offset + 20);
		
		prog_start_addr		= file_offset + p_tmp_data;
		
		vir_addr		= vir_addr + new_p_base;
				
		if(file_len != 0)
		{
			memcpy((u8*)vir_addr, (u8*)prog_start_addr, file_len);
		}
		else if(mem_len != 0)
		{			
			memcpy((u8*)vir_addr, 0, mem_len);	//fill 0
		}
		else
		{
			break;
		}		
	}

	//--------------------------------------------------------------------------------
	// stack setting
	//--------------------------------------------------------------------------------	
	u32	data_base_vaddr	= new_p_base + USER_PROC_RESERVE + code_size;
	u32	data_base_paddr	= new_p_base + USER_PROC_RESERVE + code_size;
	stack_setup(data_base_paddr, data_base_vaddr, argc, argv_addr);		

	//--------------------------------------------------------------------------------
	// page status initialize
	//--------------------------------------------------------------------------------
	//(1) all the status is set to 0	
	u8 * pages = new_p_base + USER_PAGE_DIR_SIZE + USER_PAGE_TABLE_SIZE;
	
	for(i=0; i<PG_REC_SIZE; i++)
	{
		pages[i]	= 0;
	}

	//--------------------------------------------------------------------------------
	// page status reset
	//--------------------------------------------------------------------------------
	//(2) the pages that will be used in the program will be set to 1
	s32 j;

	s32 k = (new_p_base + USER_PROC_RESERVE + total_size) / PAGE_SIZE + 1;// k is the total bits that use to remark the status of each page
	
	pages = new_p_base + USER_PAGE_DIR_SIZE + USER_PAGE_TABLE_SIZE;	
	
	s32 a = k / 8;
	s32 b = k % 8;

	i = 0;
	j = 0;

	for(; i<a; i++)
	{
		pages[i] 	= 0xFF;
	}

	for(; j<b; j++)
	{
		pages[i]	|= (1<<j);
	}

	//--------------------------------------------------------------------------------
	// update page tables
	//--------------------------------------------------------------------------------
	//(3) all the page tables are stored in USER_PAGE_TABLE_BASE(0xA01000)		
	u32 page_dir_base	= new_p_base;
	u32 page_table_base	= new_p_base + USER_PAGE_DIR_SIZE;
	
	page_set(page_dir_base, page_table_base, 0);
	
	j = (new_p_base + USER_PROC_RESERVE + total_size) / PAGE_SIZE + 1;

	u32 * page_table = new_p_base + USER_PAGE_DIR_SIZE;
	
	for(i=0; i<j; i++)
	{
		page_table[i] |= PG_EXIST;		
	}
	
	//--------------------------------------------------------------------------------
	// other new program attibutes setting
	//--------------------------------------------------------------------------------	
	new_p->page_dir_base	= new_p_base;
	new_p->page_tbl_base	= new_p_base + USER_PAGE_DIR_SIZE;
	new_p->pages_base	= new_p_base + USER_PAGE_DIR_SIZE + USER_PAGE_TABLE_SIZE;
	
	new_p->regs.ebx		= data_base_vaddr;
	
	new_p->regs.eax		= new_p_base;

	new_p->regs.ecx		= argc;
	new_p->pid		= new_p - proc_tab;
	new_p->regs.ebp		= new_p->pid;
	
	new_p->regs.esp 	= new_p_base + USER_PROC_RESERVE + total_size;
	new_p->regs.eip 	= new_p_base + USER_PROC_RESERVE;			//6MB

	//--------------------------------------------------------------------------------
	// virtual memory setting
	//--------------------------------------------------------------------------------
	new_p->mem_paddr	= new_p_base;
	
	new_p->mem_size_bytes	= USER_PROC_RESERVE + total_size;
	
	u32	paddr		= new_p_base + USER_PROC_PADDR;		// new_p_base + 0x5FFFFC
	*(u32*)(paddr)		= new_p_base;
	
	new_p->vmem_paddr	= new_p_base + USER_PAGE_DIR_SIZE + USER_PAGE_TABLE_SIZE + USER_PAGES_SIZE;

	struct s_vmem_map * vmap= new_p->vmem_paddr;
	
	u32	base_laddr	= new_p_base + desc_baseaddr_get(new_p->pid) + USER_PROC_RESERVE; 

	j = total_size / PAGE_SIZE + 1;

	for(i=0; i<j; i++, base_laddr = base_laddr + 4096)
	{
		vmap[i].laddr = base_laddr;
		vmap[i].hd_sec= sys_valloc(1);		
	}

	for(i=0; i<j; i++)
	{
		swapout(&vmap[i]);	// move the active page to harddisk	
	}

	//umap the pages that have moved to the harddisk
	j = (new_p_base + USER_PROC_RESERVE + total_size) / PAGE_SIZE + 1;
	i = (new_p_base + USER_PROC_RESERVE) / PAGE_SIZE ;

	u32	mask = 0xFFFFFFFF - 1;	//PG_N_EXIST
	for(; i<j; i++)
	{
		page_table[i] &= mask;		
	}
	
	new_p->status	 	= PROC_READY;		// 0
}

