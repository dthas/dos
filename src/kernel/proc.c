//===========================================================================
// proc.c
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
#include	"signal.h"
#include	"kernel.h"
#include	"global.h"
#include	"sys.h"
#include	"test.h"
#include	"tty.h"
#include	"mesg.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"uproc.h"
#include	"rtl8029.h"
#include	"net.h"
#include	"ftp.h"
#include	"tcp_daemon.h"
#include	"tftp.h"
#include	"dns.h"
#include	"dhcp.h"
#include	"snmp.h"
#include	"http.h"
#include	"smtp.h"
#include	"pop3.h"
#include	"telnet.h"
#include	"partition.h"
#include	"proc.h"
#include	"prototype.h"

extern	cur_pid;
extern	gdt_desc;
//===========================================================================
// init process table
// 
// there 2 level using in DTHAS, which is ring0 and ring3:
// 
// 1)ring0: kernel
//
// 2)ring3: 
//		 i) sys  process(mesg, tty, hd, fs, sys, uproc, net)
//		ii) user process(other process created later)
//
//===========================================================================
PUBLIC	void	init_proc()
{
	struct proc p;
	struct proc *ptab = proc_tab;
	s32 i;
	
	//-------------------------------------------------------------------------
	// init proc_tab
	//-------------------------------------------------------------------------
	empty_buf(proc_tab, PROC_NUM * sizeof(struct proc));


	//-------------------------------------------------------------------------
	// init system processes(0~6). All the sys process are in ring3(not ring1 or ring2)
	//-------------------------------------------------------------------------
	init_sys_proc(mesg_main, ptab, p_msg_stack, PROC_MSG);		//0
	ptab++;	
		
	init_sys_proc(tty_main, ptab, p_tty_stack, PROC_TTY);		//1
	ptab++;

	init_sys_proc(hd_main, ptab, p_hd_stack, PROC_HD);		//2
	ptab++;

	init_sys_proc(vfs_main, ptab, p_fs_stack, PROC_FS);		//3
	ptab++;

	init_sys_proc(sys_main, ptab, p_sys_stack, PROC_SYS);		//4
	ptab++;

	init_sys_proc(uproc_main, ptab, p_uproc_stack, PROC_UPROC);	//5
	ptab++;
	
	init_sys_proc(net_main, ptab, p_net_stack, PROC_NET);		//6
	ptab++;

	init_sys_proc(ftp_main, ptab, p_ftp_stack, PROC_FTP);		//7
	ptab++;

	init_sys_proc(tcp_daemon_main, ptab, p_tcp_daemon_stack, PROC_TCP_DAEMON);	//8
	ptab++;

	init_sys_proc(tftp_main, ptab, p_tftp_stack, PROC_TFTP);	//9
	ptab++;

	init_sys_proc(dns_main, ptab, p_dns_stack, PROC_DNS);		//10
	ptab++;

	init_sys_proc(dhcp_main, ptab, p_ddhcp_stack, PROC_DHCP);	//11
	ptab++;

	init_sys_proc(snmp_main, ptab, p_snmp_stack, PROC_SNMP);	//12
	ptab++;

	init_sys_proc(http_main, ptab, p_hhttp_stack, PROC_HTTP);	//13
	ptab++;

	init_sys_proc(smtp_main, ptab, p_smtp_stack, PROC_SMTP);	//14
	ptab++;

	init_sys_proc(pop3_main, ptab, p_pop3_stack, PROC_POP3);	//15
	ptab++;

	init_sys_proc(telnet_main, ptab, p_telnet_stack, PROC_TELNET);	//16
	ptab++;
	
	//-------------------------------------------------------------------------
	// init user processes(7~39)
	//-------------------------------------------------------------------------
	for(i = PROC_SYS_NUM; i < PROC_NUM; i++, ptab++)
	{
		init_default_proc(ptab, i);
	}
	
	//-------------------------------------------------------------------------
	// special setting
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// init variable
	//-------------------------------------------------------------------------
	proc_ready 	= proc_tab;
	
	s8 * s = "-init proc-";
	disp_str(s);	
}

