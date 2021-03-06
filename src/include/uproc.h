//===========================================================================
// uproc.h
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
#ifndef	_X_UPROC_H_
#define	_X_UPROC_H_


u8	*p_uproc_stack;

u8	*hd_vem_map;

s8	mem_stratigic;

//-------------------------------------------------------------------------
// hard disk blocks management struct
//-------------------------------------------------------------------------
struct s_memm
{
	struct s_memm *head;
	struct s_memm *tail;
	u32	addr;				//start address
	s32	nr_bytes;			//size in bytes	
};

struct s_vmem_map
{
	u32	laddr;				//linear address in memory
	u32	hd_sec;				//sector number in harddisk
};


#define	PARM_NUM	16 		//16 parameters in max
#define	PARM_LEN	64 		//64 bytes

#define	UPROC_STACK_SIZE	0x800 	//2048 bytes

#define	MEM_SIZE_ADDR	0xFFFFF0	//which is written in memsize32.asm

#define	FREE_START_ADDR	0xa00000

//0x0  	   ~ 0x1000	(4KB)		: page dir
//0x1000   ~ 0x401000	(4MB)		: page table
//0x401000 ~ 0x421000	(128kB)		: pages status
//0x5FFFFC ~ 0x5FFFFF	(4 bytes)	: physical start address of user process
#define	USER_PROC_RESERVE	0x600000	//each user process start from 0x600000(-Ttext 0x600000).

//-------------------------------------------------------------------------
// virtual memory harddisk(0x40 ~ 0x83F, total is 1MB)
//-------------------------------------------------------------------------
#define	VMEM_HD_START_SEC	0x40	//
#define	VMEM_HD_END_SEC		0x83F	//

#define	VMEM_HD_MAP_NUM		32	// 1MB / 4KB = 256(pages); 256 / 8 = 32(bytes, each has 8 bits)


#endif
