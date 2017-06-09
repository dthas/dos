//===========================================================================
// traceroutes.c
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
#include	"traceroute.h"
#include	"prototype.h"

//===========================================================================
// PUBLIC	void 	sys_traceroute(struct iaddr dst_ip)
//===========================================================================
PUBLIC	void 	sys_traceroute(struct s_package *pkg, struct iaddr dst_ip)
{
	//-------------------------------------------------------------------------
	// check routes in route table
	//-------------------------------------------------------------------------
	struct route_table *rtl = search_route(dst_ip);

	if(!rtl)
	{
		printk("traceroute route check fail");
		printk("\n");

		return;
	}

	//-------------------------------------------------------------------------
	// add udp header and udp content
	//-------------------------------------------------------------------------
	s16 udp_len	= 40;	//suppose

	s32 udp_s	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH; //14+20+8=42
	s32 udp_e	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + udp_len;

	s32 i;
	for(i=udp_s; i<udp_e; i++)
	{
		pkg->buffer[i] = 1;
	}

	s16 src_port	= TRACEROUTE_SRC_PORT;
	s16 dst_port	= TRACEROUTE_DST_PORT;

	add_udp_header(pkg, pnet_dev->ip, udp_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	
	
	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	u16 ip_len	= big_little_16(IPV4_HEADER_LENGTH + udp_len); 

	add_ipv4_header(pkg, pnet_dev->ip, IP_TTL, dst_ip, PROTOCOL_UDP, 0, ip_len, 0,  IP_FLAG_NO_FRAGMENT);    //TOS of traceroute is 0

	//-------------------------------------------------------------------------
	// add frame header
	//-------------------------------------------------------------------------
	add_frame_header(pkg, FRAME_IP_TYPE, rtl->gw_mac, pnet_dev->mac);

	//-------------------------------------------------------------------------
	// setting the whole package length
	//-------------------------------------------------------------------------
	pkg->length 	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + udp_len; //512 + 14	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);
}
