//===========================================================================
// dhcp_lib.c
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
#include	"dhcp.h"
#include	"prototype.h"

extern	u32 	ticket;


//===========================================================================
// add_dhcp_header
//===========================================================================
PUBLIC	void 	add_dhcp_header(struct s_package *pkg, u8 opcode, u8 hwtype, u8 hwlen, u8 ttl, u32 tran_id, u16 sec, u16 flag, struct iaddr client_ip, struct iaddr my_ip,	struct iaddr server_ip,	struct iaddr gw_ip, struct hwaddr client_mac, s8 *option)
{		
	//-------------------------------------------------------------------------
	// add dhcp header
	//-------------------------------------------------------------------------

	struct s_dhcp_header * dhcp = (struct s_dhcp_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));
	s8 *	dhcp_offset	= dhcp;

	dhcp->opcode		= opcode;
	dhcp->hwtype		= hwtype;
	dhcp->hwlen		= hwlen;
	dhcp->ttl		= ttl;
	dhcp->tran_id		= big_little_32(tran_id);
	dhcp->sec		= big_little_16(sec);
	dhcp->flag		= big_little_16(flag);
	dhcp->client_ip		= client_ip;
	dhcp->my_ip		= my_ip;
	dhcp->server_ip		= server_ip;
	dhcp->gw_ip		= gw_ip;
	dhcp->client_mac	= client_mac;

	dhcp_offset		+= DHCP_HEADER_LENGTH;		//236

	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	strcpy(dhcp_offset, option, DHCP_OPT_LEN);

}


