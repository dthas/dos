//===========================================================================
// telnet_server.c
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
#include	"telnet.h"
#include	"prototype.h"

PRIVATE s8	usertitle[10]	= "Login:";
PRIVATE s8	passtitle[10]	= "Password:";
PRIVATE s8	cmd_info[20]	= "ls -l\r\n";
PRIVATE s8	res_info[100]	= "/src\r\n/test\r\n/\r\n";
PRIVATE s8	quit_info[30]	= "connection closed\r\n";
PRIVATE s8	logintitle[10]	= "gah>";


//===========================================================================
// telnet_get_request
//===========================================================================
PUBLIC	s32 	telnet_send_usertitle(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* telnet_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(usertitle);
	strcpy(telnet_con, usertitle, str_len);

	telnet_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*telnet_con		= 0x20;		//space

	telnet_con		+= 1;
	total_len		+= 1;
		
	//-------------------------------------------------------------------------
	// telnet state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_USERTITLE;
	s32 old_state	= telnet_state;
	telnet_state	= *(p_telnet_state_tab + telnet_state * TELNET_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_TELNET;
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
// telnet_get_request
//===========================================================================
PUBLIC	s32 	telnet_send_passtitle(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* telnet_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(passtitle);
	strcpy(telnet_con, passtitle, str_len);

	telnet_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*telnet_con		= 0x20;		//space

	telnet_con		+= 1;
	total_len		+= 1;
		
	//-------------------------------------------------------------------------
	// telnet state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_PASSTITLE;
	s32 old_state	= telnet_state;
	telnet_state	= *(p_telnet_state_tab + telnet_state * TELNET_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_TELNET;
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
// telnet_get_request
//===========================================================================
PUBLIC	s32 	telnet_send_logintitle(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* telnet_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	str_len	= strlen(logintitle);
	strcpy(telnet_con, logintitle, str_len);

	telnet_con		+= str_len;
	total_len		+= str_len;

	//2)add sp
	*telnet_con		= 0x20;		//space

	telnet_con		+= 1;
	total_len		+= 1;
		
	//-------------------------------------------------------------------------
	// telnet state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_TELNET;
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
// telnet_get_request
//===========================================================================
PUBLIC	s32 	telnet_res_ls(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* telnet_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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

	u8 *src_d		= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + src_ip_header_len + src_tcp_header_len]));
	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));

	//------------------------------------------------------------------
	// add require line
	//------------------------------------------------------------------
	//1)add state
	str_len	= strlen(res_info);
	strcpy(telnet_con, res_info, str_len);

	telnet_con		+= str_len;
	total_len		+= str_len;

	//2)add prompt
	str_len	= strlen(logintitle);
	strcpy(telnet_con, logintitle, str_len);

	telnet_con		+= str_len;
	total_len		+= str_len;

	
	//-------------------------------------------------------------------------
	// telnet state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_RES;
	s32 old_state	= telnet_state;
	telnet_state	= *(p_telnet_state_tab + telnet_state * TELNET_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_TELNET;
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
// telnet_get_request
//===========================================================================
PUBLIC	s32 	telnet_res_quit(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* telnet_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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

	u8 *src_d		= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + src_ip_header_len + src_tcp_header_len]));
	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));

	//------------------------------------------------------------------
	// add require line
	//------------------------------------------------------------------
	//1)add state
	str_len	= strlen(quit_info);
	strcpy(telnet_con, quit_info, str_len);

	telnet_con		+= str_len;
	total_len		+= str_len;

	//-------------------------------------------------------------------------
	// telnet state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_QUIT;
	s32 old_state	= telnet_state;
	telnet_state	= *(p_telnet_state_tab + telnet_state * TELNET_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_TELNET;
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



//===========================================================================
// telnet_get_request
//===========================================================================
PUBLIC	s32 	telnet_res_ctrl(struct iaddr dst_ip,  struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	s8* telnet_con			= (s8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]));

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
	u8 *src_u8	= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + src_ip_header_len + src_tcp_header_len]));
	u8 *dst_u8	= telnet_con;

	u16 counter	= src_tcp_data_len;

	for(str_len=0; counter > 0; )
	{
		if(*(src_u8+1) != CTRL_SB)
		{
			struct s_ctrl_cmd_simp *src_pcc = src_u8;
			struct s_ctrl_cmd_simp *dst_pcc = dst_u8;

			if(src_pcc->p2 == CTRL_DO)
			{			
				switch(src_pcc->p3)
				{
					case OPT_TERM_SPEED:
						*(dst_u8+0)	= CTRL_IAC;
						*(dst_u8+1)	= CTRL_SB;
						*(dst_u8+2)	= OPT_TERM_SPEED;
						*(dst_u8+3)	= 0x0;
						*(dst_u8+4)	= 0x33;
						*(dst_u8+5)	= 0x38;
						*(dst_u8+6)	= 0x34;
						*(dst_u8+7)	= 0x30;
						*(dst_u8+8)	= 0x30;
						*(dst_u8+9)	= 0x2c;
						*(dst_u8+10)	= 0x33;
						*(dst_u8+11)	= 0x38;
						*(dst_u8+12)	= 0x34;
						*(dst_u8+13)	= 0x30;
						*(dst_u8+14)	= 0x30;
						*(dst_u8+15)	= CTRL_IAC;
						*(dst_u8+16)	= CTRL_SE;

						dst_u8 		+= 17;
						str_len 	+= 17;
						break;

					case OPT_XWINDOW:
						*(dst_u8+0)	= CTRL_IAC;
						*(dst_u8+1)	= CTRL_SB;
						*(dst_u8+2)	= OPT_XWINDOW;
						*(dst_u8+3)	= 0x0;
						*(dst_u8+4)	= 'g';
						*(dst_u8+5)	= 'a';
						*(dst_u8+6)	= 'h';
						*(dst_u8+7)	= ':';
						*(dst_u8+8)	= '0';
						*(dst_u8+9)	= '.';
						*(dst_u8+10)	= '0';
						*(dst_u8+11)	= CTRL_IAC;
						*(dst_u8+12)	= CTRL_SE;

						dst_u8 		+= 13;
						str_len 	+= 13;
						break;

					case OPT_NEWENV:
						*(dst_u8+0)	= CTRL_IAC;
						*(dst_u8+1)	= CTRL_SB;
						*(dst_u8+2)	= OPT_NEWENV;
						*(dst_u8+3)	= 0x0;
						*(dst_u8+4)	= 0x0;
						*(dst_u8+5)	= 'g';
						*(dst_u8+6)	= 'a';
						*(dst_u8+7)	= 'h';
						*(dst_u8+8)	= ':';
						*(dst_u8+9)	= '0';
						*(dst_u8+10)	= '.';
						*(dst_u8+11)	= '0';
						*(dst_u8+12)	= CTRL_IAC;
						*(dst_u8+13)	= CTRL_SE;

						dst_u8 		+= 14;
						str_len 	+= 14;
						break;

					case OPT_TERM_TYPE:
						*(dst_u8+0)	= CTRL_IAC;
						*(dst_u8+1)	= CTRL_SB;
						*(dst_u8+2)	= OPT_TERM_TYPE;
						*(dst_u8+3)	= 0x0;
						*(dst_u8+4)	= 'x';
						*(dst_u8+5)	= 't';
						*(dst_u8+6)	= 'e';
						*(dst_u8+7)	= 'r';
						*(dst_u8+8)	= 'm';
						*(dst_u8+9)	= CTRL_IAC;
						*(dst_u8+10)	= CTRL_SE;

						dst_u8 		+= 11;
						str_len 	+= 11;
						break;

					case OPT_ECHO:
						*(dst_u8 + 0)	= CTRL_IAC;
						*(dst_u8 + 1)	= CTRL_WILL;
						*(dst_u8 + 2)	= OPT_ECHO;

						dst_u8 		+= 3;
						str_len 	+= 3;
						break;

					default:
						*(dst_u8 + 0)	= CTRL_IAC;
						*(dst_u8 + 1)	= CTRL_WILL;
						*(dst_u8 + 2)	= src_pcc->p3;

						dst_u8 		+= 3;
						str_len 	+= 3;
						break;
				}
			}
			src_u8		+= 3;
			counter		-= 3;
		}
		else
		{
			src_u8		+= 6;
			counter		-= 6;
		}
	}

	telnet_con		+= str_len;
	total_len		+= str_len;

	//-------------------------------------------------------------------------
	// telnet state change(change to HTTP_STATE2::establish)
	//-------------------------------------------------------------------------
	u8 src_action	= SEND_CTRL;
	s32 old_state	= telnet_state;
	telnet_state	= *(p_telnet_state_tab + telnet_state * TELNET_NR_ACTION + src_action);	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= iph->dst_ip;
	dst_ip			= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= big_little_16(tcph->dst_port);
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
