//===========================================================================
// ftpopen.c
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
#include	"net.h"
#include	"mesg.h"

//===========================================================================
// PUBLIC int reset_cmdline()
//===========================================================================
PUBLIC	s32 ftppasswd(s8 *ip)
{
	struct s_mesg mesg;
	
	mesg.msg.buf1	= ip;	
	mesg.dest_pid	= PROC_FTP;
	mesg.type	= FTP_PASSWD;
	mesg.func	= SYN;
		
	send_msg(&mesg);
}
