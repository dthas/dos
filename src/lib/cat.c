//===========================================================================
// cat.c
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
#include	"prototype.h"
#include	"kernel.h"
#include	"global.h"
#include	"mesg.h"

//===========================================================================
// PUBLIC s32 send_msg(struct s_mesg *mesg)
//===========================================================================
PUBLIC	s32 cat(s8 *file_name, s8 *data, s8 mode)
{
	struct s_mesg mesg;

	mesg.msg.buf1	= file_name;
	mesg.msg.buf2	= data;
	mesg.msg.parm1	= mode;
	mesg.dest_pid	= PROC_FS;
	mesg.type	= FS_CAT;
	mesg.func	= SYN;	
	
	send_msg(&mesg);
}
