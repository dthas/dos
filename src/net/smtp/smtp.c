//===========================================================================
// smtp.c
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

PRIVATE	s32	smtp_state_tab[SMTP_NR_STATUS][SMTP_NR_ACTION];

PRIVATE	void	smtp_statetab_init();

PUBLIC	u8	smtp_stack[STACK_SIZE];

PRIVATE	s8 	mailname[64]	= "/test/mail.txt";

struct iaddr 	smtp_dest_ip;
smtp_op	smtp_fun;
//===========================================================================
// tftp main
//===========================================================================

PUBLIC	void 	smtp_main()
{
//---------test---------------
	mkfile(mailname);
//---------test---------------

	while(1)
	{
	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		struct iaddr ip;

		mesg.src_pid 	= PROC_SMTP;
		mesg.dest_pid 	= PROC_SMTP;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case SMTP_PROCESS:
					smtp_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case SMTP_OPEN:
					flg_client	= TRUE;

					s2ip(&ip, mesg.msg.buf1);
					smtp_open_sys(ip, &dst_pkg);
					break;
				case SMTP_SEND:
					flg_client	= TRUE;

					s2ip(&ip, mesg.msg.buf1);			
					
					smtp_open_sys(ip, &dst_pkg);
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
// init smtp
//===========================================================================
PUBLIC	void	init_smtp()
{
	p_smtp_stack 	= smtp_stack;

	smtp_statetab_init();

	smtp_state	= SMTP_STATE1;

	p_recv_nbuf	= NET_RECV_BUF;

	p_mailname	= mailname;

	smtp_parm_init();
		
//------for test
	char *str = "-init smtp-";
	disp_str(str);
//------for test
}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	smtp_statetab_init()
{
	s32 i,j;

	p_smtp_state_tab = smtp_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<SMTP_NR_STATUS; i++)
	{
		for(j=0; j<SMTP_NR_ACTION; j++)
		{
			smtp_state_tab[i][j]	= SMTP_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// SMTP_STATE1 
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	// SMTP_STATE2 
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE2][SEND_SERVERREADY]		= SMTP_STATE3;
	smtp_state_tab[SMTP_STATE2][RECV_SERVERREADY]		= SMTP_STATE3;

	//------------------------------------------------------------------
	// SMTP_STATE3 
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE3][SEND_HELO]			= SMTP_STATE4;
	smtp_state_tab[SMTP_STATE3][RECV_HELO]			= SMTP_STATE5;

	//------------------------------------------------------------------
	// SMTP_STATE4
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE4][RECV_OK]			= SMTP_STATE6;

	//------------------------------------------------------------------
	// SMTP_STATE5
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE5][SEND_OK]			= SMTP_STATE6;

	//------------------------------------------------------------------
	// SMTP_STATE6
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE6][SEND_MAILFROM]		= SMTP_STATE7;
	smtp_state_tab[SMTP_STATE6][RECV_MAILFROM]		= SMTP_STATE8;

	//------------------------------------------------------------------
	// SMTP_STATE7
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE7][RECV_OK]			= SMTP_STATE9;

	//------------------------------------------------------------------
	// SMTP_STATE8
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE8][SEND_OK]			= SMTP_STATE10;

	//------------------------------------------------------------------
	// SMTP_STATE9
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE9][SEND_RCPTTO]		= SMTP_STATE11;

	//------------------------------------------------------------------
	// SMTP_STATE10
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE10][RECV_RCPTTO]		= SMTP_STATE12;

	//------------------------------------------------------------------
	// SMTP_STATE11
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE11][RECV_OK]			= SMTP_STATE13;

	//------------------------------------------------------------------
	// SMTP_STATE12
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE12][SEND_OK]			= SMTP_STATE14;

	//------------------------------------------------------------------
	// SMTP_STATE13
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE13][SEND_DATA]			= SMTP_STATE15;

	//------------------------------------------------------------------
	// SMTP_STATE14
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE14][RECV_DATA]			= SMTP_STATE16;

	//------------------------------------------------------------------
	// SMTP_STATE15
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE15][RECV_STARTMAILREADY]	= SMTP_STATE17;

	//------------------------------------------------------------------
	// SMTP_STATE16
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE16][SEND_STARTMAILREADY]	= SMTP_STATE18;

	//------------------------------------------------------------------
	// SMTP_STATE17
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE17][RECV_OK]			= SMTP_STATE19;

	//------------------------------------------------------------------
	// SMTP_STATE18
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE18][RECV_DOT]			= SMTP_STATE20;

	//------------------------------------------------------------------
	// SMTP_STATE19
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE19][SEND_QUIT]			= SMTP_STATE21;

	//------------------------------------------------------------------
	// SMTP_STATE20
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE20][SEND_OK]			= SMTP_STATE22;

	//------------------------------------------------------------------
	// SMTP_STATE21
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE21][RECV_CLOSE]		= SMTP_STATE24;

	//------------------------------------------------------------------
	// SMTP_STATE22
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE22][RECV_QUIT]			= SMTP_STATE23;

	//------------------------------------------------------------------
	// SMTP_STATE23
	//------------------------------------------------------------------
	smtp_state_tab[SMTP_STATE23][SEND_CLOSE]		= SMTP_STATE24;	
}

