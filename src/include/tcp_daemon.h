//===========================================================================
// tcp_daemon.h
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

#ifndef	_X_TCP_DAEMON_H_
#define	_X_TCP_DAEMON_H_

u8	*p_tcp_daemon_stack;

#define	MSG_TCP_DAEMON_SIZE	128		// each mesg is 1518 bytes(struct	s_mesg), so 128 * 1518 = 194304(190k)

struct	s_mesg_package
{
	s32	flag;		//if the mesg has sent, then flag is EXPIRED

	struct s_package	pkg;
};

//-------------------------------------------------------------------------
// message queue buffer
//------------------------------------------------------------------------- 
struct mesg_tcp_q {
	struct s_mesg_package*	head;
	struct s_mesg_package*	tail;			
	s32	count;			
	struct s_mesg_package	msg[MSG_TCP_DAEMON_SIZE];	
};

struct mesg_tcp_q	msg_tcpd;

#define	MSGQ_TCPD_BEGIN	(msg_tcpd.msg[0])
#define	MSGQ_TCPD_END	(msg_tcpd.msg[MSG_TCP_DAEMON_SIZE - 1])


#endif
