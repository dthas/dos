//===========================================================================
// sys.c
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
#include	"stdio.h"
#include	"kernel.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"proc.h"
#include	"mesg.h"
#include	"sys.h"
#include	"prototype.h"

PUBLIC	u8	sys_stack[STACK_SIZE];

//===========================================================================
// mesg_main
//===========================================================================
PUBLIC	void	sys_main()
{
	while (1)
	{
		struct s_mesg mesg;
		mesg.src_pid 	= PROC_SYS;
		mesg.dest_pid 	= PROC_SYS;
		s32 flags 	= get_msg(&mesg);

		if(flags == HAVE_MSG)
		{
			struct s_mesg answ;

			switch(mesg.type)
			{
				case SLEEP:	
					answ.dest_pid	= mesg.msg.parm1;
					answ.type 	= PROC_SLEEP;
					setstatus(&answ);
					break;
				case WAKEUP:	
					answ.dest_pid	= mesg.msg.parm1;
					answ.type 	= PROC_READY;
					setstatus(&answ);

					unblock(answ.dest_pid);
					break;
				case CHANGE_WORKDIR:	
				case PRINT_DIR:
					setattr(&mesg);
					break;					
				default:
					break;
			}

			if(mesg.func == SYN)
			{
				struct s_mesg mesg1;

				mesg1.dest_pid	= mesg.src_pid;
				mesg1.type	= PROC_READY;
		
				setstatus(&mesg1);
			}			
		}
		
		systime_set();					
	}
}

//===========================================================================
// init_sys
//===========================================================================
PUBLIC	void	init_sys()
{
//------for test
	char *str = "-init sys-";
	disp_str(str);	
//------for test
	
	p_sys_stack = sys_stack;

	systime_init();		//the time from cmos	
}



//===========================================================================
// sys_status(struct s_mesg *msg)
//===========================================================================
PUBLIC	void	sys_status(struct s_mesg *msg)
{
	proc_tab[msg->dest_pid].status = msg->type;

	if(msg->type == PROC_SLEEP)
	{
		schd();
	}
}

//===========================================================================
// sys_attr(struct s_mesg *msg)
//===========================================================================
PUBLIC	void	sys_attr(struct s_mesg *msg)
{
	msg = (struct s_mesg *)laddr_to_paddr(proc_ready->pid, msg);
	
	msg->msg.buf1 = (void *)laddr_to_paddr(proc_ready->pid, msg->msg.buf1);
	msg->msg.buf2 = (void *)laddr_to_paddr(proc_ready->pid, msg->msg.buf2);

	switch(msg->type)
	{
		case CHANGE_WORKDIR:
			change_dir(msg->msg.buf1, msg->msg.parm1);			
			break;
		case PRINT_DIR:
			print_dir();
			break;
		default:
			break;			
	}
}

//===========================================================================
// change_dir(s8 *dir, s32 len)
//===========================================================================
PUBLIC	void	print_dir()
{
	disp_str("working dir :");
	disp_str(proc_ready->workdir);
}

//===========================================================================
// change_dir(s8 *dir, s32 len)
//===========================================================================
PUBLIC	void	change_dir(s8 *dir, s32 len_s)
{
	s8 newdir[DIR_NAME_LEN];

	s32 len_d 	= strlen(proc_ready->workdir);

	if(strcmp("..", dir))
	{
		if(len_d > 2)
		{
			len_d	-= 2;			//-1 is /, -2 is the last character of the directory.
		}
		else
		{
			return 0;
		}

		s32 i;
		for(i=len_d; i>=0; i--)
		{
			if(proc_ready->workdir[i] == '/')
			{
				break;
			}
		}
		proc_ready->workdir[i+1] = NULL;
	}
	else
	{
		len_d	= strlen(proc_ready->workdir);
		len_s	= strlen(dir);

		s8 *p  	= proc_ready->workdir + len_d ;

		strcopy(p, dir);

		proc_ready->workdir[len_s + len_d + 1] = '/';
		proc_ready->workdir[len_s + len_d + 2] = NULL;
	}

	disp_str("now dir is changed to :");
	disp_str(proc_ready->workdir);
	
	
}


//===========================================================================
// sleep(s32 pid)
//===========================================================================
PUBLIC	void	sleep(s32 pid)
{
	struct s_mesg mesg;
	
	mesg.src_pid	= cpid();
	mesg.dest_pid	= PROC_SYS;
	mesg.type	= SLEEP;
	mesg.msg.parm1	= pid;	//the proc want to sleep
	mesg.func	= ASYN;
	send_msg(&mesg);

	block(pid);
}

//===========================================================================
// wakeup(s32 pid)
//===========================================================================
PUBLIC	void	wakeup(s32 pid)
{
	struct s_mesg mesg;

	mesg.src_pid	= cpid();
	mesg.dest_pid	= PROC_SYS;
	mesg.type	= WAKEUP;
	mesg.msg.parm1	= pid;	//the proc want to be waked up
	mesg.func	= ASYN;

	send_msg(&mesg);
	
}

//===========================================================================
// sleep(s32 pid)
//===========================================================================
PUBLIC	void	block(s32 pid)
{
	if((pid < 0) && (pid >= PROC_NUM))
	{
		disp_str("block::pid exceed!");
		
		while(1)
		{};
	}

	proc_tab[pid].flg_block = BLOCKING;		
}

//===========================================================================
// sleep(s32 pid)
//===========================================================================
PUBLIC	void	unblock(s32 pid)
{
	if((pid < 0) && (pid >= PROC_NUM))
	{
		disp_str("unblock::pid exceed!");
		
		while(1)
		{};
	}

	proc_tab[pid].flg_block = NO_BLOCK;
}
