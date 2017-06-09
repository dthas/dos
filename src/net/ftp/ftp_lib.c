//===========================================================================
// ftp_lib.c
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

extern	u32	ticket;
//===========================================================================
// ftp_save_parm
//===========================================================================
PUBLIC void	ftp_parm_save()
{
	ftp_control_port	= client_port;
	ftp_control_state	= tcp_state;	
}

//===========================================================================
// ftp_save_parm
//===========================================================================
PUBLIC void	ftp_parm_restore()
{
	client_port	= ftp_control_port;
	
	tcp_state		= ftp_control_state;
	seq		= ftp_control_seq;
	ack		= ftp_control_ack;
}

//===========================================================================
// ftp_save_parm
//===========================================================================
PUBLIC void	ftp_parm_init()
{	
	client_port		= ftp_data_port;
	tcp_state 			= TCP_STATE1;
	flg_ftp_data_start	= TRUE;
	flg_ftp_data_end	= FALSE;

	recv_buf_start_addr	= NET_RECV_BUF;		//0x880000 , 8.5MB ~ 9MB   (512KB) . Setting in global.h
}

//===========================================================================
// ftpopen(struct iaddr ip)
//===========================================================================
PUBLIC void	ftp_open(struct s_package *dst_pkg, struct iaddr dst_ip)
{
	tcp_state 		= TCP_STATE1;
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_SYN);

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
	u16 dst_port		= PORT_FTP_CTRL;
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	ack			= 0;
	seq			= get_seq();	
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + 0;	//ftp control without any data
	u8 ctrl_bit		= SYN_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);
	
	ftp_control_seq		= seq + 0;
	ftp_control_ack		= ack;	

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);	
}

//===========================================================================
// ftpquit(struct iaddr ip)
//===========================================================================
PUBLIC void	ftp_quit(struct s_package *dst_pkg, struct iaddr dst_ip)
{
	u16 option_len	= 0;
	u16 data_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	u8 *dst_d	= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));

	*dst_d++	= 'Q';
	*dst_d++	= 'U';
	*dst_d++	= 'I';
	*dst_d++	= 'T';
	
	*dst_d++	= 0xd;
	*dst_d++	= 0xa;

	data_len	= 6;

	//-------------------------------------------------------------------------
	// add tcp option(data)
	//-------------------------------------------------------------------------
	

	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;	
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_FTP_CTRL;
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
// ftp_user(struct s_package *dst_pkg, struct iaddr dst_ip, s8 *user)
//===========================================================================
PUBLIC void	ftp_user(struct s_package *dst_pkg, struct iaddr dst_ip)
{	
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);

	u16 option_len	= 0;
	u16 data_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	s8 user[10]		= "ftp01";

	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));

	*dst_d++		= 'U';
	*dst_d++		= 'S';
	*dst_d++		= 'E';
	*dst_d++		= 'R';
	*dst_d++		= ' ';

	u32	name_len	= strlen(user);
	strcpy(dst_d, user, name_len);

	*(dst_d + name_len)	= 0xd;
	*(dst_d + name_len +1)	= 0xa;

	data_len		= 7 + name_len;

	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;	
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_FTP_CTRL;
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
// ftp_passwd(struct s_package *dst_pkg, struct iaddr dst_ip)
//===========================================================================
PUBLIC void	ftp_passwd(struct s_package *dst_pkg, struct iaddr dst_ip)
{	
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);

	u16 option_len	= 0;
	u16 data_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	s8 passwd[10]		= "123456";

	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));

	*dst_d++		= 'P';
	*dst_d++		= 'A';
	*dst_d++		= 'S';
	*dst_d++		= 'S';
	*dst_d++		= ' ';

	u32	passwd_len	= strlen(passwd);
	strcpy(dst_d, passwd, passwd_len);

	*(dst_d + passwd_len)	= 0xd;
	*(dst_d + passwd_len +1)= 0xa;

	data_len		= 7 + passwd_len;

	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;	
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_FTP_CTRL;
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
// ftpclose(struct iaddr ip)
//===========================================================================
PUBLIC void	ftp_close(struct s_package *src_pkg, struct s_package *dst_pkg)
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

	flg_ftp_close		= FALSE;

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);	
}

