//===========================================================================
// smtp_server.c
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


PRIVATE s8	machine_name[10]	= "dthas";

PRIVATE s8	start_mail_comment[35]	= "End data with <CR><LF>.<CR><LF>";

//===========================================================================
// smtp_get_request
//===========================================================================
PUBLIC	s32 	smtp_send_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;

	seq	= little_big_32(tcph->ack);

	if(src_tcp_data_len)
	{
		ack	= little_big_32(tcph->seq) + src_tcp_data_len;
	}
	else
	{
		ack	= little_big_32(tcph->seq) + 1;
	} 

	u16 option_len		= 0;
	u16 total_len		= 0;
	
	u16 str_len		= 0;
	//------------------------------------------------------------------
	// add require line
	//------------------------------------------------------------------
	//1)add state
	str_len	= strlen(p_ssmtp_state + STS_CMD_OK * SMTP_STATE_SIZE);
	strcpy(smtp_con, p_ssmtp_state + STS_CMD_OK * SMTP_STATE_SIZE, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*smtp_con		= 0x20;		//space

	smtp_con		+= 1;
	total_len		+= 1;

	//3)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_OK;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_SMTP;
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;	
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + total_len;	//
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}

//===========================================================================
// smtp_get_request
//===========================================================================
PUBLIC	s32 	smtp_send_ready(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;

	seq	= little_big_32(tcph->ack);

	if(src_tcp_data_len)
	{
		ack	= little_big_32(tcph->seq) + src_tcp_data_len;
	}
	else
	{		
		ack	= little_big_32(tcph->seq);		// only 220 is like this
	} 

	u16 option_len		= 0;
	u16 total_len		= 0;
	
	u16 str_len		= 0;
	//------------------------------------------------------------------
	// add require line
	//------------------------------------------------------------------
	//1)add state
	str_len	= strlen(p_ssmtp_state + STS_SERVICE_READY * SMTP_STATE_SIZE);
	strcpy(smtp_con, p_ssmtp_state + STS_SERVICE_READY * SMTP_STATE_SIZE, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*smtp_con		= 0x20;		//space

	smtp_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(machine_name);
	strcpy(smtp_con, machine_name, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_SERVERREADY;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_SMTP;
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;	
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + total_len;	//
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}

//===========================================================================
// smtp_get_request
//===========================================================================
PUBLIC	s32 	smtp_send_startmail(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;

	seq	= little_big_32(tcph->ack);

	if(src_tcp_data_len)
	{
		ack	= little_big_32(tcph->seq) + src_tcp_data_len;
	}
	else
	{
		ack	= little_big_32(tcph->seq) + 1;
	} 

	u16 option_len		= 0;
	u16 total_len		= 0;
	
	u16 str_len		= 0;
	//------------------------------------------------------------------
	// add require line
	//------------------------------------------------------------------
	//1)add state
	str_len	= strlen(p_ssmtp_state + STS_START_MAIL_INPUT * SMTP_STATE_SIZE);
	strcpy(smtp_con, p_ssmtp_state + STS_START_MAIL_INPUT * SMTP_STATE_SIZE, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*smtp_con		= 0x20;		//space

	smtp_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(start_mail_comment);
	strcpy(smtp_con, start_mail_comment, str_len);	//End data with <CR><LF>.<CR><LF>

	smtp_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_STARTMAILREADY;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_SMTP;
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + total_len;	//
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}

//===========================================================================
// smtp_get_request
//===========================================================================
PUBLIC	s32 	smtp_send_close(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;

	seq	= little_big_32(tcph->ack);

	if(src_tcp_data_len)
	{
		ack	= little_big_32(tcph->seq) + src_tcp_data_len;
	}
	else
	{
		ack	= little_big_32(tcph->seq) + 1;
	} 

	u16 option_len		= 0;
	u16 total_len		= 0;
	
	u16 str_len		= 0;
	//------------------------------------------------------------------
	// add require line
	//------------------------------------------------------------------
	//1)add state
	str_len	= strlen(p_ssmtp_state + STS_SERVICE_CLOSE_TRANS * SMTP_STATE_SIZE);
	strcpy(smtp_con, p_ssmtp_state + STS_SERVICE_CLOSE_TRANS * SMTP_STATE_SIZE, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*smtp_con		= 0x20;		//space

	smtp_con		+= 1;
	total_len		+= 1;

	//3)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CLOSE;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_SMTP;
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;	
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + total_len;	//
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}

//===========================================================================
// smtp_get_request
//===========================================================================
PUBLIC	s32 	mail_process(struct s_package *src_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;

	u8 *src_d			= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + src_ip_header_len + src_tcp_header_len]));

	strcpy(p_recv_nbuf, src_d, src_tcp_data_len);		//copy the mail from tcp package to 8.5MB
	p_recv_nbuf		+= src_tcp_data_len;

	*(p_recv_nbuf + 1)	= NULL;

	cat(p_mailname, (s8*)NET_RECV_BUF, FILE_MODE_W);		//copy the mail from 8.5MB to file(/test/mail.txt)
}