//===========================================================================
// smtp_process
//===========================================================================
PUBLIC	void 	smtp_process(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header *iph 	= (struct s_ipv4_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));
	struct s_tcp_header *tcph 	= (struct s_tcp_header *)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH]));

	u8	src_ip_header_len	= ((iph->version_len) & 0xF) * 4;
	u16	src_ip_total_len	= little_big_16(iph->length);
	u8	src_tcp_header_len	= ((tcph->header_len)>>4) * 4;
	u16	src_tcp_data_len	= src_ip_total_len - src_ip_header_len - src_tcp_header_len;
		
	u16	dst_tcp_data_len	= 0;

	u8 option_len		= 0;
	
	u8 *src_d		= (u8*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + src_ip_header_len + src_tcp_header_len]));
	u8 *dst_d		= (u8*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + TCP_HEADER_LENGTH + option_len]));
	
	if(src_tcp_data_len)
	{
		s8 t_state_code[4];
		s8 t_cmd[10];

		s8 flg_response = FALSE;
		s8 flg_tmp	= FALSE;

		
		s32 i,j,sts_code, cmd_id;
		u8 src_action;


		if((*src_d >= '0') && (*src_d <= '9'))
		{
			flg_response	= TRUE;
			flg_client	= TRUE;
		}
		else
		{
			flg_client	= FALSE;
		}

		if(flg_response)
		{
		//##################################################################
		// RECEIVE RESPONSE (client)
		//##################################################################
			i = 0;

			//------------------------------------------------------------------
			//1)get state 
			//------------------------------------------------------------------
			for(j=0; i<512; i++,j++)
			{				
				if((*(src_d+i) == 0x20) || (*(src_d+i) == 0x2d))	//space or -
				{
					break;
				}
				else
				{
					t_state_code[j]	= *(src_d + i);
				}
			}
			
			t_state_code[j]	= NULL;
			
			//------------------------------------------------------------------
			//5)get sts_code
			//------------------------------------------------------------------
			for(i=0; i<NR_SMTP_STATE; i++)
			{				
				if(flg_tmp = strcmp(t_state_code, (p_ssmtp_state + i*SMTP_STATE_SIZE)))
				{					
					break;
				}
			}
			
			if(!flg_tmp)
			{
				printk("panic(C)::state code:%s\n",t_state_code);

				while(1)
				{};
			}

			sts_code	= i;

			//------------------------------------------------------------------
			//6)action
			//------------------------------------------------------------------
			switch(sts_code)
			{
				case STS_SERVICE_READY:
					src_action	= RECV_SERVERREADY;	// recv 220
					smtp_fun	= smtp_send_ehlo;
					break;
				case STS_CMD_OK:
					src_action	= RECV_OK;		// recv 250

					switch(smtp_state)
					{
						case SMTP_STATE4:
							smtp_fun	= smtp_send_mailfrom;
							break;
						case SMTP_STATE7:
							smtp_fun	= smtp_send_rcptto;
							break;
						case SMTP_STATE11:
							smtp_fun	= smtp_send_data;
							break;
						case SMTP_STATE17:
							smtp_fun	= smtp_send_quit;
							break;
						default:
							break;
					}

					break;
				case STS_SERVICE_CLOSE_TRANS:
					src_action	= RECV_CLOSE;		// recv 221
					break;
				case STS_START_MAIL_INPUT:
					src_action	= RECV_STARTMAILREADY;	// recv 354
					smtp_fun	= smtp_send_mail;
					break;
				default:
					break;
			}

			//-------------------------------------------------------------------------
			// smtp state change(change to SMTP_STATE3/SMTP_STATE4::WAIT_C/WAIT_S)
			//-------------------------------------------------------------------------
			smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);

			if(smtp_state != SMTP_STATE24)
			{
				smtp_fun(smtp_dest_ip, src_pkg, dst_pkg);
				return ;
			}			
		}
		else
		{
		//##################################################################
		// RECEIVE REQUEST (server)
		//##################################################################

			i = 0;

			//------------------------------------------------------------------
			// 1)get smtp cmd
			//------------------------------------------------------------------
			for(j=0; i<512; i++,j++)
			{				
				if((*(src_d+i) == 0x20) || (*(src_d+i) == 0x2d) || (*(src_d+i) == 0x0d))	//space or - or /r
				{
					break;
				}
				else
				{
					t_cmd[j]	= *(src_d + i);
				}
			}

			t_cmd[j]  = NULL;
			
			//------------------------------------------------------------------
			//5)get cmd
			//------------------------------------------------------------------
			for(i=0; i<NR_CMD; i++)
			{
				if(flg_tmp = strcmp(t_cmd, (p_server_smtp_cmd + i*CMD_SIZE)))
				{					
					break;
				}
			}
			
			if(!flg_tmp)
			{
				printk("panic(S)::cmd:%s\n",t_cmd);

				while(1)
				{};
			}

			cmd_id	= i;

			//------------------------------------------------------------------
			//6)action
			//------------------------------------------------------------------
			switch(cmd_id)
			{
				case CMD_EHLO:
					smtp_fun	= smtp_send_ok;
					src_action	= RECV_HELO;
					break;
				case CMD_MAILFROM:
					smtp_fun	= smtp_send_ok;
					src_action	= RECV_MAILFROM;
					break;
				case CMD_RCPTTO:
					smtp_fun	= smtp_send_ok;
					src_action	= RECV_RCPTTO;
					break;
				case CMD_DATA:
					smtp_fun	= smtp_send_startmail;
					src_action	= RECV_DATA;
					break;
				case CMD_RECEIVEMAIL:
					mail_process(src_pkg);

					smtp_fun	= smtp_send_ok;
					src_action	= RECV_DOT;
					break;
				case CMD_QUIT:
					smtp_fun	= smtp_send_close;
					src_action	= RECV_QUIT;
					break;
				default:
					break;
			}

			//-------------------------------------------------------------------------
			// smtp state change(change to SMTP_STATE3/SMTP_STATE4::WAIT_C/WAIT_S)
			//-------------------------------------------------------------------------
			smtp_state	= *(p_smtp_state_tab + smtp_state * SMTP_NR_ACTION + src_action);

			smtp_fun(smtp_dest_ip, src_pkg, dst_pkg);

			return;
		}			
	}

	tcp_process(src_pkg, dst_pkg, src_tcp_data_len, dst_tcp_data_len, 0);

	if((smtp_state == SMTP_STATE1) && (tcp_state == TCP_STATE5))
	{
		smtp_state	= SMTP_STATE2;		
	}
	
	if((smtp_state == SMTP_STATE2) && (flg_client == FALSE))
	{
		smtp_send_ready(smtp_dest_ip, src_pkg, dst_pkg);	
		return;
	}
}