//===========================================================================
// send dhcp_discover
//===========================================================================
PUBLIC	s32 	dhcp_send_discover(struct iaddr dst_ip, struct s_package *dst_pkg)
{
	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dhcp header
	//------------------------------------------------------------------
	struct iaddr client_ip;
	struct iaddr my_ip;
	struct iaddr server_ip;
	struct iaddr gw_ip;
	struct hwaddr client_mac;
	s8 dhcp_option[DHCP_OPT_LEN];
	s8 hostname[32]		= "dthas";

	u8	opcode		= DHCP_REQ;
	u8	hwtype		= 1;
	u8	hwlen		= 6;
	u8	ttl		= 0;

	u32	tran_id		= get_transid();
	g_trans_id		= tran_id;

	u16	sec		= 0;
	u16	flag		= 0;

	client_ip.addr1		= 0;
	client_ip.addr2		= 0;
	client_ip.addr3		= 0;
	client_ip.addr4		= 0;

	my_ip.addr1		= 0;
	my_ip.addr2		= 0;
	my_ip.addr3		= 0;
	my_ip.addr4		= 0;

	server_ip.addr1		= 0;
	server_ip.addr2		= 0;
	server_ip.addr3		= 0;
	server_ip.addr4		= 0;

	gw_ip.addr1		= 0;
	gw_ip.addr2		= 0;
	gw_ip.addr3		= 0;
	gw_ip.addr4		= 0;

	client_mac		= pnet_dev->mac;

	//-------------------------------------------------------------------------
	// add dhcp magic code
	//-------------------------------------------------------------------------
	option_len	= 0;	

	struct iaddr * magic_code	= dhcp_option;

	magic_code->addr1	= 99; 
	magic_code->addr2	= 130; 
	magic_code->addr3	= 83; 
	magic_code->addr4	= 99; 

	option_len		+= 4;
	
	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	struct s_dhcp_option * dhcp_op	= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_FLAG_STATUS;
	dhcp_op->len		= 1;
	dhcp_op->val		= DHCPDISCOVER;

	option_len		+= 3;

	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_HOSTNAME;
	dhcp_op->len		= strlen(hostname);

	strcpy((s8*)(&(dhcp_op->val)), hostname, dhcp_op->len);			

	option_len		+= (dhcp_op->len + 2);


	
	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------	
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_REQIPADDR;		//50
	dhcp_op->len		= 4;

	strcpy((s8*)(&(dhcp_op->val)), (s8*)(&(req_ip)), dhcp_op->len);			

	option_len		+= (dhcp_op->len + 2);
	


	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_PARAMLIST;
	dhcp_op->len		= 13;

	struct s_dhcp_option_paramlist * opt_pl	= (struct s_dhcp_option_paramlist*)(&(dhcp_op->val));

	opt_pl->subnet_mask		= 1;
	opt_pl->broadcast_addr		= 0x1c;
	opt_pl->time_offset		= 2;
	opt_pl->router			= 3;
	opt_pl->domain_name		= 0xf;
	opt_pl->domain_name_server	= 6;
	opt_pl->domain_search		= 0x77;
	opt_pl->hostname		= 0xc;
	opt_pl->netbios_over_nserver	= 0x2c;
	opt_pl->netbios_over_scope	= 0x2f;
	opt_pl->interface_mtu		= 0x1a;
	opt_pl->static_route		= 0x79;
	opt_pl->network_time		= 0x2a;

	option_len		+= 15;

	//-------------------------------------------------------------------------
	// add dhcp option end
	//-------------------------------------------------------------------------
	*(dhcp_option + option_len)	= DHCP_OPT_END;
	option_len++;

	//------------------------------------------------------------------
	// add dhcp option padding
	//------------------------------------------------------------------
	s32 i;
	for(i=option_len; i<DHCP_OPT_LEN; i++)
	{
		*(dhcp_option + i)	= DHCP_OPT_PADDING;
	}


	add_dhcp_header(dst_pkg, opcode, hwtype, hwlen, ttl, tran_id, sec, flag, client_ip, my_ip, server_ip, gw_ip, client_mac, dhcp_option);

	//-------------------------------------------------------------------------
	// dhcp state change
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_DHCPDISCOVER;
	s32 old_state	= dhcp_state;
	dhcp_state	= *(p_dhcp_state_tab + dhcp_state * DHCP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	option_len		= DHCP_OPT_LEN;
	total_len		+= (UDP_HEADER_LENGTH + DHCP_HEADER_LENGTH + option_len);	

	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= PORT_DHCP_CLIENT;
	s16 dst_port	= PORT_DHCP_SERVER;

	add_udp_header(dst_pkg, my_ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, my_ip, dst_ip, PROTOCOL_UDP, total_len);
}

//===========================================================================
// send dhcp_request
//===========================================================================
PUBLIC	s32 	dhcp_send_request(struct iaddr dst_ip, struct s_package *dst_pkg, struct iaddr req_ip)
{
	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dhcp header
	//------------------------------------------------------------------
	struct iaddr client_ip;
	struct iaddr my_ip;
	struct iaddr server_ip;
	struct iaddr gw_ip;
	struct hwaddr client_mac;

	s8 dhcp_option[DHCP_OPT_LEN];
	s8 hostname[32]		= "dthas";

	u8	opcode		= DHCP_REQ;
	u8	hwtype		= 1;
	u8	hwlen		= 6;
	u8	ttl		= 0;
	u32	tran_id		= g_trans_id;
	u16	sec		= 0;
	u16	flag		= 0;

	client_ip.addr1		= 0;
	client_ip.addr2		= 0;
	client_ip.addr3		= 0;
	client_ip.addr4		= 0;

	my_ip.addr1		= 0;
	my_ip.addr2		= 0;
	my_ip.addr3		= 0;
	my_ip.addr4		= 0;

	server_ip.addr1		= 0;
	server_ip.addr2		= 0;
	server_ip.addr3		= 0;
	server_ip.addr4		= 0;

	gw_ip.addr1		= 0;
	gw_ip.addr2		= 0;
	gw_ip.addr3		= 0;
	gw_ip.addr4		= 0;

	client_mac		= pnet_dev->mac;

	//-------------------------------------------------------------------------
	// add dhcp magic code
	//-------------------------------------------------------------------------
	option_len	= 0;	

	struct iaddr * magic_code	= dhcp_option;

	magic_code->addr1	= 99; 
	magic_code->addr2	= 130; 
	magic_code->addr3	= 83; 
	magic_code->addr4	= 99; 

	option_len		+= 4;
	
	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	struct s_dhcp_option * dhcp_op	= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_FLAG_STATUS;
	dhcp_op->len		= 1;
	dhcp_op->val		= DHCPREQUEST;

	option_len		+= 3;

	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_IDENSERVER;		//54
	dhcp_op->len		= 4;

	strcpy((s8*)(&(dhcp_op->val)), (s8*)(&(iden_server_ip)), dhcp_op->len);			

	option_len		+= (dhcp_op->len + 2);

	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_REQIPADDR;		//50
	dhcp_op->len		= 4;

	strcpy((s8*)(&(dhcp_op->val)), (s8*)(&(req_ip)), dhcp_op->len);			

	option_len		+= (dhcp_op->len + 2);


	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_HOSTNAME;
	dhcp_op->len		= strlen(hostname);

	strcpy((s8*)(&(dhcp_op->val)), hostname, dhcp_op->len);			

	option_len		+= (dhcp_op->len + 2);

	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_PARAMLIST;
	dhcp_op->len		= 13;

	struct s_dhcp_option_paramlist * opt_pl	= (struct s_dhcp_option_paramlist*)(&(dhcp_op->val));

	opt_pl->subnet_mask		= 1;
	opt_pl->broadcast_addr		= 0x1c;
	opt_pl->time_offset		= 2;
	opt_pl->router			= 3;
	opt_pl->domain_name		= 0xf;
	opt_pl->domain_name_server	= 6;
	opt_pl->domain_search		= 0x77;
	opt_pl->hostname		= 0xc;
	opt_pl->netbios_over_nserver	= 0x2c;
	opt_pl->netbios_over_scope	= 0x2f;
	opt_pl->interface_mtu		= 0x1a;
	opt_pl->static_route		= 0x79;
	opt_pl->network_time		= 0x2a;

	option_len		+= 15;

	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_CLIENT;		//61
	dhcp_op->len		= 7;

	s8 *op_src		= &(dhcp_op->val);

	//client identifier = type(1) + mac(6)
	*(op_src)		= 1;				// type = 1

	strcpy((s8*)(op_src+1), (s8*)(&(pnet_dev->mac)), (dhcp_op->len-1));	//mac address			

	option_len		+= (dhcp_op->len + 2);

	//-------------------------------------------------------------------------
	// add dhcp option end
	//-------------------------------------------------------------------------
	*(dhcp_option + option_len)	= DHCP_OPT_END;
	option_len++;

	//------------------------------------------------------------------
	// add dhcp option padding
	//------------------------------------------------------------------
	s32 i;
	for(i=option_len; i<DHCP_OPT_LEN; i++)
	{
		*(dhcp_option + i)	= DHCP_OPT_PADDING;
	}
	
	add_dhcp_header(dst_pkg, opcode, hwtype, hwlen, ttl, tran_id, sec, flag, client_ip, my_ip, server_ip, gw_ip, client_mac, dhcp_option);

	//-------------------------------------------------------------------------
	// dhcp state change
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_DHCPREQUEST;
	s32 old_state	= dhcp_state;
	dhcp_state	= *(p_dhcp_state_tab + dhcp_state * DHCP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	option_len		= DHCP_OPT_LEN;
	total_len		+= (UDP_HEADER_LENGTH + DHCP_HEADER_LENGTH + option_len);	

	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= PORT_DHCP_CLIENT;
	s16 dst_port	= PORT_DHCP_SERVER;

	add_udp_header(dst_pkg, my_ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, my_ip, dst_ip, PROTOCOL_UDP, total_len);

}

//===========================================================================
// send dhcp_release
//===========================================================================
PUBLIC	s32 	dhcp_send_release(struct iaddr dst_ip, struct s_package *dst_pkg)
{
	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dhcp header
	//------------------------------------------------------------------
	struct iaddr client_ip;
	struct iaddr my_ip;
	struct iaddr server_ip;
	struct iaddr gw_ip;
	struct hwaddr client_mac;

	s8 dhcp_option[DHCP_OPT_LEN];
	s8 hostname[32]		= "dthas";

	u8	opcode		= DHCP_REQ;
	u8	hwtype		= 1;
	u8	hwlen		= 6;
	u8	ttl		= 0;
	u32	tran_id		= g_trans_id;
	u16	sec		= 0;
	u16	flag		= 0;

	client_ip.addr1		= req_ip.addr1;
	client_ip.addr2		= req_ip.addr2;
	client_ip.addr3		= req_ip.addr3;
	client_ip.addr4		= req_ip.addr4;

	my_ip.addr1		= 0;
	my_ip.addr2		= 0;
	my_ip.addr3		= 0;
	my_ip.addr4		= 0;

	server_ip.addr1		= 0;
	server_ip.addr2		= 0;
	server_ip.addr3		= 0;
	server_ip.addr4		= 0;

	gw_ip.addr1		= 0;
	gw_ip.addr2		= 0;
	gw_ip.addr3		= 0;
	gw_ip.addr4		= 0;

	client_mac		= pnet_dev->mac;

	//-------------------------------------------------------------------------
	// add dhcp magic code
	//-------------------------------------------------------------------------
	option_len	= 0;	

	struct iaddr * magic_code	= dhcp_option;

	magic_code->addr1	= 99; 
	magic_code->addr2	= 130; 
	magic_code->addr3	= 83; 
	magic_code->addr4	= 99; 

	option_len		+= 4;
	
	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	struct s_dhcp_option * dhcp_op	= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_FLAG_STATUS;
	dhcp_op->len		= 1;
	dhcp_op->val		= DHCPRELEASE;

	option_len		+= 3;

	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_IDENSERVER;		//54
	dhcp_op->len		= 4;

	strcpy((s8*)(&(dhcp_op->val)), (s8*)(&(iden_server_ip)), dhcp_op->len);			

	option_len		+= (dhcp_op->len + 2);


	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_CLIENT;		//61
	dhcp_op->len		= 7;

	s8 *op_src		= &(dhcp_op->val);

	//client identifier = type(1) + mac(6)
	*(op_src)		= 1;				// type = 1

	strcpy((s8*)(op_src+1), (s8*)(&(pnet_dev->mac)), (dhcp_op->len-1));	//mac address			

	option_len		+= (dhcp_op->len + 2);

	
	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	dhcp_op			= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_MESSAGE;		//56
	dhcp_op->len		= 14;

	s8 op_msg[15]		= "clean shutdown";

	strcpy((s8*)(&(dhcp_op->val)), (s8*)(op_msg), dhcp_op->len);	//mac address			

	option_len		+= (dhcp_op->len + 2);


	//-------------------------------------------------------------------------
	// add dhcp option end
	//-------------------------------------------------------------------------
	*(dhcp_option + option_len)	= DHCP_OPT_END;
	option_len++;

	//------------------------------------------------------------------
	// add dhcp option padding
	//------------------------------------------------------------------
	s32 i;
	for(i=option_len; i<DHCP_OPT_LEN; i++)
	{
		*(dhcp_option + i)	= DHCP_OPT_PADDING;
	}
	
	add_dhcp_header(dst_pkg, opcode, hwtype, hwlen, ttl, tran_id, sec, flag, client_ip, my_ip, server_ip, gw_ip, client_mac, dhcp_option);

	//-------------------------------------------------------------------------
	// dhcp state change
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_DHCPRELEASE;
	s32 old_state	= dhcp_state;
	dhcp_state	= *(p_dhcp_state_tab + dhcp_state * DHCP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	option_len		= DHCP_OPT_LEN;
	total_len		+= (UDP_HEADER_LENGTH + DHCP_HEADER_LENGTH + option_len);	

	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= PORT_DHCP_CLIENT;
	s16 dst_port	= PORT_DHCP_SERVER;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);

}

//===========================================================================
// send dhcp_offer
//===========================================================================
PUBLIC	s32 	dhcp_send_offer(struct s_package *src_pkg, struct s_package *dst_pkg, struct iaddr res_ip)
{
	struct s_ipv4_header* iph	= (struct s_ipv4_header*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_dhcp_header * src_dhcp = (struct s_dhcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));

	struct iaddr dst_ip	= iph->src_ip;

	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dhcp header
	//------------------------------------------------------------------
	struct iaddr client_ip;
	struct iaddr my_ip;
	struct iaddr server_ip;
	struct iaddr gw_ip;
	struct hwaddr client_mac;
	s8 dhcp_option[DHCP_OPT_LEN];

	u8	opcode		= DHCP_REPLY;
	u8	hwtype		= 1;
	u8	hwlen		= 6;
	u8	ttl		= 0;
	u32	tran_id		= big_little_32(src_dhcp->tran_id);
	u16	sec		= 0;
	u16	flag		= 0;

	client_ip.addr1		= 0;
	client_ip.addr2		= 0;
	client_ip.addr3		= 0;
	client_ip.addr4		= 0;

	my_ip.addr1		= res_ip.addr1;
	my_ip.addr2		= res_ip.addr2;
	my_ip.addr3		= res_ip.addr3;
	my_ip.addr4		= res_ip.addr4;

	server_ip.addr1		= 0;
	server_ip.addr2		= 0;
	server_ip.addr3		= 0;
	server_ip.addr4		= 0;

	gw_ip.addr1		= 0;
	gw_ip.addr2		= 0;
	gw_ip.addr3		= 0;
	gw_ip.addr4		= 0;

	client_mac		= pnet_dev->mac;

	//-------------------------------------------------------------------------
	// add dhcp magic code
	//-------------------------------------------------------------------------
	option_len	= 0;	

	struct iaddr * magic_code	= dhcp_option;

	magic_code->addr1	= 99; 
	magic_code->addr2	= 130; 
	magic_code->addr3	= 83; 
	magic_code->addr4	= 99; 

	option_len		+= 4;
	
	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	struct s_dhcp_option * dhcp_op	= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_FLAG_STATUS;
	dhcp_op->len		= 1;
	dhcp_op->val		= DHCPOFFER;

	option_len		+= 3;

	//-------------------------------------------------------------------------
	// add dhcp option end
	//-------------------------------------------------------------------------
	*(dhcp_option + option_len)	= DHCP_OPT_END;
	option_len++;

	//------------------------------------------------------------------
	// add dhcp option padding
	//------------------------------------------------------------------
	s32 i;
	for(i=option_len; i<DHCP_OPT_LEN; i++)
	{
		*(dhcp_option + i)	= DHCP_OPT_PADDING;
	}
	
	add_dhcp_header(dst_pkg, opcode, hwtype, hwlen, ttl, tran_id, sec, flag, client_ip, my_ip, server_ip, gw_ip, client_mac, dhcp_option);

	
	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	option_len		= DHCP_OPT_LEN;
	total_len		+= (UDP_HEADER_LENGTH + DHCP_HEADER_LENGTH + option_len);	

	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= PORT_DHCP_SERVER;
	s16 dst_port	= PORT_DHCP_CLIENT;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);

}

//===========================================================================
// send dhcp_ack
//===========================================================================
PUBLIC	s32 	dhcp_send_ack(struct s_package *src_pkg, struct s_package *dst_pkg, struct iaddr res_ip)
{
	struct s_ipv4_header* iph	= (struct s_ipv4_header*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_dhcp_header * src_dhcp = (struct s_dhcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));

	struct iaddr dst_ip	= iph->src_ip;	

	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dhcp header
	//------------------------------------------------------------------
	struct iaddr client_ip;
	struct iaddr my_ip;
	struct iaddr server_ip;
	struct iaddr gw_ip;
	struct hwaddr client_mac;
	s8 dhcp_option[DHCP_OPT_LEN];

	u8	opcode		= DHCP_REPLY;
	u8	hwtype		= 1;
	u8	hwlen		= 6;
	u8	ttl		= 0;
	u32	tran_id		= big_little_32(src_dhcp->tran_id);
	u16	sec		= 0;
	u16	flag		= 0;

	client_ip.addr1		= 0;
	client_ip.addr2		= 0;
	client_ip.addr3		= 0;
	client_ip.addr4		= 0;

	my_ip.addr1		= res_ip.addr1;
	my_ip.addr2		= res_ip.addr2;
	my_ip.addr3		= res_ip.addr3;
	my_ip.addr4		= res_ip.addr4;

	server_ip.addr1		= 0;
	server_ip.addr2		= 0;
	server_ip.addr3		= 0;
	server_ip.addr4		= 0;

	gw_ip.addr1		= 0;
	gw_ip.addr2		= 0;
	gw_ip.addr3		= 0;
	gw_ip.addr4		= 0;

	client_mac		= pnet_dev->mac;


	//-------------------------------------------------------------------------
	// add dhcp magic code
	//-------------------------------------------------------------------------
	option_len	= 0;	

	struct iaddr * magic_code	= dhcp_option;

	magic_code->addr1	= 99; 
	magic_code->addr2	= 130; 
	magic_code->addr3	= 83; 
	magic_code->addr4	= 99; 

	option_len		+= 4;
	
	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	struct s_dhcp_option * dhcp_op	= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_FLAG_STATUS;
	dhcp_op->len		= 1;
	dhcp_op->val		= DHCPACK;

	option_len		+= 3;
	
	//-------------------------------------------------------------------------
	// add dhcp option end
	//-------------------------------------------------------------------------
	*(dhcp_option + option_len)	= DHCP_OPT_END;
	option_len++;

	//------------------------------------------------------------------
	// add dhcp option padding
	//------------------------------------------------------------------
	s32 i;
	for(i=option_len; i<DHCP_OPT_LEN; i++)
	{
		*(dhcp_option + i)	= DHCP_OPT_PADDING;
	}
	
	add_dhcp_header(dst_pkg, opcode, hwtype, hwlen, ttl, tran_id, sec, flag, client_ip, my_ip, server_ip, gw_ip, client_mac, dhcp_option);

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	option_len		= DHCP_OPT_LEN;
	total_len		+= (UDP_HEADER_LENGTH + DHCP_HEADER_LENGTH + option_len);	
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= PORT_DHCP_SERVER;
	s16 dst_port	= PORT_DHCP_CLIENT;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}

//===========================================================================
// send dhcp_ack
//===========================================================================
PUBLIC	s32 	dhcp_send_nack(struct s_package *src_pkg, struct s_package *dst_pkg, struct iaddr res_ip)
{
	struct s_ipv4_header* iph	= (struct s_ipv4_header*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_dhcp_header * src_dhcp = (struct s_dhcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));

	struct iaddr dst_ip	= iph->src_ip;	

	u16 option_len		= 0;
	u16 total_len		= 0;

	//------------------------------------------------------------------
	// add dhcp header
	//------------------------------------------------------------------
	struct iaddr client_ip;
	struct iaddr my_ip;
	struct iaddr server_ip;
	struct iaddr gw_ip;
	struct hwaddr client_mac;
	s8 dhcp_option[DHCP_OPT_LEN];

	u8	opcode		= DHCP_REPLY;
	u8	hwtype		= 1;
	u8	hwlen		= 6;
	u8	ttl		= 0;
	u32	tran_id		= big_little_32(src_dhcp->tran_id);
	u16	sec		= 0;
	u16	flag		= 0;

	client_ip.addr1		= 0;
	client_ip.addr2		= 0;
	client_ip.addr3		= 0;
	client_ip.addr4		= 0;

	my_ip.addr1		= res_ip.addr1;
	my_ip.addr2		= res_ip.addr2;
	my_ip.addr3		= res_ip.addr3;
	my_ip.addr4		= res_ip.addr4;

	server_ip.addr1		= 0;
	server_ip.addr2		= 0;
	server_ip.addr3		= 0;
	server_ip.addr4		= 0;

	gw_ip.addr1		= 0;
	gw_ip.addr2		= 0;
	gw_ip.addr3		= 0;
	gw_ip.addr4		= 0;

	client_mac		= pnet_dev->mac;

	//-------------------------------------------------------------------------
	// add dhcp magic code
	//-------------------------------------------------------------------------
	option_len	= 0;	

	struct iaddr * magic_code	= dhcp_option;

	magic_code->addr1	= 99; 
	magic_code->addr2	= 130; 
	magic_code->addr3	= 83; 
	magic_code->addr4	= 99; 

	option_len		+= 4;
	
	//-------------------------------------------------------------------------
	// add dhcp option
	//-------------------------------------------------------------------------
	struct s_dhcp_option * dhcp_op	= dhcp_option + option_len;

	dhcp_op->flag		= DHCP_OPT_FLAG_STATUS;
	dhcp_op->len		= 1;
	dhcp_op->val		= DHCPNACK;

	option_len		+= 3;	

	//-------------------------------------------------------------------------
	// add dhcp option end
	//-------------------------------------------------------------------------
	*(dhcp_option + option_len)	= DHCP_OPT_END;
	option_len++;

	//------------------------------------------------------------------
	// add dhcp option padding
	//------------------------------------------------------------------
	s32 i;
	for(i=option_len; i<DHCP_OPT_LEN; i++)
	{
		*(dhcp_option + i)	= DHCP_OPT_PADDING;
	}
	
	add_dhcp_header(dst_pkg, opcode, hwtype, hwlen, ttl, tran_id, sec, flag, client_ip, my_ip, server_ip, gw_ip, client_mac, dhcp_option);

	//-------------------------------------------------------------------------
	// total_len
	//-------------------------------------------------------------------------
	option_len		= DHCP_OPT_LEN;
	total_len		+= (UDP_HEADER_LENGTH + DHCP_HEADER_LENGTH + option_len);	
	
	//-------------------------------------------------------------------------
	// add udp header
	//-------------------------------------------------------------------------
	s16 src_port	= PORT_DHCP_SERVER;
	s16 dst_port	= PORT_DHCP_CLIENT;

	add_udp_header(dst_pkg, pnet_dev->ip, total_len, dst_ip, PROTOCOL_UDP, src_port, dst_port);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_UDP, total_len);
}

//===========================================================================
// rand_val
//===========================================================================
PUBLIC	u32	get_transid(void)
{
	u32 id = ticket & 0xFFFF;

	return id;
}