//===========================================================================
// ftp_passwd(struct s_package *dst_pkg, struct iaddr dst_ip)
//===========================================================================
PUBLIC u32	ftp_ls(struct s_package *dst_pkg, struct iaddr dst_ip)
{
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);

	u16 option_len	= 0;
	u16 data_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	s8	rep[10]		= "LIST -l\r\n";

	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len			= 9;
	strcpy(dst_d, rep, len);

	data_len		= len;
	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;	
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_FTP_CTRL;
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + data_len;	//ftp control without any data
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);
	
	ftp_control_seq		= seq + data_len;
	ftp_control_ack		= ack;	

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);	

	return data_len;
}

//===========================================================================
//ftp_get(struct s_package *dst_pkg, struct iaddr dst_ip)
//===========================================================================
PUBLIC u32	ftp_get(struct s_package *dst_pkg, struct iaddr dst_ip)
{
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);

	u16 option_len	= 0;
	u16 data_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------	
	s8	rep[20]		= "RETR download.txt\r\n";

	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));	
	u32 len			= 19;
	strcpy(dst_d, rep, len);

	data_len		= len;
	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;	
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_FTP_CTRL;
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + data_len;	//ftp control without any data
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);
	
	ftp_control_seq		= seq + data_len;
	ftp_control_ack		= ack;

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);

	return data_len;
}


//===========================================================================
// ftp_passwd(struct s_package *dst_pkg, struct iaddr dst_ip)
//===========================================================================
PUBLIC u32	ftp_put(struct s_package *dst_pkg, struct iaddr dst_ip)
{
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);

	u16 option_len	= 0;
	u16 data_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	// 1) copy data from file to NET_SEND_BUF(0x800000)
	read("/test/upload.txt", FILE_MODE_R);			//read file data into 7MB

	s32	src_len	= strlen((s8*)VFS_TMP_DATA);	
	strcpy((s8*)NET_SEND_BUF, (s8*)VFS_TMP_DATA, src_len); 	//copy data from 7MB to 8MB
	
	send_buf_start_addr	= NET_SEND_BUF;

	// 2) setting the tcp command
	s8	rep[21]		= "STOR upload.txt\r\n";

	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len			= 20;
	strcpy(dst_d, rep, len);

	data_len		= len;
	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_FTP_CTRL;
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + data_len;	//ftp control without any data
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);
	
	ftp_control_seq		= seq + data_len;
	ftp_control_ack		= ack;	

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);	

	return data_len;
}

//===========================================================================
//ftp_get(struct s_package *dst_pkg, struct iaddr dst_ip)
//===========================================================================
PUBLIC u32	ftp_type(struct s_package *dst_pkg, struct iaddr dst_ip)
{
	tcp_state 		= *(p_tcp_state_tab + tcp_state * NR_ACTION + SEND_ACK);

	u16 option_len	= 0;
	u16 data_len	= 0;

	//-------------------------------------------------------------------------
	// add tcp data
	//-------------------------------------------------------------------------
	s8	rep[9]		= "TYPE I\r\n";

	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	u32 len			= 8;
	strcpy(dst_d, rep, len);

	data_len		= len;
	//-------------------------------------------------------------------------
	// add tcp option
	//-------------------------------------------------------------------------
	
	
	//-------------------------------------------------------------------------
	// add tcp header
	//-------------------------------------------------------------------------
	struct iaddr src_ip	= pnet_dev->ip;	
	u8 protocol		= PROTOCOL_TCP;		//6
	u16 src_port		= client_port;
	u16 dst_port		= PORT_FTP_CTRL;
	u8 header_len		= (TCP_HEADER_LENGTH + option_len) / 4;    // 40 / 4 = 10
	u16 winsize		= TCP_WINSIZE_DEFAULT;
	u16 upointer		= 0;
	u16 tcp_len		= (TCP_HEADER_LENGTH + option_len) + data_len;	//ftp control without any data
	u8 ctrl_bit		= ACK_BIT; 

	add_tcp_header(dst_pkg, src_ip, tcp_len, dst_ip, protocol, src_port, dst_port, seq, ack, header_len, ctrl_bit, winsize, upointer);
	
	ftp_control_seq		= seq + data_len;
	ftp_control_ack		= ack;

	ip_process(dst_pkg, pnet_dev->ip, dst_ip, PROTOCOL_TCP, tcp_len);	

	return data_len;
}
