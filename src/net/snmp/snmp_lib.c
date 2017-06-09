//===========================================================================
// snmp_lib.c
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
#include	"stdio.h"
#include	"signal.h"
#include	"kernel.h"
#include	"global.h"
#include	"sys.h"
#include	"tty.h"
#include	"keyboard.h"
#include	"mesg.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"proc.h"
#include	"rtl8029.h"
#include	"frame.h"
#include	"net.h"
#include	"route.h"
#include	"ipv4.h"
#include	"udp.h"
#include	"snmp.h"
#include	"prototype.h"

//===========================================================================
// send getrequest
//===========================================================================
PUBLIC	s32 	getrequest(struct iaddr dst_ip, struct s_package *dst_pkg)
{
	s8* snmp		= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]);

	u16 total_len		= 0;

	//------------------------------------------------------------------
	// whole packet
	//------------------------------------------------------------------
	snmp[total_len]		= 0x30;
	snmp[total_len + 1]	= 0x6d;
	
	total_len		+= 2;
	//------------------------------------------------------------------
	// version
	//------------------------------------------------------------------
	snmp[total_len]		= 0x2;
	snmp[total_len + 1]	= 0x1;
	snmp[total_len + 2]	= 0x3;

	total_len		+= 3;
	//-------------------------------------------------------------------------
	// global data
	//-------------------------------------------------------------------------

	// part information
	snmp[total_len]		= 0x30;
	snmp[total_len + 1]	= 0xd;

	total_len		+= 2;

	// id
	snmp[total_len]		= 0x2;
	snmp[total_len + 1]	= 0x1;
	snmp[total_len + 2]	= 0x40;

	total_len		+= 3;

	// max size
	snmp[total_len]		= 0x2;
	snmp[total_len + 1]	= 0x2;
	snmp[total_len + 2]	= 0x4;
	snmp[total_len + 3]	= 0x0;

	total_len		+= 4;

	// flag
	snmp[total_len]		= 0x4;
	snmp[total_len + 1]	= 0x1;
	snmp[total_len + 2]	= 0x4;
	
	total_len		+= 3;

	// safe mode
	snmp[total_len]		= 0x2;
	snmp[total_len + 1]	= 0x1;
	snmp[total_len + 2]	= 0x3;
	
	total_len		+= 3;
	
	//-------------------------------------------------------------------------
	// safe parm
	//-------------------------------------------------------------------------

	// part information
	snmp[total_len]		= 0x30;
	snmp[total_len + 1]	= 0x28;

	total_len		+= 2;

	s32 i;
	for(i=0; i<10; i++)
	{
		snmp[total_len]		= 0x04;
		snmp[total_len + 1]	= 0x02;
		snmp[total_len + 2]	= 0x41;
		snmp[total_len + 3]	= 0x42;

		total_len		+= 0x4;

	}
	

	//-------------------------------------------------------------------------
	// scope pdu
	//-------------------------------------------------------------------------

	// part information
	snmp[total_len]		= 0x30;
	snmp[total_len + 1]	= 0x2f;

	total_len		+= 2;

	//contexEngine
	snmp[total_len]		= 0x4;
	snmp[total_len + 1]	= 0xc;
	snmp[total_len + 2]	= 0x0;
	snmp[total_len + 3]	= 0x0;
	snmp[total_len + 4]	= 0x0;
	snmp[total_len + 5]	= 0x61;
	snmp[total_len + 6]	= 0x2;
	snmp[total_len + 7]	= 0x0;
	snmp[total_len + 8]	= 0x0;
	snmp[total_len + 9]	= 0xb2;
	snmp[total_len + 10]	= 0xc1;
	snmp[total_len + 11]	= 0x41;
	snmp[total_len + 12]	= 0x82;
	snmp[total_len + 13]	= 0x20;
	
	total_len		+= 14;

	//contexName
	snmp[total_len]		= 0x4;
	snmp[total_len + 1]	= 0x0;

	total_len		+= 2;

	//-------------------------------------------------------------------------
	// pdu
	//-------------------------------------------------------------------------

	// part information
	snmp[total_len]		= 0xA0;
	snmp[total_len + 1]	= 0x1d;

	total_len		+= 2;

	// require id
	snmp[total_len]		= 0x2;
	snmp[total_len + 1]	= 0x4;
	snmp[total_len + 2]	= 0x0;
	snmp[total_len + 3]	= 0x1;
	snmp[total_len + 4]	= 0x6;
	snmp[total_len + 5]	= 0x11;

	total_len		+= 6;

	// error state
	snmp[total_len]		= 0x2;
	snmp[total_len + 1]	= 0x1;
	snmp[total_len + 2]	= 0x0;
	
	total_len		+= 3;

	// error index
	snmp[total_len]		= 0x2;
	snmp[total_len + 1]	= 0x1;
	snmp[total_len + 2]	= 0x0;
	
	total_len		+= 3;

	// var binding part information
	snmp[total_len]		= 0x30;
	snmp[total_len + 1]	= 0xf;
		
	total_len		+= 2;

	// var binding
	snmp[total_len]		= 0x30;
	snmp[total_len + 1]	= 0xd;
		
	total_len		+= 2;

	// var
	snmp[total_len]		= 0x6;
	snmp[total_len + 1]	= 0x9;
	snmp[total_len + 2]	= 0x1;
	snmp[total_len + 3]	= 0x3;
	snmp[total_len + 4]	= 0x6;
	snmp[total_len + 5]	= 0x1;
	snmp[total_len + 6]	= 0x2;
	snmp[total_len + 7]	= 0x1;
	snmp[total_len + 8]	= 0x7;
	snmp[total_len + 9]	= 0x1;
	snmp[total_len + 10]	= 0x0;
	
	total_len		+= 11;

	// value
	snmp[total_len]		= 0x5;
	snmp[total_len + 1]	= 0x0;
		
	total_len		+= 2;	

	//-------------------------------------------------------------------------
	// snmp state change
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_GETREQUEST;
	snmp_state	= *(p_snmp_state_tab + snmp_state * SNMP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= UDP_HEADER_LENGTH;	

	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= get_port();
	s16 dst_port	= PORT_SNMP_SERVER;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}
