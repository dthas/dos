//===========================================================================
// pop3_server.c
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

PRIVATE s8	ready_info[20]	= "Server ready.";
PRIVATE s8	login_info[13]	= "Logged in.";
PRIVATE s8	stat_info[10]	= "1";
PRIVATE s8	list_info[30]	= "2 message:\r\n1 400\r\n2 300";
PRIVATE s8	mail_info[100]	= "Received: by dthas\r\nTo:<test@test.com>\r\nSubject:test4001\r\nFrom:gah@gah.com\r\njust a test";

PRIVATE	void	read_data();

//===========================================================================
// read_data
//===========================================================================
PRIVATE	void	read_data()
{
	s8 fname[64]	= "/test/mail.src";	

	mkfile("/test/mail.src");
	cat("/test/mail.src", mail_info,   FILE_MODE_W);

	read(fname, FILE_MODE_R);			//read file data into 7MB
	s32 str_len	= strlen((s8*)VFS_TMP_DATA);
	strcpy((s8*)NET_SEND_BUF, (s8*)VFS_TMP_DATA, str_len); 	//copy data from 7MB to 8MB
	*(s8*)(NET_SEND_BUF + str_len )	= NULL;
}

//===========================================================================
// pop3_get_request
//===========================================================================
PUBLIC	s32 	pop3_send_ready(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* pop3_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(p_ppop3_state + STS_P_OK * POP3_STATE_SIZE);
	strcpy(pop3_con, p_ppop3_state + STS_P_OK * POP3_STATE_SIZE, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*pop3_con		= 0x20;		//space

	pop3_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(ready_info);
	strcpy(pop3_con, ready_info, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// pop3 state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CMD_OK;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_POP3;
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
// pop3_get_request
//===========================================================================
PUBLIC	s32 	pop3_send_user_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* pop3_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(p_ppop3_state + STS_P_OK * POP3_STATE_SIZE);
	strcpy(pop3_con, p_ppop3_state + STS_P_OK * POP3_STATE_SIZE, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// pop3 state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CMD_OK;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_POP3;
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
// pop3_get_request
//===========================================================================
PUBLIC	s32 	pop3_send_pass_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* pop3_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(p_ppop3_state + STS_P_OK * POP3_STATE_SIZE);
	strcpy(pop3_con, p_ppop3_state + STS_P_OK * POP3_STATE_SIZE, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*pop3_con		= 0x20;		//space

	pop3_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(login_info);
	strcpy(pop3_con, login_info, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// pop3 state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CMD_OK;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_POP3;
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
// pop3_get_request
//===========================================================================
PUBLIC	s32 	pop3_send_mailinform_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* pop3_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(p_ppop3_state + STS_P_OK * POP3_STATE_SIZE);
	strcpy(pop3_con, p_ppop3_state + STS_P_OK * POP3_STATE_SIZE, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*pop3_con		= 0x20;		//space

	pop3_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(stat_info);
	strcpy(pop3_con, stat_info, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// pop3 state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CMD_OK;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_POP3;
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
// pop3_get_request
//===========================================================================
PUBLIC	s32 	pop3_send_maillist_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* pop3_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(p_ppop3_state + STS_P_OK * POP3_STATE_SIZE);
	strcpy(pop3_con, p_ppop3_state + STS_P_OK * POP3_STATE_SIZE, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*pop3_con		= 0x20;		//space

	pop3_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(list_info);
	strcpy(pop3_con, list_info, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;

	//5).
	*pop3_con		= '.';		// .
		
	pop3_con		+= 1;
	total_len		+= 1;

	//6)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// pop3 state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CMD_OK;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_POP3;
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
// pop3_get_request
//===========================================================================
PUBLIC	s32 	pop3_send_mail(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* pop3_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	//1)add state
	//------------------------------------------------------------------	
	str_len	= strlen(p_ppop3_state + STS_P_OK * POP3_STATE_SIZE);
	strcpy(pop3_con, p_ppop3_state + STS_P_OK * POP3_STATE_SIZE, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;
	
	//------------------------------------------------------------------
	//2)add sp
	//------------------------------------------------------------------
	*pop3_con		= 0x20;		//space

	pop3_con		+= 1;
	total_len		+= 1;

	//------------------------------------------------------------------
	//3)\r\n
	//------------------------------------------------------------------
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	//4)add content
	//------------------------------------------------------------------	
	
	//read data
	read_data();		//read data from file into NET_SEND_BUF(8mb)

	//add data
	str_len	= strlen((s8*)NET_SEND_BUF);
	strcpy(pop3_con, (s8*)NET_SEND_BUF, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//------------------------------------------------------------------
	//5)\r\n
	//------------------------------------------------------------------		
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	//6).
	//------------------------------------------------------------------	
	*pop3_con		= '.';		// .
		
	pop3_con		+= 1;
	total_len		+= 1;

	//------------------------------------------------------------------
	//7)\r\n
	//------------------------------------------------------------------	
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// pop3 state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CMD_OK;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_POP3;
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
// pop3_get_request
//===========================================================================
PUBLIC	s32 	pop3_send_quit_ok(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* pop3_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(p_ppop3_state + STS_P_OK * POP3_STATE_SIZE);
	strcpy(pop3_con, p_ppop3_state + STS_P_OK * POP3_STATE_SIZE, str_len);

	pop3_con		+= str_len;
	total_len		+= str_len;

	//2)\r\n
	*pop3_con		= '\r';		// \r
	*(pop3_con+1)		= '\n';		// \n
	
	pop3_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// pop3 state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CMD_OK;
	s32 old_state	= pop3_state;
	pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_POP3;
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + total_len;	//
	u8 ctrl_bit		= FIN_BIT | ACK_BIT; 
	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	//-------------------------------------------------------------------------
	// ip process
	//-------------------------------------------------------------------------
	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}
