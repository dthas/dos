//===========================================================================
// tcp.c
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
#include	"prototype.h"

extern u32 	ticket;

PRIVATE	s8 	*tmp_tcp_data = NET_TEMP_BUF;		//0x9MB, define in global.h

PRIVATE	s32	tcp_state_tab[NR_STATUS][NR_ACTION];

PRIVATE	void	tcp_statetab_init();

//===========================================================================
// add_tcp_header
//===========================================================================
PUBLIC	void 	add_tcp_header(struct s_package *pkg, struct iaddr src_ip, u16 tcp_len, struct iaddr dst_ip, u8 protocol,u16 src_port, u16 dst_port, u32 seq, u32 ack, u8 header_len, u8 ctrl_bit, u16 winsize, u16 upointer)
{
	
	//-------------------------------------------------------------------------
	// make checksum
	//-------------------------------------------------------------------------

	//1)build test space and copy tcp data
	s8 *dst_addr	= &tmp_tcp_data[TCP_HEADER_LENGTH + PTCP_HEADER_LENGTH];
	s8 *src_addr	= &(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH]);
	s32 len		= tcp_len - TCP_HEADER_LENGTH;

	strcpy(dst_addr, src_addr, len);

	struct s_g_tcp_header *g = (struct s_g_tcp_header *)tmp_tcp_data;

	//2)create tcp psedu-header
	struct s_ptcp_header *p = &(g->p);
	
	p->src_ip	= src_ip;
	p->dst_ip	= dst_ip;
	p->zero		= 0;
	p->protocol	= protocol;
	p->tcp_len	= big_little_16(tcp_len);

	//3) create tcp header
	struct s_tcp_header *t = &(g->t);
	
	t->src_port	= big_little_16(src_port);
	t->dst_port	= big_little_16(dst_port);

	t->seq		= big_little_32(seq);
	t->ack		= big_little_32(ack);
	t->header_len	= header_len<<4;
	t->ctrl_bit	= ctrl_bit;
	t->winsize	= big_little_16(winsize);
	t->checksum	= 0;
	t->upointer	= big_little_16(upointer);

	t->checksum	= makechksum(tmp_tcp_data,(tcp_len+PTCP_HEADER_LENGTH));
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------

	struct s_tcp_header * tcp = (struct s_tcp_header *)(&(pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	tcp->src_port	= big_little_16(src_port);
	tcp->dst_port	= big_little_16(dst_port);
	tcp->seq	= big_little_32(seq);
	tcp->ack	= big_little_32(ack);
	tcp->header_len	= header_len<<4;
	tcp->ctrl_bit	= ctrl_bit;
	tcp->winsize	= big_little_16(winsize);
	tcp->checksum	= t->checksum;
	tcp->upointer	= big_little_16(upointer);
}

//===========================================================================
// init tcp
//===========================================================================
PUBLIC	void	init_tcp()
{
	tcp_statetab_init();

	tcp_state 		= TCP_STATE1;	

	p_send_nbuf	= NET_SEND_BUF;		//8MB   ~ 8.5MB (512KB) global.h
	p_recv_nbuf	= NET_RECV_BUF;		//8.5MB ~ 9MB   (512KB) global.h

	connect_seq	= 0;
	connect_ack	= 0;
}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	tcp_statetab_init()
{
//------for test
	char *str = "-tcp_state_tab init-";
	disp_str(str);	
//------for test
	
	s32 i,j;

	p_tcp_state_tab = tcp_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<NR_STATUS; i++)
	{
		for(j=0; j<NR_ACTION; j++)
		{
			tcp_state_tab[i][j]	= TCP_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// TCP_STATE1 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE1][SEND_SYN]	= TCP_STATE3;
	tcp_state_tab[TCP_STATE1][RECV_SYN]	= TCP_STATE12;
	
	//------------------------------------------------------------------
	// TCP_STATE3 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE3][RECV_SYN_ACK]	= TCP_STATE11;

	//------------------------------------------------------------------
	// TCP_STATE4 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE4][RECV_ACK]	= TCP_STATE5;

	//------------------------------------------------------------------
	// TCP_STATE5 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE5][SEND_FIN_ACK]	= TCP_STATE6;
	tcp_state_tab[TCP_STATE5][RECV_FIN_ACK]	= TCP_STATE9;

	tcp_state_tab[TCP_STATE5][SEND_FIN]	= TCP_STATE6;
	tcp_state_tab[TCP_STATE5][RECV_FIN]	= TCP_STATE14;

	tcp_state_tab[TCP_STATE5][SEND_DATA]	= TCP_STATE5;	//while in state of established, send/receive data will not change its state
	tcp_state_tab[TCP_STATE5][RECV_DATA]	= TCP_STATE5;
	tcp_state_tab[TCP_STATE5][RECV_ACK]	= TCP_STATE5;
	tcp_state_tab[TCP_STATE5][SEND_ACK]	= TCP_STATE5;

	//------------------------------------------------------------------
	// TCP_STATE6 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE6][RECV_ACK]	= TCP_STATE7;
	tcp_state_tab[TCP_STATE6][RECV_FIN_ACK]	= TCP_STATE13;

	//------------------------------------------------------------------
	// TCP_STATE7 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE7][RECV_ACK]	= TCP_STATE7;

	tcp_state_tab[TCP_STATE7][RECV_FIN]	= TCP_STATE13;
	tcp_state_tab[TCP_STATE7][RECV_FIN_ACK]	= TCP_STATE13;

	//------------------------------------------------------------------
	// TCP_STATE8 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE8][EX_TIME]	= TCP_STATE1;

	//------------------------------------------------------------------
	// TCP_STATE9 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE9][SEND_FIN]	= TCP_STATE10;
	tcp_state_tab[TCP_STATE9][SEND_FIN_ACK]	= TCP_STATE10;

	//------------------------------------------------------------------
	// TCP_STATE10 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE10][RECV_ACK]	= TCP_STATE_END;

	//------------------------------------------------------------------
	// TCP_STATE11 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE11][SEND_ACK]	= TCP_STATE5;

	//------------------------------------------------------------------
	// TCP_STATE12
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE12][SEND_SYN_ACK]= TCP_STATE4;

	//------------------------------------------------------------------
	// TCP_STATE13 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE13][SEND_ACK]	= TCP_STATE8;

	//------------------------------------------------------------------
	// TCP_STATE14 
	//------------------------------------------------------------------
	tcp_state_tab[TCP_STATE14][SEND_ACK]	= TCP_STATE9;
	tcp_state_tab[TCP_STATE14][SEND_FIN_ACK]= TCP_STATE10;
}


//===========================================================================
// tcp_process
//===========================================================================
PUBLIC	void 	tcp_process(struct s_package *src_pkg, struct s_package *dst_pkg, u16 src_tcp_data_len, u16 dst_tcp_data_len, u8 tos)
{
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	u8 flg_f	= tcph->ctrl_bit & FIN_BIT; 
	u8 flg_a	= tcph->ctrl_bit & ACK_BIT;
	u8 flg_s	= tcph->ctrl_bit & SYN_BIT;

	u8 action	= -1;
	u8 ctrl_bit	= 0;
	
	if(flg_f && flg_a)
	{
		action		= RECV_FIN_ACK;		
		
		if(tcp_state == TCP_STATE6)
		{
			ctrl_bit	= ACK_BIT;
		}
		else
		{
			ctrl_bit	= FIN_BIT | ACK_BIT;
		}
	}
	else if(flg_s && flg_a)
	{
		action		= RECV_SYN_ACK;
		ctrl_bit	= ACK_BIT;
	}
	else if(flg_f)
	{
		action 		= RECV_FIN;
		ctrl_bit	= ACK_BIT;
	}
	else if(flg_s)
	{
		action 		= RECV_SYN;
		ctrl_bit	= SYN_BIT | ACK_BIT;
	}
	else if(flg_a)
	{
		action 		= RECV_ACK;		
		ctrl_bit	= ACK_BIT;
	}
	else
	{		
		return;
	}

	if(action)
	{
		s32 old_tcp_state	= tcp_state;
		tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + action);

		if(tcp_state == TCP_STATE_END)
		{
			return;		//it come from TCP_STATE10, which mean the previous connection is close
		}

		if((old_tcp_state == TCP_STATE4) && (tcp_state == TCP_STATE5))
		{
			seq		= little_big_32(tcph->ack);
			ack		= little_big_32(tcph->seq);

			return;		// while receive ack from client, server no need to send ack back. (SYN_RECV ----> ESTABLISHED)
		}

		if((old_tcp_state == TCP_STATE6) && (tcp_state == TCP_STATE7))
		{
			return;		
		}

		if((old_tcp_state == TCP_STATE7) && (tcp_state == TCP_STATE7))
		{
			return;		
		}
				
		seq	= little_big_32(tcph->ack);

		if(src_tcp_data_len)
		{
			ack	= little_big_32(tcph->seq) + src_tcp_data_len;
		}
		else
		{
			ack	= little_big_32(tcph->seq) + 1;
		} 


		switch(tcp_state)
		{
			case TCP_STATE9:				
				action	= SEND_FIN_ACK;
				tcp_state 	= *(p_tcp_state_tab + tcp_state * NR_ACTION + action);
				break;
			case TCP_STATE11:
				action	= SEND_ACK;
				tcp_state 	= *(p_tcp_state_tab + tcp_state * NR_ACTION + action);				
				break;
			case TCP_STATE12:
				//send SYN+ACK
				seq	= get_seq();
				
				action	= SEND_SYN_ACK;
				tcp_state 	= *(p_tcp_state_tab + tcp_state * NR_ACTION + action);				
				break;
			case TCP_STATE13:				
				action	= SEND_ACK;
				tcp_state 	= *(p_tcp_state_tab + tcp_state * NR_ACTION + action);
				break;
			case TCP_STATE14:
				//send FIN+ACK;
				action	= SEND_FIN_ACK;
				tcp_state 	= *(p_tcp_state_tab + tcp_state * NR_ACTION + action);
				break;
			default:
				break;
		}
	}

	if(tcp_state)
	{
		//-------------------------------------------------------------------------
		// add tcp option
		//-------------------------------------------------------------------------
		u16 option_len	= 0;

		//-------------------------------------------------------------------------
		// add tcp header
		//-------------------------------------------------------------------------
		struct iaddr src_ip	= iph->dst_ip;
		struct iaddr dst_ip	= iph->src_ip;
		u8 protocol		= PROTOCOL_TCP;		//6
		u16 src_port		= little_big_16(tcph->dst_port);
		u16 dst_port		= little_big_16(tcph->src_port);
		u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
		u16 winsize		= TCP_WINSIZE_DEFAULT;
		u16 upointer		= 0;
		u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + dst_tcp_data_len;
		
		add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

		ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
	}
}

//===========================================================================
// rand_val
//===========================================================================
PUBLIC	u32	get_seq(void)
{
	u32 seq	= ticket & 0xFFFF;

	return seq;
}
