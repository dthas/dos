//===========================================================================
// proc.h
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

#ifndef	_X_PROC_H_
#define	_X_PROC_H_

#define LDT_NUM		3
#define FILE_NUM	20

#define LDT_CODE	0
#define LDT_DATA	1
#define LDT_STACK	2

#define RPL_SYS		3
#define RPL_USER	3

#define PRIV_SYS	3
#define PRIV_USER	3

#define	SEL_TIL		4

#define	PRI_SYS		3
#define	PRI_USER	3

#define	NO_INIT		-1

#define PROC_NUM	40
#define PROC_SYS_NUM	17
#define PROC_USER_NUM	23
#define PROC_USER_TEST_NUM	0

#define	GDT_LDT_INDX	5
#define	ATTR_LDT	0x82	
#define	SEL_LDT		0x28


#define PRI_LEVEL_1	1
#define PRI_LEVEL_2	2
#define PRI_LEVEL_3	3
#define PRI_LEVEL_4	4
#define PRI_LEVEL_5	5
#define PRI_LEVEL_6	6
#define PRI_LEVEL_7	7
#define PRI_LEVEL_8	8
#define PRI_LEVEL_9	9
#define PRI_LEVEL_10	10
#define PRI_LEVEL_11	11
#define PRI_LEVEL_12	12
#define PRI_LEVEL_13	13
#define PRI_LEVEL_14	14
#define PRI_LEVEL_15	15
#define PRI_LEVEL_16	16

#define PRI_TICK_BASE	3

struct registers
{
	u32	eax;
	u32	ebx;
	u32	ecx;
	u32	edx;
	u32	esi;
	u32	edi;
	u32	ebp;
	
	u32	ds;
	u32	gs;
	u32	fs;
	u32	es;

	u32	temp;

	u32	eip;
	u32	cs;
	u32	eflags;
	u32	esp;
	u32	ss;
};


struct p_desc
{
	u16	limit_1;		// 0  ~ 15: lowest 16 bits of limit
	u16	base_1;			// 0  ~ 15: lowest 16 bits of base addr
	u8	base_2;			// 16 ~ 23: middle 8  bits of base addr
	u16	limit_2;		// 16 ~ 19: highest 4 bits of limit(include some attrbutes)
	u8	base_3;			// 24 ~ 31: highest 8 bits of base addr
};


struct proc
{
	struct registers	regs;	// registers for saving while switching
	u32	flg_block;		// current ldt selector


	u32	page_dir_base;
	u32	page_tbl_base;
	u32	pages_base;		// pages[], whichremark status of all the pages. Only user program need to initial.

	struct f_desc	files[FILE_NUM];// files that the process opened
	
	u32	pri;			// priority
	s32	pid;			// current pid
	s32	p_pid;			// parent pid
	u32	status;			// status is 0 while ready
	s32	ticks_max;		//  
	s32	ticks;			// ticks left(how much time left for this process)

	u32	start_time;		// process start time(by ticks)
	u32	pause_time;		// process pause time(by ticks)
	u32	sys_time;		// the ticks that process run in ring0
	u32	user_time;		// the ticks that process run in ring3

	u32	signal_set;		// if receive a signal, the corresponsing bit will be set in signal_set;
					// A process can receive 0 ~ 21 signals
	u32	signal_mask;		// 0: enable; 1: disable

	struct	s_mesg	msg;		// use for message
	u32	flags;			// 0: no message; 1: have message	

	u32	gdt_desc_addr;

	u32	mem_paddr;
	s32	mem_size_bytes;

	u32	vmem_paddr;
	
	s8	workdir[DIR_NAME_LEN];	// this is for a working directory		

};

struct proc *proc_ready;
struct proc proc_tab[PROC_NUM];

#endif
