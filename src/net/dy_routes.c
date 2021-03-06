//===========================================================================
// dy_routes.c
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
#include	"dy_routes.h"
#include	"prototype.h"

s8 tmp_ospf_data[PKG_8023_DATA_MAX];
//===========================================================================
// PUBLIC	void 	sys_traceroute(struct iaddr dst_ip)
//===========================================================================
PUBLIC	void 	dyroutes_process(struct s_package *pkg, u8 type, u8 code)
{
	switch(type)
	{
		case DY_RIP1:
			rip1_process(pkg, code);
			break;
		case DY_RIP2:
			rip2_process(pkg, code);
			break;
		case DY_OSPF2:
			ospf2_process(pkg, code);
			break;
		case DY_BGP:
			bgp_process(pkg, code);
			break;
		default:
			break;
	}
}

//===========================================================================
// PUBLIC	void 	sys_traceroute(struct iaddr dst_ip)
//===========================================================================
PUBLIC	void 	rip1_process(struct s_package *pkg, u8 code)
{
	struct iaddr dst_ip;
	dst_ip.addr1	= 0;
	dst_ip.addr2	= 0;
	dst_ip.addr3	= 0;
	dst_ip.addr4	= 0;

	//-------------------------------------------------------------------------
	// add udp content
	//-------------------------------------------------------------------------
	struct s_rip1_h * riph	= (struct s_rip1_h *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));
	struct s_rip1_c * ripc	= (struct s_rip1_c *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + RIPH_LEN]));

	riph->cmd	= code;
	riph->version	= RIP_VERSION_1;
	riph->zero1	= 0;

	if(code == RIP_CMD_REQ)
	{
		ripc->addr		= 0;
	}
	else
	{
		ripc->addr		= big_little_16(2);
	}

	ripc->zero2	= 0;
	ripc->ip	= dst_ip;
	ripc->zero3	= 0;
	ripc->zero4	= 0;

	if(code == RIP_CMD_REQ)
	{
		ripc->metric		= big_little_32(16);
	}

	//-------------------------------------------------------------------------
	// add udp header 
	//-------------------------------------------------------------------------
	s16 udp_len	= 512;	// (20*25 + 4)(rip) + 8(udp_header) = 512

	s16 src_port	= DYROUTES_PORT;
	s16 dst_port	= DYROUTES_PORT;

	add_udp_header(pkg, pnet_dev->ip, udp_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	
	
	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	u16 ip_len	= big_little_16(IPV4_HEADER_LENGTH + udp_len); 

	add_ipv4_header(pkg, pnet_dev->ip, IP_TTL, dst_ip, PROTOCOL_UDP, 0, ip_len, 0,  IP_FLAG_NO_FRAGMENT);    //TOS of traceroute is 0

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
	pkg->length 	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + udp_len; //512 + 14	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);
}



//===========================================================================
// PUBLIC	void 	sys_traceroute(struct iaddr dst_ip)
//===========================================================================
PUBLIC	void 	rip2_process(struct s_package *pkg, u8 code)
{
	struct iaddr dst_ip;
	dst_ip.addr1	= 0;
	dst_ip.addr2	= 0;
	dst_ip.addr3	= 0;
	dst_ip.addr4	= 0;

	//-------------------------------------------------------------------------
	// add udp content
	//-------------------------------------------------------------------------
	struct s_rip2_h * riph	= (struct s_rip2_h *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));
	struct s_rip2_c * ripc	= (struct s_rip2_c *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + RIPH_LEN]));

	riph->cmd		= code;
	riph->version		= RIP_VERSION_2;
	riph->routedomain	= 0;

	if(code == RIP_CMD_REQ)
	{
		ripc->addr		= 0;
	}
	else
	{
		ripc->addr		= big_little_16(2);
	}

	ripc->routemark		= 0;
	ripc->ip		= dst_ip;

	ripc->netmask.addr1	= 0;
	ripc->netmask.addr2	= 0;
	ripc->netmask.addr3	= 0;
	ripc->netmask.addr4	= 0;

	ripc->next_ip.addr1	= 0;
	ripc->next_ip.addr2	= 0;
	ripc->next_ip.addr3	= 0;
	ripc->next_ip.addr4	= 0;

	if(code == RIP_CMD_REQ)
	{
		ripc->metric		= big_little_32(16);
	}

	//-------------------------------------------------------------------------
	// add udp header 
	//-------------------------------------------------------------------------
	s16 udp_len		= 512;	// (20*25 + 4)(rip) + 8(udp_header) = 512

	s16 src_port		= DYROUTES_PORT;
	s16 dst_port		= DYROUTES_PORT;

	add_udp_header(pkg, pnet_dev->ip, udp_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	
	
	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	u16 ip_len	= big_little_16(IPV4_HEADER_LENGTH + udp_len); 

	add_ipv4_header(pkg, pnet_dev->ip, IP_TTL, dst_ip, PROTOCOL_UDP, 0, ip_len, 0,  IP_FLAG_NO_FRAGMENT);    //TOS of traceroute is 0

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
	pkg->length 	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + udp_len; //512 + 14	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);
}



