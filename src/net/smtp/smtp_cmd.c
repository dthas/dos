//===========================================================================
// stmp_cmd.c
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
#include	"tcp.h"
#include	"smtp.h"
#include	"prototype.h"

extern	u32	ticket;

//===========================================================================
// smtp_open(struct s_package *dst_pkg, struct iaddr dst_ip)
//===========================================================================
PUBLIC void	smtp_open_sys(struct iaddr dst_ip, struct s_package *dst_pkg)
{
	tcp_state 	= TCP_STATE1;
	tcp_state 	= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_SYN);

	//-------------------------------------------------------------------------
	// add tcp option(data)
	//-------------------------------------------------------------------------
	u16 option_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	client_port	= ticket & 0xFFFF;
	if(client_port < 25600)
	{
		client_port	+= 25600;
	}

	struct iaddr src_ip	= pnet_dev->ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_SMTP;
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	ack			= 0;
	seq			= get_seq();	
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + 0;	//ftp control without any data
	u8 ctrl_bit		= SYN_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}

//===========================================================================
// ftpclose(struct iaddr ip)
//===========================================================================
PUBLIC void	smtp_close_sys(struct s_package *src_pkg, struct s_package *dst_pkg)
{	
	tcp_state 			= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_FIN_ACK);

	struct s_ipv4_header *iph= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_tcp_header *tcph= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	u16 data_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	u16 option_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	struct iaddr dst_ip	= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= big_little_16(tcph->dst_port);
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + data_len;	//ftp control without any data
	u8 ctrl_bit		= FIN_BIT | ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}
