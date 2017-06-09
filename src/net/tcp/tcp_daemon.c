//===========================================================================
// tcp_daemon.c
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
#include	"net.h"
#include	"tcp_daemon.h"
#include	"prototype.h"

PUBLIC	u8	tcp_daemon_stack[STACK_SIZE];

PRIVATE	void	tcp_out_queue();
PRIVATE	void	tcp_in_queue(struct s_mesg_package *msg, struct s_package* buf);
PRIVATE	s32	tcp_forward();

//===========================================================================
// mesg_init
//===========================================================================
PUBLIC	void	init_tcp_daemon()
{
	p_tcp_daemon_stack = tcp_daemon_stack;

	msg_tcpd.head	= &MSGQ_TCPD_BEGIN;
	msg_tcpd.tail	= &MSGQ_TCPD_BEGIN;
	msg_tcpd.count	= 0;

	empty_buf(&MSGQ_TCPD_BEGIN, (sizeof(struct s_mesg_package) * MSG_TCP_DAEMON_SIZE));

//------for test
	char *str = "--init tcp daemon--";
	disp_str(str);
//------for test	
}

//===========================================================================
// mesg_main
//===========================================================================
PUBLIC	void	tcp_daemon_main()
{
	struct s_mesg mesg;

	while(1)
	{
		struct s_mesg_package	msg;

		mesg.src_pid	= PROC_TCP_DAEMON;
		mesg.dest_pid 	= PROC_TCP_DAEMON;
		s32 flags 	= get_msg(&mesg);

		if(flags == HAVE_MSG)
		{			
			switch(mesg.type)
			{
				case TCP_SEND:
					tcp_in_queue(&msg, (struct s_package*)mesg.msg.buf1);
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

		if (msg_tcpd.count > 0)
		{
			if(msg_tcpd.head > &MSGQ_TCPD_END)
			{
				msg_tcpd.head	= &MSGQ_TCPD_BEGIN;
			}

			if((msg_tcpd.head)->flag == MSG_NEW)
			{
				tcp_forward();	
				continue;					
			}	
			msg_tcpd.head++;
		}
	}
}

//===========================================================================
// tcp_out_queue
//===========================================================================
PRIVATE	void	tcp_out_queue()
{
	if(msg_tcpd.head > &MSGQ_TCPD_END)
	{
		msg_tcpd.head	= &MSGQ_TCPD_BEGIN;
	}

	rtl8029_send_pkg(&((msg_tcpd.head)->pkg));

	(msg_tcpd.head)->flag	= MSG_EXPIRED;

	msg_tcpd.head++;
	msg_tcpd.count--;			
}

//===========================================================================
// tcp_in_queue
//===========================================================================
PRIVATE	void	tcp_in_queue(struct s_mesg_package *msg, struct s_package* buf)
{
	if(msg_tcpd.count > MSG_TCP_DAEMON_SIZE)
	{
		disp_str("mesg buffer is full");
			
		while(1)
		{};
	} 
	else
	{	
		s32 i;
		struct s_mesg_package *p = msg_tcpd.tail;

		for(i=0; i<=MSG_TCP_DAEMON_SIZE; i++, p++)
		{
			if(p > &MSGQ_TCPD_END)
			{
				p = &MSGQ_TCPD_BEGIN;
			}

			if(p->flag != MSG_NEW)
			{
				msg_tcpd.tail = p;
				break;
			}			
		}
	}

	if(msg_tcpd.tail > &MSGQ_TCPD_END)
	{
		msg_tcpd.tail	= &MSGQ_TCPD_BEGIN;
	}	

	strcpy(&(msg->pkg), buf, (buf->length + 2));
	strcpy(msg_tcpd.tail, msg, (msg->pkg.length + 2 + 4));

	(msg_tcpd.tail)->flag	= MSG_NEW;

	msg_tcpd.tail++;
	msg_tcpd.count++;	
}


//===========================================================================
// PRIVATE	s32	tcp_forward()
//===========================================================================
PRIVATE	s32	tcp_forward()
{
	tcp_out_queue();	
	return TRUE;		
}