//===========================================================================
// PUBLIC	void 	sys_traceroute(struct iaddr dst_ip)
//===========================================================================
PUBLIC	void 	ospf2_process(struct s_package *pkg, u8 code)
{
	struct iaddr dst_ip;
	dst_ip.addr1	= 0;
	dst_ip.addr2	= 0;
	dst_ip.addr3	= 0;
	dst_ip.addr4	= 0;

	//-------------------------------------------------------------------------
	// add udp content
	//-------------------------------------------------------------------------
	struct s_ospf_h * ospfh		= (struct s_ospf_h *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));
	struct s_ospf_c_hello * ospfc	= (struct s_ospf_c_hello *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + OSPF_H_LEN]));
	struct iaddr * ospfc_addi_ip	= (struct iaddr *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + OSPF_H_LEN + OSPF_C_LEN]));

	ospfh->version		= OSPF_VERSION;
	ospfh->type		= code;
	ospfh->length		= big_little_16(OSPF_H_LEN + OSPF_C_LEN + 8);	// 8 can be modified, which means 2 neighbor ip(each ip is 4 bytes)
	ospfh->src_ip		= pnet_dev->ip;
	ospfh->area_id		= big_little_32(AS_ID);
	ospfh->chksum		= 0;
	ospfh->iden_type	= big_little_16(OSPF_IDENTIFY_N);
	ospfh->identify_0_3	= big_little_32(0);
	ospfh->identify_4_7	= big_little_32(0);

	ospfc->netmask.addr1	= 255;
	ospfc->netmask.addr2	= 255;
	ospfc->netmask.addr3	= 255;
	ospfc->netmask.addr4	= 0;
	
	ospfc->hello_intv	= big_little_16(30);
	ospfc->option		= 0;
	ospfc->prio		= 0;
	ospfc->router_dis_intv	= big_little_32(1);

	ospfc->router_ip.addr1	= 0;
	ospfc->router_ip.addr2	= 0;
	ospfc->router_ip.addr3	= 0;
	ospfc->router_ip.addr4	= 0;
	 
	ospfc->bak_router_ip.addr1	= 0;
	ospfc->bak_router_ip.addr2	= 0;
	ospfc->bak_router_ip.addr3	= 0;
	ospfc->bak_router_ip.addr4	= 0;

	//neighbor ip	
	struct iaddr * ipt	= ospfc_addi_ip;
	ipt->addr1		= 101;
	ipt->addr2		= 102;
	ipt->addr3		= 103;
	ipt->addr4		= 101;

	ipt++;
	ipt->addr1		= 192;
	ipt->addr2		= 168;
	ipt->addr3		= 1;
	ipt->addr4		= 103;


	// update checksum
	s8 *dst_addr	= tmp_ospf_data;
	s8 *src_addr	= &(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]);
	s32 len_h	= OSPF_H_LEN - 10; // exclude iden_type, identify_0_3, identify_4_7

	strcpy(dst_addr, src_addr, len_h);

	dst_addr	= tmp_ospf_data + len_h;
	src_addr	= &(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + OSPF_H_LEN]);
	s32 len_c	= OSPF_C_LEN;

	strcpy(dst_addr, src_addr, len_c);

	dst_addr	= tmp_ospf_data + len_h + len_c;
	src_addr	= ospfc_addi_ip;
	s32 len_addi	= 8;

	strcpy(dst_addr, src_addr, len_addi);

	ospfh->chksum	= makechksum(tmp_ospf_data,(len_h + len_c + len_addi));

	u16 ospf_len	= OSPF_H_LEN + OSPF_C_LEN + 8;
	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	u16 ip_len	= big_little_16(IPV4_HEADER_LENGTH + ospf_len); 

	add_ipv4_header(pkg, pnet_dev->ip, IP_TTL, dst_ip, PROTOCOL_OSPF, 0, ip_len, 0,  IP_FLAG_NO_FRAGMENT);    //TOS of traceroute is 0

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
	pkg->length 	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + ospf_len; //512 + 14	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);
}

//===========================================================================
// PUBLIC	void 	sys_traceroute(struct iaddr dst_ip)
//===========================================================================
PUBLIC	void 	bgp_process(struct s_package *pkg, u8 code)
{
	struct iaddr dst_ip;
	dst_ip.addr1	= 0;
	dst_ip.addr2	= 0;
	dst_ip.addr3	= 0;
	dst_ip.addr4	= 0;

	//-------------------------------------------------------------------------
	// add tcp content
	//-------------------------------------------------------------------------
	struct s_bgp_h * bgph	= (struct s_ospf_h *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));
	
	bgph->iden1		= big_little_32(0);
	bgph->iden2		= big_little_32(0);
	bgph->iden3		= big_little_32(0);
	bgph->iden4		= big_little_32(0);

	bgph->length		= big_little_16(BGP_H_LEN + BGP_C_KEEPALIVE_LEN);
	bgph->type		= code;

	u16 bgp_len		= BGP_H_LEN + BGP_C_KEEPALIVE_LEN;

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	u16 tcp_len		= bgp_len;

	//-------------------------------------------------------------------------
	// add ip header
	//-------------------------------------------------------------------------
	u16 ip_len	= big_little_16(IPV4_HEADER_LENGTH + tcp_len); 

	add_ipv4_header(pkg, pnet_dev->ip, IP_TTL, dst_ip, PROTOCOL_OSPF, 0, ip_len, 0,  IP_FLAG_NO_FRAGMENT);    //TOS of traceroute is 0

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
	pkg->length 	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + tcp_len; //512 + 14	

	//-------------------------------------------------------------------------
	// send package
	//-------------------------------------------------------------------------
	rtl8029_send_pkg(pkg);
}
