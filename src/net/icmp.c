//===========================================================================
// icmp.c
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
#include	"icmp.h"
#include	"prototype.h"

//===========================================================================
// init tty
//===========================================================================
PUBLIC	void icmp_process(struct s_package *pkg, struct iaddr dst_ip, u8 type, u8 code, u16 iden, u16 seq)
{
	switch(type)
	{
		case 0:
			//break;
		case 8:
			icmp_echoreply(pkg, dst_ip, type, code, iden, seq);
			break;
		case 13:
			//break;
		case 14:
			icmp_timereply(pkg, dst_ip, type, code, iden, seq);
			break;
		default:
			break;
	}
}

//===========================================================================
// icmp_echoreply(struct s_package *pkg, struct iaddr dst_ip, u8 type, u8 code)
//===========================================================================
PUBLIC	void 	icmp_echoreply(struct s_package *pkg, struct iaddr dst_ip, u8 type, u8 code, u16 iden, u16 seq)
{
	//-------------------------------------------------------------------------
	// check routes in route table
	//-------------------------------------------------------------------------
	struct route_table *rtl = search_route(dst_ip);

	if(!rtl)
	{
		printk("icmp_echoreply route check fail");
		printk("\n");

		return;
	}

	//-------------------------------------------------------------------------
	// add icmp header and ip content
	//-------------------------------------------------------------------------
	struct s_icmp_header *icmph = (struct s_icmp_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	icmph->type	= type;
	icmph->code	= code;
	icmph->checksum	= 0;
	icmph->iden	= iden;
	icmph->seq	= seq;

	s32 icmp_con_s	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + ICMP_HEADER_LENGTH_0_8; //14+20+8=42
	s32 icmp_con_e	= FRAME_HEADER_LENGTH + IPV4_DATA_MIN_LENGTH;
	
	s32 i;
	for(i=icmp_con_s; i<icmp_con_e; i++)
	{
		pkg->buffer[i] = 1;
	}

	u16 length	= IPV4_DATA_MIN_LENGTH - IPV4_HEADER_LENGTH;   // 512 - 20 = 512 - ip_header
	icmph->checksum	= makechksum(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]), length);	
	
	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	u16 ip_len	= big_little_16(0x200);

	add_ipv4_header(pkg, pnet_dev->ip, IP_TTL, dst_ip, PROTOCOL_ICMP, SERVICE_ICMP_QUERY, ip_len, 0,  IP_FLAG_NO_FRAGMENT);

	//-------------------------------------------------------------------------
	// add frame header
	//-------------------------------------------------------------------------
	add_frame_header(pkg, FRAME_IP_TYPE, rtl->gw_mac, pnet_dev->mac);

	//-------------------------------------------------------------------------
	// setting the whole package length
	//-------------------------------------------------------------------------
	pkg->length 	= IPV4_DATA_MIN_LENGTH + FRAME_HEADER_LENGTH; //512 + 14	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);
}

//===========================================================================
// icmp_echoreply(struct s_package *pkg, struct iaddr dst_ip, u8 type, u8 code)
//===========================================================================
PUBLIC	void 	icmp_timereply(struct s_package *pkg, struct iaddr dst_ip, u8 type, u8 code, u16 iden, u16 seq)
{
	//-------------------------------------------------------------------------
	// check routes in route table
	//-------------------------------------------------------------------------
	struct route_table *rtl = search_route(dst_ip);

	if(!rtl)
	{
		printk("icmp_echoreply route check fail");
		printk("\n");

		return;
	}

	//-------------------------------------------------------------------------
	// add icmp timestamp content
	//-------------------------------------------------------------------------
	struct s_icmp_timestamp *icmpt	= (struct s_icmp_timestamp *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + ICMP_HEADER_LENGTH_0_8]));
	icmpt->send_timestamp	= 0;
	icmpt->recv_timestamp	= 0;
	icmpt->tran_timestamp	= 0;

	//-------------------------------------------------------------------------
	// add icmp header and ip content
	//-------------------------------------------------------------------------
	struct s_icmp_header *icmph = (struct s_icmp_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	icmph->type	= type;
	icmph->code	= code;
	icmph->checksum	= 0;
	icmph->iden	= iden;
	icmph->seq	= seq;

	s32 icmp_con_s	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + ICMP_HEADER_LENGTH_0_8; //14+20+8=42
	s32 icmp_con_e	= FRAME_HEADER_LENGTH + IPV4_DATA_MIN_LENGTH;
	
	s32 i;
	for(i=icmp_con_s; i<icmp_con_e; i++)
	{
		pkg->buffer[i] = 1;
	}

	u16 length	= IPV4_DATA_MIN_LENGTH - IPV4_HEADER_LENGTH;   // 512 - 20 = 512 - ip_header
	icmph->checksum	= makechksum(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]), length);	
	
	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	u16 ip_len	= big_little_16(0x200);

	add_ipv4_header(pkg, pnet_dev->ip, IP_TTL, dst_ip, PROTOCOL_ICMP, SERVICE_ICMP_QUERY, ip_len, 0,  IP_FLAG_NO_FRAGMENT);

	//-------------------------------------------------------------------------
	// add frame header
	//-------------------------------------------------------------------------
	add_frame_header(pkg, FRAME_IP_TYPE, rtl->gw_mac, pnet_dev->mac);

	//-------------------------------------------------------------------------
	// setting the whole package length
	//-------------------------------------------------------------------------
	pkg->length 	= IPV4_DATA_MIN_LENGTH + FRAME_HEADER_LENGTH; //512 + 14	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);
}
