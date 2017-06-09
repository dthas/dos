//===========================================================================
// ftp.c
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


PUBLIC	u8	ftp_stack[STACK_SIZE];
//===========================================================================
// init tty
//===========================================================================
PUBLIC	void 	init_ftp()
{
	p_ftp_stack 	= ftp_stack;

	flg_ftp_user	= FALSE;
	flg_ftp_pass	= FALSE;
	flg_ftp_login	= FALSE;
	flg_ftp_quit	= FALSE;
	flg_ftp_syst	= FALSE;
	flg_ftp_port	= FALSE;
	flg_ftp_filests	= FALSE;
	flg_ftp_get	= FALSE;
	flg_ftp_put	= FALSE;
	flg_ftp_type	= FALSE;
	flg_ftp_close	= FALSE;
	flg_ftp_ls	= FALSE;

	flg_ftp_data_start	= FALSE;
	flg_ftp_data_end	= FALSE;
			
	send_buf_start_addr	= NET_SEND_BUF;			//0x800000 , 8.5MB ~ 9MB   (512KB) . Setting in global.h
	recv_buf_start_addr	= NET_RECV_BUF;			//0x880000 , 8.5MB ~ 9MB   (512KB) . Setting in global.h

	data_size_total_bytes	= 0;
	
//------for test
	s8 *str = "-init ftp-";
	disp_str(str);	
//------for test

}

//===========================================================================
// net main
//===========================================================================
PUBLIC	void 	ftp_main()
{	
	while(1)
	{
	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		u16 seq_inc;

	
		struct iaddr ip;

		mesg.src_pid 	= PROC_FTP;
		mesg.dest_pid 	= PROC_FTP;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case FTP_CTRL:
					ftp_ctrl_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case FTP_DATA:
					ftp_data_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
			
				//-------------------------------------------------------------------------
				// FTP CONTROL CMD
				//-------------------------------------------------------------------------
				case FTP_OPEN:
					s2ip(&ip, mesg.msg.buf1);
					ftp_open(&dst_pkg, ip);
					break;
				case FTP_QUIT:
					s2ip(&ip, mesg.msg.buf1);
					ftp_quit(&dst_pkg, ip);
					break;
				case FTP_USER:
					s2ip(&ip, mesg.msg.buf1);					
					ftp_user(&dst_pkg, ip);
					break;
				case FTP_PASSWD:
					s2ip(&ip, mesg.msg.buf1);
					ftp_passwd(&dst_pkg, ip);
					break;
				case FTP_TYPE_I:
					s2ip(&ip, mesg.msg.buf1);
					ftp_type(&dst_pkg, ip);
					break;

				//-------------------------------------------------------------------------
				// FTP DATA CMD
				//-------------------------------------------------------------------------
				case FTP_LS:
					
					s2ip(&ip, mesg.msg.buf1);
					ftp_ls(&dst_pkg, ip);

					flg_ftp_ls	= TRUE;

					ftp_parm_save();
					ftp_parm_init();				
					break;

				case FTP_GET:
					
					s2ip(&ip, mesg.msg.buf1);
					ftp_get(&dst_pkg, ip);
		
					flg_ftp_get	= TRUE;

					ftp_parm_save();
					ftp_parm_init();

					break;

				case FTP_PUT:
					
					s2ip(&ip, mesg.msg.buf1);
					ftp_put(&dst_pkg, ip);
		
					flg_ftp_put	= TRUE;

					ftp_parm_save();
					ftp_parm_init();				
					break;
				
				default:
					break;
			}

			if(mesg.func == SYN)
			{
				if(mesg.type != EXIT)
				{
					struct s_mesg mesg1;

					mesg1.dest_pid	= mesg.src_pid;
					mesg1.type	= PROC_READY;
		
					setstatus(&mesg1);
				}
			}
		}	
	}			
}

