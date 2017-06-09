//===========================================================================
// dhcp.h
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
#ifndef	_X_DHCP_H_
#define	_X_DHCP_H_

#define	DHCP_HEADER_LENGTH	236

//-------------------------------------------------------------------------
// dns status
//-------------------------------------------------------------------------
#define	DHCP_NR_STATUS	9

#define	DHCP_STATE_END	-1		// ERROR

#define	DHCP_STATE1		0	// INIT
#define	DHCP_STATE2		1	// SELECTING(client)
#define	DHCP_STATE3		2	// RECV_OFFER
#define	DHCP_STATE4		3	// SEND_OFFER
#define	DHCP_STATE5		4	// REQUESTING
#define	DHCP_STATE6		5	// BOUND
#define	DHCP_STATE7		6	// RENEWING
#define	DHCP_STATE8		7	// REBINDING
#define	DHCP_STATE9		8	// SELECTING(server)

//-------------------------------------------------------------------------
// dns action in each status
//-------------------------------------------------------------------------
#define	DHCP_NR_ACTION	13

#define	EX_TIME			0	//

#define	SEND_DHCPDISCOVER	1	//
#define	SEND_DHCPOFFER		2	//
#define	SEND_DHCPREQUEST	3	//
#define	SEND_DHCPACK		4	//
#define	SEND_DHCPNACK		5	//
#define	SEND_DHCPRELEASE	6	//


#define	RECV_DHCPDISCOVER	7	//
#define	RECV_DHCPOFFER		8	//
#define	RECV_DHCPREQUEST	9	//
#define	RECV_DHCPACK		10	//
#define	RECV_DHCPNACK		11	//
#define	RECV_DHCPRELEASE	12	//

//-------------------------------------------------------------------------
// dhcp header
//-------------------------------------------------------------------------

struct s_dhcp_header
{
	u8  opcode;
	u8  hwtype;
	u8  hwlen;
	u8  ttl;
	u32 tran_id;
	u16 sec;
	u16 flag;
	struct iaddr client_ip;
	struct iaddr my_ip;
	struct iaddr server_ip;
	struct iaddr gw_ip;
	struct hwaddr client_mac;				
};

struct s_dhcp_option
{
	u8  flag;
	u8  len;
	u8  val;			
};

struct s_dhcp_option_paramlist
{
	u8	subnet_mask;
	u8	broadcast_addr;
	u8	time_offset;
	u8	router;
	u8	domain_name;
	u8	domain_name_server;
	u8	domain_search;
	u8	hostname;
	u8 	netbios_over_nserver;
	u8	netbios_over_scope;
	u8	interface_mtu;
	u8	static_route;
	u8	network_time;
};

#define	DHCP_REQ		1	//
#define	DHCP_REPLY		2	//

//-------------------------------------------------------------------------
// dhcp option
//-------------------------------------------------------------------------
#define	DHCP_OPT_FLAG_STATUS	53	//
#define	DHCP_OPT_HOSTNAME	12
#define	DHCP_OPT_PARAMLIST	55
#define	DHCP_OPT_REQIPADDR	50
#define	DHCP_OPT_IDENSERVER	54
#define	DHCP_OPT_MESSAGE	56
#define	DHCP_OPT_CLIENT		61

#define	DHCP_OPT_END		255
#define	DHCP_OPT_PADDING	0

#define	DHCP_OPT_LEN		64

#define	DHCPDISCOVER		1	//
#define	DHCPOFFER		2	//
#define	DHCPREQUEST		3	//
#define	DHCPDECLINE		4
#define	DHCPACK			5	//
#define	DHCPNACK		6	//
#define	DHCPRELEASE		7	//
#define	DHCPINFORM		8	//

PUBLIC s32 	*p_dhcp_state_tab;

PUBLIC s32 	dhcp_state;

PUBLIC u8	*p_ddhcp_stack;

PUBLIC u32	g_trans_id;

PUBLIC struct iaddr req_ip;
PUBLIC struct iaddr iden_server_ip;

#endif