//===========================================================================
// init_sys_proc(u32 proc_addr, struct proc *p)
//===========================================================================
PUBLIC	void	init_sys_proc(proc_t proc_addr, struct proc *p, u8 * stack, s32 seq_num)
{
	u32	i;

	//-------------------------------------------------------------------------
	// init registers
	//-------------------------------------------------------------------------
	p->regs.eax 	= 0;
	p->regs.ebx 	= 0;
	p->regs.ecx 	= 0;
	p->regs.edx 	= 0;
	p->regs.esi 	= 0;
	p->regs.edi 	= 0;
	p->regs.ebp 	= 0;
	p->regs.esp 	= (u32)stack;
	p->regs.eip 	= proc_addr;

	// each process has 3 desc, which is code_desc(0), data_desc(8), stack_desc(16). Total size of 3 desc is 24 
	// bytes(DESC_SIZE * 3). DESC_SIZE * 3 + 8 is data_desc
	p->regs.cs 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3) | RPL_SYS;
	p->regs.ds 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_SYS;
	p->regs.es 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_SYS;
	p->regs.fs 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_SYS;
	p->regs.ss 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_SYS;
	p->regs.gs 	= SEL_DISP & 0xFFFC | RPL_SYS;

	//modi on 2012-7-20
	//As CPL<=IOPL, so while SYS process in ring1(CPL=1),IOPL should be >= 1, then eflags is 0x1202
	//While SYS process in ring3(CPL=3), IOPL should be >= 3, then eflags is 0x3202
	//p->regs.eflags	= 0x1202;
	p->regs.eflags	= 0x3202;	

	//-------------------------------------------------------------------------
	// init files
	//-------------------------------------------------------------------------
	for (i = 0; i < FILE_NUM; i++)
	{
		p->files[i].mode 	= 0;
		p->files[i].cur_pos 	= 0;
		p->files[i].nr_links 	= 0;
		p->files[i].inode 	= 0;
	}

	//-------------------------------------------------------------------------
	// init other attributes
	//-------------------------------------------------------------------------
	p->flg_block	= NO_BLOCK;	

	p->pri	 	= PRI_LEVEL_2;			// 10 ticks
	p->ticks_max	= PRI_LEVEL_2 * PRI_TICK_BASE;
	p->ticks 	= PRI_LEVEL_2 * PRI_TICK_BASE;	// 20 ticks

	p->pid		= cur_pid++;		// cur_pid autoadd 1
	p->p_pid	= NO_INIT;
	p->status 	= PROC_READY;		// 0	

	p->start_time	= 0;
	p->pause_time	= 0;
	p->sys_time	= 0;
	p->user_time	= 0;

	p->signal_set	= 0;
	p->signal_mask	= 0;			// enable all the signals;

	p->flags	= NO_MSG;

	p->page_dir_base= KERNEL_PAGE_DIR_BASE;
	p->page_tbl_base= KERNEL_PAGE_TABLE_BASE;

	p->gdt_desc_addr= gdt_desc;

	p->mem_paddr	= 0;
	p->mem_size_bytes=0;
}

//===========================================================================
// PUBLIC	void	init_user_proc(proc_t proc_addr, struct proc *p, u8 * stack, s32 seq_num)
//===========================================================================
PUBLIC	void	init_user_proc(proc_t proc_addr, struct proc *p, u8 * stack, s32 seq_num)
{
	u32	i;

	//-------------------------------------------------------------------------
	// init registers
	//-------------------------------------------------------------------------
	p->regs.eax 	= 0;
	p->regs.ebx 	= 0;
	p->regs.ecx 	= 0;
	p->regs.edx 	= 0;
	p->regs.esi 	= 0;
	p->regs.edi 	= 0;
	p->regs.ebp 	= 0;
	p->regs.esp 	= (u32)stack;
	p->regs.eip 	= proc_addr;

	// each process has 3 desc, which is code_desc(0), data_desc(8), stack_desc(16). Total size of 3 desc is 24 
	// bytes(DESC_SIZE * 3). DESC_SIZE * 3 + 8 is data_desc
	p->regs.cs 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3) | RPL_USER;
	p->regs.ds 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_USER;
	p->regs.es 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_USER;
	p->regs.fs 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_USER;
	p->regs.ss 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_USER;
	p->regs.gs 	= SEL_DISP & 0xFFFC | RPL_USER;

	p->regs.eflags	= 0x202;

	//-------------------------------------------------------------------------
	// init files
	//-------------------------------------------------------------------------
	for (i = 0; i < FILE_NUM; i++)
	{
		p->files[i].mode 	= 0;
		p->files[i].cur_pos 	= 0;
		p->files[i].nr_links 	= 0;
		p->files[i].inode 	= 0;
	}

	//-------------------------------------------------------------------------
	// init other attributes
	//-------------------------------------------------------------------------
	p->flg_block	= NO_BLOCK;	

	p->pri	 	= PRI_LEVEL_1;			// 10 ticks
	p->ticks_max	= PRI_LEVEL_1 * PRI_TICK_BASE;
	p->ticks 	= PRI_LEVEL_1 * PRI_TICK_BASE;	// 20 ticks

	p->pid		= cur_pid++;		// cur_pid autoadd 1
	p->p_pid	= NO_INIT;
	p->status 	= PROC_READY;		// 0

	p->start_time	= 0;
	p->pause_time	= 0;
	p->sys_time	= 0;
	p->user_time	= 0;

	p->signal_set	= 0;
	p->signal_mask	= 0;			// enable all the signals;

	p->flags	= NO_MSG;
	
	p->page_dir_base= NO_INIT;
	p->page_tbl_base= NO_INIT;

	p->gdt_desc_addr= gdt_desc;

	p->mem_paddr	= 0;
	p->mem_size_bytes=0;

	p->pages_base	= NO_INIT;
}