//===========================================================================
// ftp process
//===========================================================================
PUBLIC	void 	ftp_ctrl_process(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));



	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;

	u16	dst_tcp_data_len	= 0;	

	if(src_tcp_data_len)
	{
		u8 option_len		= 20;

		u8 *src_d		= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + src_ip_header_len + src_tcp_header_len]));
		u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
		
		u32	src_opcode	= (*src_d<<16) | ((*(src_d+1))<<8) | (*(src_d+2));
		u32	src_cmd		= (*src_d<<24) | ((*(src_d+1))<<16)| ((*(src_d+2))<<8) | (*(src_d+3));

		switch(src_opcode)
		{
			case SERVER_READY:
				if(!flg_ftp_user)
				{
					printk("Service ready for new user\n");
					flg_ftp_user	= TRUE;				
					sys_reset_cmdline();
				}				
				break;

			case PASSWORD_NEED:
				if(!flg_ftp_pass)
				{
					printk("Please specify the password\n");
					flg_ftp_pass	= TRUE;
					sys_reset_cmdline();
				}				
				break;

			case USER_LOGIN_OK:
				if(!flg_ftp_login)
				{
					printk("Login successful\n");
					flg_ftp_login	= TRUE;
					sys_reset_cmdline();
				}				
				break;

			case TYPE_I:
				if(!flg_ftp_type)
				{
					printk("200.Type I\n");
					flg_ftp_type	= TRUE;
					sys_reset_cmdline();
				}
				break;

			case SERVER_CLOSE:
				if(!flg_ftp_quit)
				{
					printk("Service closing control connection\n");
					flg_ftp_quit	= TRUE;
					sys_reset_cmdline();
				}				
				break;
			case FILE_STATE_OK:
				if(!flg_ftp_filests)
				{
					printk("150.Here comes the directory listing\n");
					flg_ftp_filests	= TRUE;
					
					ftp_control_seq		= little_big_32(tcph->ack);
					ftp_control_ack		= little_big_32(tcph->seq) + src_tcp_data_len;

					sys_reset_cmdline();
				}
				
				return;		//drop the packet

				break;
			default:
				break;
		}

		switch(src_cmd)
		{
			//-------------------------------------------------------------------------
			// FTP CONTROL CMD
			//-------------------------------------------------------------------------
			case USER:
				dst_tcp_data_len = ftp_cmd_user(dst_pkg);
				break;
			case PASS:
				dst_tcp_data_len = ftp_cmd_pass(dst_pkg);
				break;
			case SYST:
				dst_tcp_data_len = ftp_cmd_syst(dst_pkg);
				break;
			case QUIT:
				ftp_cmd_quit(src_pkg, dst_pkg);
				ftp_close(src_pkg, dst_pkg);

				return;

				break;
			//-------------------------------------------------------------------------
			// FTP DATA CMD
			//-------------------------------------------------------------------------
			

			default:
				break;
		}
	}

	tcp_process(src_pkg, dst_pkg, src_tcp_data_len, dst_tcp_data_len, 0);

	
	if((tcp_state == TCP_STATE5) && (!flg_ftp_user))
	{
		ftp_cmd_user_pre(src_pkg,dst_pkg);
	}

	if((flg_ftp_login) && (flg_ftp_type) && (!flg_ftp_port))
	{
		ftp_cmd_port(src_pkg,dst_pkg);

		printk("data port:%d\n",ftp_data_port);
		sys_reset_cmdline();
	}	
}


//===========================================================================
// ftp_data_process
//===========================================================================
PUBLIC	void 	ftp_data_process(struct s_package *src_pkg, struct s_package *dst_pkg)
{	
	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));



	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;
	u16	flag_offset		= (big_little_16(iph->flag_offset))>>13;	
	u16	dst_tcp_data_len	= 0;

	u8 option_len		= 0;
	
	u8 *src_d		= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + src_ip_header_len + src_tcp_header_len]));
	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));

	if(src_tcp_data_len)
	{
		u8 flg_data		= TRUE;				
		
		u32	src_opcode	= (*src_d<<16) | ((*(src_d+1))<<8) | (*(src_d+2));
		u32	src_cmd		= (*src_d<<24) | ((*(src_d+1))<<16)| ((*(src_d+2))<<8) | (*(src_d+3));

		switch(src_opcode)
		{
			case DIR_OK:
				printk("Directory send ok\n");

				flg_data	= FALSE;

				flg_ftp_ls	= FALSE;

				sys_reset_cmdline();
				break;
			default:
				break;
		}

		switch(src_cmd)
		{
			//-------------------------------------------------------------------------
			// FTP CONTROL CMD
			//-------------------------------------------------------------------------
			

			//-------------------------------------------------------------------------
			// FTP DATA CMD
			//-------------------------------------------------------------------------
			

			default:
				break;
		}		

		if((flg_data) && (flg_ftp_get))
		{			
			strcpy(recv_buf_start_addr, src_d, (src_tcp_data_len-2));	//exclude '\r\n'
			recv_buf_start_addr	+= (src_tcp_data_len-2);

			
			if((flag_offset & IP_FLAG_MAY_FRAGMENT) == FALSE)
			{			
				*(recv_buf_start_addr+1)= NULL;
				cat("/test/download.txt", (s8*)NET_RECV_BUF, FILE_MODE_W);
				recv_buf_start_addr	= NET_RECV_BUF;

				flg_ftp_get	= FALSE;
			}			
		}	
		
		if((flg_data) && (flg_ftp_ls))
		{
			strcpy(recv_buf_start_addr, src_d, (src_tcp_data_len-2));	//exclude '\r\n'
			recv_buf_start_addr	+= (src_tcp_data_len-2);

			*(recv_buf_start_addr+1)= NULL;

			printk("%s\n",(s8*)NET_RECV_BUF);

			recv_buf_start_addr	= NET_RECV_BUF;

			sys_reset_cmdline();
		}
		
	}

	if((tcp_state == TCP_STATE5) && (flg_ftp_put))
	{
		ftp_cmd_put(src_pkg,dst_pkg);
		
		return;
	}

	if((tcp_state == TCP_STATE5) && (data_size_total_bytes == (seq - connect_ack)) && (flg_ftp_data_start) && (!flg_ftp_data_end))
	{
		flg_ftp_close	= TRUE;		
	}

	if((tcp_state == TCP_STATE5) && (flg_ftp_close))
	{
		ftp_close(src_pkg, dst_pkg);
		return;
	}

	tcp_process(src_pkg, dst_pkg, src_tcp_data_len, dst_tcp_data_len, 0);	
	
	if(((tcp_state == TCP_STATE8) || (tcp_state == TCP_STATE_END)) && (flg_ftp_data_start) && (!flg_ftp_data_end))
	{
		flg_ftp_data_start	= FALSE;
		flg_ftp_data_end	= TRUE;

		ftp_parm_restore();
	}
}
