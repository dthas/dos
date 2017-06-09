//===========================================================================
// ftp_cmd.c
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
#include	"route.h"
#include	"ipv4.h"
#include	"icmp.h"
#include	"net.h"
#include	"tcp.h"
#include	"ftp.h"
#include	"prototype.h"

extern u32 ticket;
//===========================================================================
// ftp_cmd_user
//===========================================================================
PUBLIC s32	ftp_cmd_user_pre(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	

	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u16 option_len	= 0;
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);	
	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	s8	rep[21]	= "220 (vsFTPd 2.2.2)\r\n";
	u8 *dst_d	= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len		= 20;
	strcpy(dst_d, rep, len);

	flg_ftp_user	= TRUE;

	u16 data_len	= len;
	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	struct iaddr dst_ip	= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_FTP_CTRL;
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + data_len;	//ftp control without any data
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

	ftp_control_seq		= seq + data_len;
	ftp_control_ack		= ack;

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);	
}

//===========================================================================
// ftp_cmd_user
//===========================================================================
PUBLIC s32	ftp_cmd_user(struct s_package *dst_pkg)
{
	u16 option_len	= 0;
				
	s8	rep[35]	= "331 Please specify the password\r\n";
	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	u8 *dst_d	= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len		= 34;
	strcpy(dst_d, rep, len);

	flg_ftp_pass	= TRUE;
	
	ftp_control_seq		= seq + len;
	ftp_control_ack		= ack;

	return len;
}

//===========================================================================
// ftp_cmd_user
//===========================================================================
PUBLIC s32	ftp_cmd_pass(struct s_package *dst_pkg)
{
	u16 option_len	= 0;
				
	s8	rep[24]	= "230 Login successful\r\n";
	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	u8 *dst_d	= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len		= 23;
	strcpy(dst_d, rep, len);

	flg_ftp_login	= TRUE;
	
	ftp_control_seq		= seq + len;
	ftp_control_ack		= ack;

	return len;
}

//===========================================================================
// ftp_cmd_user
//===========================================================================
PUBLIC s32	ftp_cmd_syst(struct s_package *dst_pkg)
{
	u16 option_len	= 0;
				
	s8	rep[20]	= "215 UNIX TYPE L8\r\n";
	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	u8 *dst_d	= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len		= 19;
	strcpy(dst_d, rep, len);

	flg_ftp_syst	= TRUE;
	
	ftp_control_seq		= seq + len;
	ftp_control_ack		= ack;

	return len;
}

//===========================================================================
// ftp_cmd_user
//===========================================================================
PUBLIC s32	ftp_cmd_quit(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u16 option_len	= 0;
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);	
	
	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	s8	rep[15]	= "221 Goodbye\r\n";

	u8 *dst_d	= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len		= 14;
	strcpy(dst_d, rep, len);

	flg_ftp_quit	= TRUE;

	u16 data_len	= len;
	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	struct iaddr dst_ip	= iph->src_ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= PORT_FTP_CTRL;
	u16 dst_port		= big_little_16(tcph->src_port);
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + data_len;	//ftp control without any data
	u8 ctrl_bit		= ACK_BIT;	

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);
	
	ftp_control_seq		= seq + data_len;
	ftp_control_ack		= ack;

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);	
}

//===========================================================================
// ftp_cmd_port
//===========================================================================
PUBLIC s32	ftp_cmd_port(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u16 option_len	= 0;
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);

	
	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------	
	s8	rep[29]	= "PORT 192,168,1,140,000,000\r\n";	//203 * 256 + 163 = 52131
	
	ftp_data_port	= ticket & 0xFFFF;
	if(ftp_data_port < 25600)
	{
		ftp_data_port 	+= 25600;
	}
	u16	t1	= ftp_data_port / 256;
	u16	t2	= ftp_data_port % 256;
	s8	tmpb[3];

	clear_buf(tmpb, 3);
	i2s(t1, tmpb);
	strcpy(&rep[19], tmpb, 3);

	clear_buf(tmpb, 3);
	i2s(t2, tmpb);
	if(t2 < 100)
	{
		strcpy(&rep[24], tmpb, 2);
	}
	else
	{
		strcpy(&rep[23], tmpb, 3);
	}
		
	u8 *dst_d	= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len		= 28;
	strcpy(dst_d, rep, len);

	flg_ftp_port	= TRUE;

	u16 data_len	= len;
	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
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
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);
	
	ftp_control_seq		= seq + data_len;
	ftp_control_ack		= ack;

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);
}

//===========================================================================
// ftp_cmd_user
//===========================================================================
PUBLIC s32	ftp_cmd_put(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;


	u16 option_len	= 0;
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);	
	
	s8 *upl		= (s8*)NET_SEND_BUF;		//
	u32 len		= strlen(upl);
	u16 data_len_d	= len+2;

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
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + data_len_d;	//ftp control without any data
	u8 ctrl_bit		= ACK_BIT; 

	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------	
	if((len + 2) <= IP_DATA_MAX)
	{
		s8 *dst_d	= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
		strcpy(dst_d, upl, len);

		*(dst_d+len)		= '\r';
		*(dst_d+len+1)		= '\n';

		flg_ftp_put		= FALSE;
		data_size_total_bytes	= data_len_d;	
		
		seq			= little_big_32(tcph->ack);
		ack			= little_big_32(tcph->seq);

		connect_seq		= ack;
		connect_ack		= seq;		

		add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

		//Since after ftp_cmd_put(), it's ftp_close(), so the seq must be updated for ftp_close(but no need for ack)
		seq			= seq + data_len_d;
		ip_process(dst_pkg, src_ip, dst_ip, PROTOCOL_TCP, tcp_len);		
	}
	else
	{		
		s8	*dst, *src;
		s16	t_offset, t_ip_len, t_data_len;
		s32 	data_len = len;
		u16	total_header_len	= FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len;

		data_size_total_bytes	= 0;
		
		seq		= little_big_32(tcph->ack);
		ack		= little_big_32(tcph->seq);

		connect_seq	= ack;
		connect_ack	= seq;
		
		for(t_offset=0, t_ip_len=0, t_data_len=0; data_len >0 ; data_len -= IP_DATA_MAX, t_offset += t_data_len, data_size_total_bytes += t_data_len)
		{
			if(data_len > IP_DATA_MAX)
			{
				t_data_len	= IP_DATA_MAX;
			}
			else
			{
				t_data_len	= data_len;
			}

			//-------------------------------------------------------------------------
			// //copy the data
			//-------------------------------------------------------------------------
			dst		= (s8*)(&(dst_pkg->buffer[total_header_len]));
			src		= (s8*)(&(upl[t_offset]));
			len		= t_data_len;
			strcpy(dst, src, len);		//copy the data			

			tcp_len	= TCP_HEADER_LENGTH + option_len + t_data_len;			
		
			add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);

			//Since after ftp_cmd_put(), it's ftp_close(), so the seq must be updated for ftp_close(but no need for ack)
			seq			= seq + t_data_len;			
			
			t_ip_len	= IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len + t_data_len;		

			//-------------------------------------------------------------------------
			// send the pkg
			//-------------------------------------------------------------------------
			ipv4_send(dst_pkg, src_ip, IP_TTL, dst_ip, protocol, 0, t_ip_len, 0,  IP_FLAG_NO_FRAGMENT);		
		}		

		flg_ftp_put		= FALSE;		
	}
}