//===========================================================================
// PUBLIC	void	init_default_proc(struct proc *p, s32 seq_num)
//===========================================================================
PUBLIC	void	init_default_proc(struct proc *p, s32 seq_num)
{
	u32	i;

	//-------------------------------------------------------------------------
	// init registers
	//-------------------------------------------------------------------------
	p->regs.eax 	= 0;
	p->regs.ebx 	= 0;
	p->regs.ecx 	= 0;
	p->regs.edx 	= 0;
	p->regs.esi 	= 0;
	p->regs.edi 	= 0;
	p->regs.ebp 	= 0;

	// each process has 3 desc, which is code_desc(0), data_desc(8), stack_desc(16). Total size of 3 desc is 24 
	// bytes(DESC_SIZE * 3). DESC_SIZE * 3 + 8 is data_desc
	p->regs.cs 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3) | RPL_USER;
	p->regs.ds 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_USER;
	p->regs.es 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_USER;
	p->regs.fs 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_USER;
	p->regs.ss 	= (SEL_RPOC_FIRST + seq_num * DESC_SIZE * 3 + 8) | RPL_USER;
	p->regs.gs 	= SEL_DISP & 0xFFFC | RPL_USER;

	p->regs.eflags	= 0x202;

	//-------------------------------------------------------------------------
	// init files
	//-------------------------------------------------------------------------
	for (i = 0; i < FILE_NUM; i++)
	{
		p->files[i].mode 	= 0;
		p->files[i].cur_pos 	= 0;
		p->files[i].nr_links 	= 0;
		p->files[i].inode 	= 0;
	}

	//-------------------------------------------------------------------------
	// init other attributes
	//-------------------------------------------------------------------------
	p->flg_block	= NO_BLOCK;
	
	p->pri	 	= PRI_LEVEL_1;			// 10 ticks
	p->ticks_max	= PRI_LEVEL_1 * PRI_TICK_BASE;
	p->ticks 	= PRI_LEVEL_1 * PRI_TICK_BASE;	// 20 ticks
	
	p->pid		= NO_INIT;		// -1
	p->p_pid	= NO_INIT;
	p->status 	= NO_INIT;		// -1	

	p->start_time	= 0;
	p->pause_time	= 0;
	p->sys_time	= 0;
	p->user_time	= 0;

	p->signal_set	= 0;
	p->signal_mask	= 0;			// enable all the signals;

	p->flags	= NO_MSG;
	
	p->page_dir_base= NO_INIT;
	p->page_tbl_base= NO_INIT;

	p->gdt_desc_addr= gdt_desc;

	p->mem_paddr	= 0;
	p->mem_size_bytes=0;

	p->pages_base	= NO_INIT;
}

//===========================================================================
// laddr_to_paddr
//===========================================================================
PUBLIC	u32	laddr_to_paddr(u32 pid, u32 laddr)
{
	u32 base_addr 	= desc_baseaddr_get(pid);

	u32 paddr	= base_addr + laddr;

	return paddr;
}

//===========================================================================
// laddr_to_paddr
//===========================================================================
PUBLIC	u32	paddr_to_laddr(u32 pid, u32 paddr)
{
	u32 base_addr 	= desc_baseaddr_get(pid);
	u32 laddr	= paddr - base_addr ;

	return laddr;
}
