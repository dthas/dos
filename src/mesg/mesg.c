//===========================================================================
// mesg.c
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
#include	"prototype.h"

PUBLIC	u8	msg_stack[STACK_SIZE];

//===========================================================================
// mesg_init
//===========================================================================
PUBLIC	void	init_mesg()
{
	p_msg_stack = msg_stack;

	msg_q.head	= &MSGQ_BEGIN;
	msg_q.tail	= &MSGQ_BEGIN;
	msg_q.count	= 0;

	empty_buf(&MSGQ_BEGIN, (sizeof(struct s_mesg) * MSG_INPUT_SIZE));

//------for test
	char *str = "--init msg--";
	disp_str(str);	
//------for test	
}

//===========================================================================
// mesg_main
//===========================================================================
PUBLIC	void	mesg_main()
{
	while(1)
	{
		if (msg_q.count > 0)
		{
			if(msg_q.head > &MSGQ_END)
			{
				msg_q.head	= &MSGQ_BEGIN;
			}

			if((msg_q.head)->flag == MSG_NEW)
			{
				if(proc_tab[(msg_q.head)->dest_pid].flags == NO_MSG)
				{
					forward();	
					continue;	
				}
			}	
			msg_q.head++;
		}
	}
}

//===========================================================================
// out_queue
//===========================================================================
PUBLIC	void	out_queue(struct s_mesg *msg)
{
	if(msg_q.head > &MSGQ_END)
	{
		msg_q.head	= &MSGQ_BEGIN;
	}

	strcpy(msg, msg_q.head, sizeof(struct s_mesg));

	(msg_q.head)->flag	= MSG_EXPIRED;

	msg_q.head++;
	msg_q.count--;			
}

//===========================================================================
// in_queue
//===========================================================================
PUBLIC	void	in_queue(struct s_mesg *msg)
{
	if(msg_q.count > MSG_INPUT_SIZE)
	{
		disp_str("mesg buffer is full");
			
		while(1)
		{};
	} 
	else
	{	
		s32 i;
		struct s_mesg *p = msg_q.tail;

		for(i=0; i<=MSG_INPUT_SIZE; i++, p++)
		{
			if(p > &MSGQ_END)
			{
				p = &MSGQ_BEGIN;
			}

			if(p->flag != MSG_NEW)
			{
				msg_q.tail = p;
				break;
			}			
		}
	}

	if(msg_q.tail > &MSGQ_END)
	{
		msg_q.tail	= &MSGQ_BEGIN;
	}	

	strcpy(msg_q.tail, msg, sizeof(struct s_mesg));

	(msg_q.tail)->flag	= MSG_NEW;

	msg_q.tail++;
	msg_q.count++;	
}

//===========================================================================
// 0: sys_send
//===========================================================================
PUBLIC 	int 	sys_send(struct s_mesg *msg)
{
	msg = (struct s_mesg *)laddr_to_paddr(proc_ready->pid, msg);
	
	msg->msg.buf1 = (void *)laddr_to_paddr(proc_ready->pid, msg->msg.buf1);
	msg->msg.buf2 = (void *)laddr_to_paddr(proc_ready->pid, msg->msg.buf2);

	msg->src_pid	= proc_ready->pid;

	in_queue(msg);

	if(msg->func == SYN)
	{
		proc_tab[msg->src_pid].status = PROC_SLEEP;
		schd();
	}
}

//===========================================================================
// 1: sys_forward
//===========================================================================
PUBLIC	s32	sys_forward()
{
	u32 pid	= (msg_q.head)->dest_pid;

	proc_tab[pid].flags = HAVE_MSG;

	struct s_mesg *msg = (struct s_mesg *)laddr_to_paddr(PROC_MSG, &(proc_tab[pid].msg));
	out_queue(msg);	// copy the message to the dest proc_table of the dest process	

	return TRUE;	
}

//===========================================================================
// 1: sys_receive
//===========================================================================
PUBLIC	s32	sys_receive(struct s_mesg *msg)
{
	struct s_mesg *p= &(proc_tab[msg->dest_pid].msg);
	
	struct s_mesg *src = (struct s_mesg *)laddr_to_paddr(PROC_MSG, p);	
	
	u32 dest_pid	= src->dest_pid;
	u32 src_pid	= src->src_pid;

	u32 *dst = laddr_to_paddr(dest_pid, msg);
	
	strcpy(dst, src, sizeof(struct s_mesg));	// copy the message from proc_table

	proc_tab[dest_pid].flags = NO_MSG; 
}
