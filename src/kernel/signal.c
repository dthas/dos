//===========================================================================
// signal.c
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
#include	"global.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"proc.h"
#include	"signal.h"
#include	"prototype.h"

//===========================================================================
// how to use: 
//	src	user process: 	(1) call en_sig/dis_sig to umask/mask a signal
//				(2) call set_sig/clear_sig to send/remove a signal to a process
//
//  	dest	user process:	(1) call sig_process to deal with a signal
//
//
//	kernel is dealing with the information of process table
//
//===========================================================================


//===========================================================================
// signal
//===========================================================================
PUBLIC	void sig_process(s32 pid)
{
	struct proc *p = proc_tab;
	s32 flg = FALSE;

	for(; p < &proc_tab[PROC_NUM]; p++)
	{
		if(p->pid == pid)
		{
			flg = TRUE;
			break;
		}
	}
	
	if(flg == FALSE)
	{
		disp_str("no such process:");
		disp_num(pid);
		return;
	}

	u32 sigx = p->signal_set;

	switch(sigx)
	{
		case	SIGHUPX:
			disp_str("-signal SIGHUP-");
			break;
		case	SIGINTX:
			disp_str("-signal SIGINT-");
			break;
		case	SIGQUTTX:
			disp_str("-signal SIGQUTT-");
			break;
		case	SIGILLX:
			disp_str("-signal SIGILL-");
			break;
		case	SIGABRTX:
			disp_str("-signal SIGABRT-");
			break;
		case	SIGFPEX:
			disp_str("-signal SIGFPE-");
			break;
		case	SIGKILLX:
			disp_str("-signal SIGKILL-");
			sigkill(pid, SIGKILL);
			break;
		case	SIGUSR1X:
			disp_str("-signal SIGUSR1-");
			break;
		case	SIGUSR2X:
			disp_str("-signal SIGUSR2-");
			break;
		case	SIGPIPEX:
			disp_str("signal SIGPIPE");
			break;
		case	SIGALRMX:
			disp_str("-signal SIGALRM-");
			break;
		case	SIGTERMX:
			disp_str("signal SIGTERM");
			break;
		case	SIGCHLDX:
			disp_str("-signal SIGCHLD-");
			break;
		case	SIGCONTX:
			disp_str("-signal SIGCONT-");
			break;
		case	SIGSTOPX:
			disp_str("-signal SIGSTOP-");
			break;
		case	SIGTSTPX:
			disp_str("-signal SIGTSTP-");
			break;
		case	SIGTTINX:
			disp_str("-signal SIGTTIN-");
			break;
		case	SIGTTOUX:
			disp_str("-signal SIGTTOU-");
			break;
		case	SIGKMESSX:
			disp_str("-signal SIGKMESS-");
			break;
		case	SIGKSIGX:
			disp_str("-signal SIGKSIG-");
			break;
		case	SIGKSTOPX:
			disp_str("-signal SIGKSTOP-");
			break;

		default:
			break;
	}

	
}


//===========================================================================
// sigkill
//===========================================================================
PUBLIC	void sigkill(s32 pid, u32 sig)
{
	if(sig_mask_check(pid, sig) == FALSE)
	{
		disp_str("(process:");
		disp_num(pid);
		disp_str(", sig:");
		disp_num(sig);
		disp_str(" is mask)"); 
	}
	else
	{
		disp_str("(kill process:");
		disp_num(pid);
		disp_str(" success)");
	}

	clear_sig(pid, sig);
}

//===========================================================================
// en_sig(sig is signal number in signal.h, which is in 0 ~ 21)
// en_sig will set the sig bit in signal_mask to 0(0 : enable)
//===========================================================================
PUBLIC	void en_sig(s32 pid, u32 sig)
{
	struct proc *p = proc_tab;
	s32 flg = FALSE;

	for(; p < &proc_tab[PROC_NUM]; p++)
	{
		if(p->pid == pid)
		{
			flg = TRUE;
			break;
		}
	}
	
	if(flg == FALSE)
	{
		disp_str("no process:");
		disp_num(pid);
		return;
	}

	u32 active_bit 	= 1 << sig;
	active_bit	= 0xFFFFFFFF - active_bit;
 
	p->signal_mask 	&= active_bit;		
}

//===========================================================================
// dis_sig(sig is signal number in signal.h, which is in 0 ~ 21)
// dis_sig will set the sig bit in signal_mask to 1(1 : disable)
//===========================================================================
PUBLIC	void dis_sig(s32 pid, u32 sig)
{
	struct proc *p = proc_tab;
	s32 flg = FALSE;

	for(; p < &proc_tab[PROC_NUM]; p++)
	{
		if(p->pid == pid)
		{
			flg = TRUE;
			break;
		}
	}
	
	if(flg == FALSE)
	{
		disp_str("no process:");
		disp_num(pid);
		return;
	}

	u32 active_bit 	= 1 << sig;
	 
	p->signal_mask 	|= active_bit;		
}


//===========================================================================
// clear_sig(sig is signal number in signal.h, which is in 0 ~ 21)
// clear_sig will set the sig bit in signal_set to 0(0 : no signal with the number is sig need to deal with)
//===========================================================================
PUBLIC	void clear_sig(s32 pid, u32 sig)
{
	struct proc *p = proc_tab;
	s32 flg = FALSE;

	for(; p < &proc_tab[PROC_NUM]; p++)
	{
		if(p->pid == pid)
		{
			flg = TRUE;
			break;
		}
	}
	
	if(flg == FALSE)
	{
		disp_str("no process:");
		disp_num(pid);
		return;
	}

	u32 active_bit 	= 1 << sig;
	active_bit	= 0xFFFFFFFF - active_bit;
 
	p->signal_set 	&= active_bit;		
}

//===========================================================================
// set_sig(sig is signal number in signal.h, which is in 0 ~ 21)
// set_sig will set the sig bit in signal_set to 1(1 : a signal with the number is sig need to deal with)
//===========================================================================
PUBLIC	void set_sig(s32 pid, u32 sig)
{
	struct proc *p = proc_tab;
	s32 flg = FALSE;

	for(; p < &proc_tab[PROC_NUM]; p++)
	{
		if(p->pid == pid)
		{
			flg = TRUE;
			break;
		}
	}
	
	if(flg == FALSE)
	{
		disp_str("no process:");
		disp_num(pid);
		return;
	}

	u32 active_bit 	= 1 << sig;
	 
	p->signal_set 	|= active_bit;		
}


//===========================================================================
// sigkill
//===========================================================================
PUBLIC	u8 sig_mask_check(s32 pid, u32 sig)
{
	struct proc *p = proc_tab;
	s32 flg = FALSE;

	for(; p < &proc_tab[PROC_NUM]; p++)
	{
		if(p->pid == pid)
		{
			flg = TRUE;
			break;
		}
	}
	
	if(flg == FALSE)
	{
		disp_str("no this process:");
		disp_num(pid);
		return;
	}

	u32 active_bit 	= 1 << sig;
	 
	active_bit	&= p->signal_mask ;	

	if(active_bit == 0)
	{
		return	TRUE;
	}
	else
	{
		return 	FALSE;
	}	
}
