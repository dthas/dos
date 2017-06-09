//===========================================================================
// smtp_client.c
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

PRIVATE	s8	smtp_cmd[NR_CMD][CMD_SIZE] = {"EHLO","MAIL FROM","RCPT TO","DATA","QUIT","RSET","VRFY","NOOP", "TURN",
						"EXPN", "HELP", "SEND FROM", "SMOL FROM", "SMAL FROM", "Received:"};

PRIVATE	s8	server_smtp_cmd[NR_CMD][CMD_SIZE] = {"EHLO","MAIL","RCPT","DATA","QUIT","RSET","VRFY","NOOP", "TURN",
						"EXPN", "HELP", "SEND", "SMOL", "SMAL", "Received:"};

PRIVATE	s8	smtp_state_code[NR_SMTP_STATE][SMTP_STATE_SIZE]	= {"211", "214", "220", "221", "250", "251",
									"354", "421", "450", "451", "452",
									"500", "501", "502", "503", "504", "550", "551", "552", "553", "554"};	

PRIVATE s8	machine_name[10]	= "dthas";

PRIVATE s8	receiver[20]		= "Received: by dthas";
PRIVATE s8	src_mailbox[22]		= "<test@test.com>";
PRIVATE s8	dst_mailbox[22]		= "<test_in@dthas.com>";
PRIVATE s8	mail_subject[25]	= "Subject: test10001";
PRIVATE s8	mail_data[50]		= "aaaaaaaaaaakkkkkkkkvvvvvvveeeeeeeeee";	

//===========================================================================
// parm_init()
//===========================================================================
PUBLIC	void 	smtp_parm_init()
{
	p_smtp_cmd		= smtp_cmd;
	p_ssmtp_state		= smtp_state_code;

	p_server_smtp_cmd	= server_smtp_cmd;
	
	flg_client		= FALSE;
}


//===========================================================================
// smtp_get_request
//===========================================================================
PUBLIC	s32 	smtp_send_sys(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{

}


//===========================================================================
// smtp_get_request
//===========================================================================
PUBLIC	s32 	smtp_send_ehlo(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
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
	//1)add method
	str_len	= strlen(&smtp_cmd[CMD_EHLO][0]);
	strcpy(smtp_con, &smtp_cmd[CMD_EHLO][0], str_len);

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
	u8 src_action	= SEND_HELO;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_SMTP;
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
// smtp_send_mailfrom
//===========================================================================
PUBLIC	s32 	smtp_send_mailfrom(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(&smtp_cmd[CMD_MAILFROM][0]);
	strcpy(smtp_con, &smtp_cmd[CMD_MAILFROM][0], str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)add comma
	*smtp_con		= 0x3a;		// :

	smtp_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(src_mailbox);
	strcpy(smtp_con, src_mailbox, str_len);

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
	u8 src_action	= SEND_MAILFROM;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_SMTP;
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
// smtp_send_rcptto
//===========================================================================
PUBLIC	s32 	smtp_send_rcptto(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(&smtp_cmd[CMD_RCPTTO][0]);
	strcpy(smtp_con, &smtp_cmd[CMD_RCPTTO][0], str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)add comma
	*smtp_con		= 0x3a;		// :

	smtp_con		+= 1;
	total_len		+= 1;

	//3)add param
	str_len	= strlen(dst_mailbox);
	strcpy(smtp_con, dst_mailbox, str_len);

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
	u8 src_action	= SEND_RCPTTO;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_SMTP;
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
// smtp_send_data
//===========================================================================
PUBLIC	s32 	smtp_send_data(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(&smtp_cmd[CMD_DATA][0]);
	strcpy(smtp_con, &smtp_cmd[CMD_DATA][0], str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_DATA;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_SMTP;
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
// smtp_send_rcptto
//===========================================================================
PUBLIC	s32 	smtp_send_mail(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	// add mail receiver
	//------------------------------------------------------------------
	//1)add receiver
	str_len	= strlen(receiver);
	strcpy(smtp_con, receiver, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// add dst mail box
	//------------------------------------------------------------------
	//1)add TO
	*smtp_con		= 84;		// T
	*(smtp_con+1)		= 111;		// o

	smtp_con		+= 2;
	total_len		+= 2;

	//2)add comma
	*smtp_con		= 0x3a;		// :

	smtp_con		+= 1;
	total_len		+= 1;

	//3)add dst mail box
	str_len	= strlen(dst_mailbox);
	strcpy(smtp_con, dst_mailbox, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//4)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// add mail subject
	//------------------------------------------------------------------
	//5)add subject
	str_len	= strlen(mail_subject);
	strcpy(smtp_con, mail_subject, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//6)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// add src mail box
	//------------------------------------------------------------------
	//7)add FROM
	*smtp_con		= 70;		// F
	*(smtp_con+1)		= 114;		// r
	*(smtp_con+2)		= 111;		// o
	*(smtp_con+3)		= 109;		// m

	smtp_con		+= 4;
	total_len		+= 4;

	//8)add comma
	*smtp_con		= 0x3a;		// :

	smtp_con		+= 1;
	total_len		+= 1;

	//9)add src mail box
	str_len	= strlen(src_mailbox);
	strcpy(smtp_con, src_mailbox, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//10)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// add mail content
	//------------------------------------------------------------------

	//11)add mail data
	str_len	= strlen(mail_data);
	strcpy(smtp_con, mail_data, str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//12)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;

	//------------------------------------------------------------------
	// add mail end
	//------------------------------------------------------------------
	//13)add dot
	*smtp_con		= 0x2e;		// .

	smtp_con		+= 1;
	total_len		+= 1;

	//14)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_SMTP;
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
// smtp_send_quit
//===========================================================================
PUBLIC	s32 	smtp_send_quit(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* smtp_con		= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(&smtp_cmd[CMD_QUIT][0]);
	strcpy(smtp_con, &smtp_cmd[CMD_QUIT][0], str_len);

	smtp_con		+= str_len;
	total_len		+= str_len;

	//2)\r\n
	*smtp_con		= '\r';		// \r
	*(smtp_con+1)		= '\n';		// \n
	
	smtp_con		+= 2;
	total_len		+= 2;
	
	//-------------------------------------------------------------------------
	// smtp state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_QUIT;
	s32 old_state	= smtp_state;
	smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_SMTP;
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
