//===========================================================================
// dns_lib.c
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
#include	"dns.h"
#include	"prototype.h"

extern	u32 	ticket;


//===========================================================================
// add_dns_header
//===========================================================================
PUBLIC	void 	add_dns_header(struct s_package *pkg, u16 id, u16 flag, u16 question_cnt, u16 answer_cnt, u16 auth_cnt, u16 addition_cnt)
{		
	//-------------------------------------------------------------------------
	// add dns header
	//-------------------------------------------------------------------------

	struct s_dns_header * dns = (struct s_dns_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));

	dns->id			= big_little_16(id);
	dns->flag		= big_little_16(flag);
	dns->question_cnt	= big_little_16(question_cnt);
	dns->answer_cnt		= big_little_16(answer_cnt);
	dns->auth_cnt		= big_little_16(auth_cnt);
	dns->addition_cnt	= big_little_16(addition_cnt);
}


//===========================================================================
// dns_send_ip (needs dns server return domain name)
//===========================================================================
PUBLIC	s32 	dns_send_ip(struct iaddr dst_ip, struct s_package *dst_pkg, struct iaddr res_ip)
{
	s8* dst; s8* src;
	u32 len;

	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dns header
	//------------------------------------------------------------------
	u16	id		= get_id();	
	u16	flag		= RD;				//0x100
	u16	question_cnt	= 1;
	u16	answer_cnt	= 0;
	u16	auth_cnt	= 0;
	u16	addition_cnt	= 0;

	add_dns_header(dst_pkg, id, flag, question_cnt, answer_cnt, auth_cnt, addition_cnt);

	total_len		+= DNS_HEADER_LENGTH;
	//------------------------------------------------------------------
	// add question record
	//------------------------------------------------------------------
	s8 *data	= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + total_len]);

	src		= p_ip_format;
	dst		= data;
	len		= strlen(p_ip_format)+1;

	strcpy(dst, src, len);

	data		+= len;

	*(s16*)(data)	= big_little_16(QTYPE_PTR);
	*(s16*)(data+2)	= big_little_16(QCLASS_AN);

	total_len	+= (len+4);

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= get_port();
	s16 dst_port	= PORT_DNS;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}

//===========================================================================
// dns_send_name (needs dns server return ip)
//===========================================================================
PUBLIC	s32 	dns_send_name(struct iaddr dst_ip, struct s_package *dst_pkg, s8* domain_name_format)
{
	s8* dst; 
	s8* src;
	u32 len;

	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dns header
	//------------------------------------------------------------------
	u16	id		= get_id();
	u16	flag		= RD;			//0x100
	u16	question_cnt	= 1;
	u16	answer_cnt	= 0;
	u16	auth_cnt	= 0;
	u16	addition_cnt	= 0;

	add_dns_header(dst_pkg, id, flag, question_cnt, answer_cnt, auth_cnt, addition_cnt);

	total_len		+= DNS_HEADER_LENGTH;
	//------------------------------------------------------------------
	// add question record
	//------------------------------------------------------------------
	s8 *data	= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + total_len]);

	src		= domain_name_format;
	dst		= data;
	len		= strlen(domain_name_format)+1;

	strcpy(dst, src, len);

	data		+= len;

	*(s16*)(data)	= big_little_16(QTYPE_A);
	*(s16*)(data+2)	= big_little_16(QCLASS_AN);

	total_len	+= (len+4);

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= get_port();
	s16 dst_port	= PORT_DNS;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}

