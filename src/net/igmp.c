//===========================================================================
// igmp.c
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
#include	"igmp.h"
#include	"prototype.h"

//===========================================================================
// PUBLIC	void 	sys_traceroute(struct iaddr dst_ip)
//===========================================================================
PUBLIC	void 	igmp_process(struct s_package *pkg, u8 type, u8 code)
{
	switch(type)
	{
		case IGMP3:
			igmp3_process(pkg, code);
			break;
		default:
			break;
	}
}

//===========================================================================
// PUBLIC	void 	sys_traceroute(struct iaddr dst_ip)
//===========================================================================
PUBLIC	void 	igmp3_process(struct s_package *pkg, u8 code)
{
	struct iaddr dst_ip;
	dst_ip.addr1	= 0;
	dst_ip.addr2	= 0;
	dst_ip.addr3	= 0;
	dst_ip.addr4	= 0;

	//-------------------------------------------------------------------------
	// add udp content
	//-------------------------------------------------------------------------
	struct s_igmp_query_h * igmph	= (struct s_igmp_query_h *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));
	struct iaddr * igmpc_addi_ip	= (struct iaddr *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + IGMP_H_LEN]));

	igmph->type		= code;
	igmph->res_code		= 1;
	igmph->chksum		= 0;

	igmph->group_addr.addr1 = 224;
	igmph->group_addr.addr2 = 0;
	igmph->group_addr.addr3 = 0;
	igmph->group_addr.addr4 = 22;

	igmph->resv_s_qrv	= (IGMP_QUERY_RESV << 4) | (IGMP_QUERY_S << 3) | IGMP_QUERY_QRV;

	igmph->qqic		= 1;
	igmph->nr_src		= big_little_16(0);

	s16 igmp_len		= IGMP_H_LEN;
	igmph->chksum		= makechksum(igmph,igmp_len);

	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	u16 ip_len	= big_little_16(IPV4_HEADER_LENGTH + igmp_len); 

	add_ipv4_header(pkg, pnet_dev->ip, IP_TTL, dst_ip, PROTOCOL_IGMP, 0, ip_len, 0,  IP_FLAG_NO_FRAGMENT);    //TOS of traceroute is 0

	//-------------------------------------------------------------------------
	// add frame header
	//-------------------------------------------------------------------------
	struct hwaddr dst_mac;
	dst_mac.addr1	= 0xff;
	dst_mac.addr2	= 0xff;
	dst_mac.addr3	= 0xff;
	dst_mac.addr4	= 0xff;
	dst_mac.addr5	= 0xff;
	dst_mac.addr6	= 0xff;

	add_frame_header(pkg, FRAME_IP_TYPE, dst_mac, pnet_dev->mac);

	//-------------------------------------------------------------------------
	// setting the whole package length
	//-------------------------------------------------------------------------
	pkg->length 	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + igmp_len; //512 + 14	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);
}
