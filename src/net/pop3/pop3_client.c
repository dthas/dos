//===========================================================================
// pop3_client.c
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
#include	"pop3.h"
#include	"prototype.h"

extern	u32	ticket;

PRIVATE	s8	pop3_cmd[NR_POP3_CMD][POP3_CMD_SIZE]		= {"user","pass","apop","stat","uidl","list","retr","dele", "rset",
									"top", "noop", "quit"};

PRIVATE	s8	pop3_state_code[NR_POP3_STATE][POP3_STATE_SIZE]	= {"+OK", "-ERR"};	

PRIVATE s8 	username[10]	= "test_in";
PRIVATE	s8	password[10]	= "123456";

//===========================================================================
// parm_init()
//===========================================================================
PUBLIC	void 	pop3_parm_init()
{
	p_pop3_cmd	= pop3_cmd;
	p_ppop3_state	= pop3_state_code;

	flg_pop3_client	= FALSE;
}

//===========================================================================
// smtp_get_request
//===========================================================================
PUBLIC	s32 	pop3_send_user(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	s8* pop3_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	//1)add method
	str_len	= strlen(&pop3_cmd[CMD_USER][0]);
	strcpy(pop3_con, &pop3_cmd[CMD_USER][0], str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*pop3_con		= 0x20;		//space

	pop3_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(username);
	strcpy(pop3_con, username, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_USERNAME;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_POP3;
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
PUBLIC	s32 	pop3_send_pass(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	s8* pop3_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	//1)add method
	str_len	= strlen(&pop3_cmd[CMD_PASS][0]);
	strcpy(pop3_con, &pop3_cmd[CMD_PASS][0], str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*pop3_con		= 0x20;		//space

	pop3_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(password);
	strcpy(pop3_con, password, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_PASSWORD;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_POP3;
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
PUBLIC	s32 	pop3_send_mailinform(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	s8* pop3_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	//1)add method
	str_len	= strlen(&pop3_cmd[CMD_STAT][0]);
	strcpy(pop3_con, &pop3_cmd[CMD_STAT][0], str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_MAILINFORM;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_POP3;
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
PUBLIC	s32 	pop3_send_maillist(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	s8* pop3_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	//1)add method
	str_len	= strlen(&pop3_cmd[CMD_LIST][0]);
	strcpy(pop3_con, &pop3_cmd[CMD_LIST][0], str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_MAILLIST;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_POP3;
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
PUBLIC	s32 	pop3_send_retr(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	s8* pop3_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	//1)add method
	str_len	= strlen(&pop3_cmd[CMD_RETR][0]);
	strcpy(pop3_con, &pop3_cmd[CMD_RETR][0], str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)space
	*pop3_con		= 0x20;		// space
		
	pop3_con		+= 1;
	total_len		+= 1;

	//3)param
	*pop3_con		= '1';		// 1 email
		
	pop3_con		+= 1;
	total_len		+= 1;

	//4)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_RETR;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_POP3;
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
PUBLIC	s32 	pop3_send_quit(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	s8* pop3_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	//1)add method
	str_len	= strlen(&pop3_cmd[CMD_QUIT][0]);
	strcpy(pop3_con, &pop3_cmd[CMD_QUIT][0], str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_QUIT;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_POP3;
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
