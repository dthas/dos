//===========================================================================
// mesg.h
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

#ifndef	_X_MESG_H_
#define	_X_MESG_H_

#define	MSG_INPUT_SIZE	128		// each mesg is 32 bytes(struct	s_mesg), so 128 * 32 = 4096

u8	*p_msg_stack;

//-------------------------------------------------------------------------
// message queue buffer
//------------------------------------------------------------------------- 
struct mesg_q {
	struct	s_mesg*	head;
	struct	s_mesg*	tail;			
	s32	count;			
	struct	s_mesg	msg[MSG_INPUT_SIZE];	
};

struct mesg_q	msg_q;

#define	MSGQ_BEGIN	(msg_q.msg[0])
#define	MSGQ_END	(msg_q.msg[MSG_INPUT_SIZE - 1])

#endif