//===========================================================================
// dns_return_name (return domain name)
//===========================================================================
PUBLIC	s32 	dns_return_name(struct s_package *src_pkg, struct s_package *dst_pkg, s8* domain_name_format)
{
	s8* dst; s8* src;
	u32 len;

	struct s_ipv4_header * iph = (struct s_ipv4_header*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_udp_header * udph = (struct s_udp_header*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));
	struct s_dns_header * dnsh = (struct s_dns_header*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));

	s16 udp_len	= udph->udp_len;

	struct iaddr dst_ip	= iph->src_ip;

	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dns header
	//------------------------------------------------------------------
	u16	id		= big_little_16(dnsh->id);
	u16	flag		= QR | (OP_REVERSE<<11) | AA | RD | RA;		//8d80
	u16	question_cnt	= 1;
	u16	answer_cnt	= 1;
	u16	auth_cnt	= 0;
	u16	addition_cnt	= 0;

	add_dns_header(dst_pkg, id, flag, question_cnt, answer_cnt, auth_cnt, addition_cnt);

	total_len		+= DNS_HEADER_LENGTH;
	//------------------------------------------------------------------
	// add question record
	//------------------------------------------------------------------
	dst		= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + total_len]);
	src		= &(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + total_len]);
	len		= udp_len - UDP_HEADER_LENGTH; 

	strcpy(dst, src, len);

	u16 qtype	= big_little_16(*(s16*)(dst + len - 4));
	u16 qclass	= big_little_16(*(s16*)(dst + len - 2));

	s8 * data	= dst + len;
	total_len	+= len;

	//------------------------------------------------------------------
	// add resource record
	//------------------------------------------------------------------
	*(s16*)data	= 0xC00C;		//offset pointer
	data		+= 2;	
	total_len	+= 2;	

	*(s16*)data	= qtype;
	data		+= 2;
	total_len	+= 2;

	*(s16*)data	= qclass;
	data		+= 2;
	total_len	+= 2;

	*(s32*)data	= 24000;		// ttl
	data		+= 4;
	total_len	+= 4;

	len		= strlen(domain_name_format);

	*(s16*)data	= len;
	data		+= 2;
	total_len	+= 2;

	src		= domain_name_format;
	dst		= data;	

	strcpy(dst, src, len);

	total_len	+= len;

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= big_little_16(udph->dst_port);
	s16 dst_port	= big_little_16(udph->src_port);

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}

//===========================================================================
// dns_return_name (return domain name)
//===========================================================================
PUBLIC	s32 	dns_return_ip(struct s_package *src_pkg, struct s_package *dst_pkg, struct iaddr res_ip)
{
	s8* dst; s8* src;
	u32 len;

	struct s_ipv4_header * iph = (struct s_ipv4_header*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_udp_header * udph = (struct s_udp_header*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));
	struct s_dns_header * dnsh = (struct s_dns_header*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));

	s16 udp_len	= udph->udp_len;

	struct iaddr dst_ip	= iph->src_ip;

	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dns header
	//------------------------------------------------------------------
	u16	id		= big_little_16(dnsh->id);
	u16	flag		= QR | RD | RA;					//8180
	u16	question_cnt	= 1;
	u16	answer_cnt	= 1;
	u16	auth_cnt	= 0;
	u16	addition_cnt	= 0;

	add_dns_header(dst_pkg, id, flag, question_cnt, answer_cnt, auth_cnt, addition_cnt);

	total_len		+= DNS_HEADER_LENGTH;
	//------------------------------------------------------------------
	// add question record
	//------------------------------------------------------------------
	dst		= &(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + total_len]);
	src		= &(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + total_len]);
	len		= udp_len - UDP_HEADER_LENGTH; 

	strcpy(dst, src, len);

	u16 qtype	= big_little_16(*(s16*)(dst + len - 4));
	u16 qclass	= big_little_16(*(s16*)(dst + len - 2));

	s8 * data	= dst + len;
	total_len	+= len;

	//------------------------------------------------------------------
	// add resource record
	//------------------------------------------------------------------
	*(s16*)data	= 0xC00C;		//offset pointer
	data		+= 2;	
	total_len	+= 2;	

	*(s16*)data	= qtype;
	data		+= 2;
	total_len	+= 2;

	*(s16*)data	= qclass;
	data		+= 2;
	total_len	+= 2;

	*(s32*)data	= 12000;		// ttl
	data		+= 4;
	total_len	+= 4;

	*(s16*)data	= 4;			// ip address length
	data		+= 2;
	total_len	+= 2;

	*data		= res_ip.addr1;
	*(data+1)	= res_ip.addr2;
	*(data+2)	= res_ip.addr3;
	*(data+3)	= res_ip.addr4;

	data		+= 4;
	total_len	+= 4;

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	total_len		+= (UDP_HEADER_LENGTH + option_len);
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= big_little_16(udph->dst_port);
	s16 dst_port	= big_little_16(udph->src_port);

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}


//===========================================================================
// rand_val
//===========================================================================
PUBLIC	u16	get_id(void)
{
	u16 id = ticket & 0xFFFF;

	return id;
}	

