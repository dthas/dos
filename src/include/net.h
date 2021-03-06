//===========================================================================
// net.h
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
#ifndef	_X_NET_H_
#define	_X_NET_H_

#define PKG_8023_SIZE_MAX	1514
#define PKG_8023_SIZE_MIN	60
#define PKG_8023_DATA_MAX	1480	// 1514 - 14 - 20

#define IP_DATA_MAX		512	

// Protocol
#define	PROTOCOL_ICMP		0x01
#define	PROTOCOL_IGMP		0x02
#define	PROTOCOL_TCP		0x06
#define	PROTOCOL_UDP		0x11
#define	PROTOCOL_OSPF		0x59

// TOS
#define SERVICE_TELNET		0x10
#define SERVICE_RLOGIN		0x10
#define SERVICE_FTP_CTRL	0x10
#define SERVICE_FTP_DATA	0x08
#define SERVICE_FTP_DATA_ANY	0x08
#define SERVICE_TFTP		0x10
#define SERVICE_SMTP_CMD	0x10
#define SERVICE_SMTP_DATA	0x08
#define SERVICE_DNS_UDP		0x10
#define SERVICE_DNS_TCP		0x00
#define SERVICE_DNS_AREA	0x08
#define SERVICE_ICMP_ERROR	0x00
#define SERVICE_ICMP_QUERY	0x00
#define SERVICE_ICMP_IGP	0x04
#define SERVICE_SNMP		0x04
#define SERVICE_BOOTP		0x00
#define SERVICE_NNTP		0x02

// PORT(TCP)
#define PORT_ECHO		7
#define PORT_DISCARD		9
#define PORT_USERS		11
#define PORT_DAYTIME		13
#define PORT_QUOTE		17
#define PORT_CHARGEN		19
#define PORT_FTP_DATA		20
#define PORT_FTP_CTRL		21
#define PORT_TELNET		23
#define PORT_SMTP		25
#define PORT_DNS		53
#define PORT_DHCP_SERVER	67
#define PORT_DHCP_CLIENT	68
#define	PORT_TFTP		69
#define PORT_FINGER		79
#define PORT_HTTP		80
#define PORT_POP3		110

// PORT(UDP)
#define PORT_SNMP_SERVER	161
#define PORT_SNMP_CLIENT	162

u8	*p_net_stack;

struct s_package
{
	u16 length;	
	u8 buffer[PKG_8023_SIZE_MAX];
};

struct s_package * package;

#endif
