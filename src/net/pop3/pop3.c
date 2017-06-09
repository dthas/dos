//===========================================================================
// pop3.c
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

PRIVATE	s32	pop3_state_tab[POP3_NR_STATUS][POP3_NR_ACTION];

PRIVATE	void	pop3_statetab_init();

PUBLIC	u8	pop3_stack[STACK_SIZE];


struct iaddr 	pop3_dest_ip;
pop3_op	pop3_fun;
//===========================================================================
// pop3 main
//===========================================================================

PUBLIC	void 	pop3_main()
{
	while(1)
	{
	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		struct iaddr ip;

		mesg.src_pid 	= PROC_POP3;
		mesg.dest_pid 	= PROC_POP3;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case POP3_PROCESS:
					pop3_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case POP3_OPEN:
					flg_pop3_client	= TRUE;
					s2ip(&ip, mesg.msg.buf1);
					pop3_open_sys(ip, &dst_pkg);
					break;
				case POP3_USER:					
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
// init pop3
//===========================================================================
PUBLIC	void	init_pop3()
{
	p_pop3_stack 	= pop3_stack;

	pop3_statetab_init();

	pop3_state	= POP3_STATE1;

	pop3_parm_init();
			
//------for test
	char *str = "-init pop3-";
	disp_str(str);
//------for test
}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	pop3_statetab_init()
{
	s32 i,j;

	p_pop3_state_tab = pop3_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<POP3_NR_STATUS; i++)
	{
		for(j=0; j<POP3_NR_ACTION; j++)
		{
			pop3_state_tab[i][j]	= POP3_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// POP3_STATE1 
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE1][RECV_CMD_OK]		= POP3_STATE2;
	pop3_state_tab[POP3_STATE1][SEND_CMD_OK]		= POP3_STATE2;

	//------------------------------------------------------------------
	// POP3_STATE2 
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE2][SEND_USERNAME]		= POP3_STATE3;
	pop3_state_tab[POP3_STATE2][RECV_USERNAME]		= POP3_STATE4;

	//------------------------------------------------------------------
	// POP3_STATE3 
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE3][RECV_CMD_OK]		= POP3_STATE5;
	
	//------------------------------------------------------------------
	// POP3_STATE4
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE4][SEND_CMD_OK]		= POP3_STATE6;

	//------------------------------------------------------------------
	// POP3_STATE5
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE5][SEND_PASSWORD]		= POP3_STATE7;

	//------------------------------------------------------------------
	// POP3_STATE6
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE6][RECV_PASSWORD]		= POP3_STATE8;
	
	//------------------------------------------------------------------
	// POP3_STATE7
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE7][RECV_CMD_OK]		= POP3_STATE9;

	//------------------------------------------------------------------
	// POP3_STATE8
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE8][SEND_CMD_OK]		= POP3_STATE10;

	//------------------------------------------------------------------
	// POP3_STATE9
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE9][SEND_MAILINFORM]		= POP3_STATE11;

	//------------------------------------------------------------------
	// POP3_STATE10
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE10][RECV_MAILINFORM]		= POP3_STATE12;

	//------------------------------------------------------------------
	// POP3_STATE11
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE11][RECV_CMD_OK]		= POP3_STATE13;

	//------------------------------------------------------------------
	// POP3_STATE12
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE12][SEND_CMD_OK]		= POP3_STATE14;

	//------------------------------------------------------------------
	// POP3_STATE13
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE13][SEND_MAILLIST]		= POP3_STATE15;

	//------------------------------------------------------------------
	// POP3_STATE14
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE14][RECV_MAILLIST]		= POP3_STATE16;

	//------------------------------------------------------------------
	// POP3_STATE15
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE15][RECV_CMD_OK]		= POP3_STATE17;

	//------------------------------------------------------------------
	// POP3_STATE16
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE16][SEND_CMD_OK]		= POP3_STATE18;

	//------------------------------------------------------------------
	// POP3_STATE17
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE17][SEND_RETR]			= POP3_STATE19;

	//------------------------------------------------------------------
	// POP3_STATE18
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE18][RECV_RETR]			= POP3_STATE20;

	//------------------------------------------------------------------
	// POP3_STATE19
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE19][RECV_CMD_OK]		= POP3_STATE21;

	//------------------------------------------------------------------
	// POP3_STATE20
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE20][SEND_CMD_OK]		= POP3_STATE22;

	//------------------------------------------------------------------
	// POP3_STATE21
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE21][SEND_QUIT]			= POP3_STATE23;

	//------------------------------------------------------------------
	// POP3_STATE22
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE22][RECV_QUIT]			= POP3_STATE24;

	//------------------------------------------------------------------
	// POP3_STATE23
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE23][RECV_CMD_OK]		= POP3_STATE25;

	//------------------------------------------------------------------
	// POP3_STATE24
	//------------------------------------------------------------------
	pop3_state_tab[POP3_STATE24][SEND_CMD_OK]		= POP3_STATE25;		
}


//===========================================================================
// pop3_process
//===========================================================================
PUBLIC	void 	pop3_process(struct s_package *src_pkg, struct s_package *dst_pkg)
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
		s8 t_state_code[POP3_STATE_SIZE];
		s8 t_cmd[POP3_CMD_SIZE];

		s8 flg_response = FALSE;
		s8 flg_tmp	= FALSE;

		
		s32 i,j,sts_code, cmd_id;
		u8 src_action;


		if((*src_d == '+') || (*src_d == '-'))
		{
			flg_response	= TRUE;
			flg_pop3_client	= TRUE;
		}
		else
		{
			flg_response	= FALSE;
			flg_pop3_client	= FALSE;
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
				if((*(src_d+i) == 0x20) || (*(src_d+i) == 0x0d))	// space or \r
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
			for(i=0; i<NR_POP3_STATE; i++)
			{
				if(flg_tmp = strcmp(t_state_code, (p_ppop3_state+i*POP3_STATE_SIZE)))
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
				case STS_P_OK:
					src_action	= RECV_CMD_OK;		// 250

					switch(pop3_state)
					{
						case POP3_STATE1:
							pop3_fun	= pop3_send_user;
							break;
						case POP3_STATE3:
							pop3_fun	= pop3_send_pass;
							break;
						case POP3_STATE7:
							pop3_fun	= pop3_send_mailinform;
							break;
						case POP3_STATE11:
							pop3_fun	= pop3_send_maillist;
							break;
						case POP3_STATE15:
							pop3_fun	= pop3_send_retr;
							break;
						case POP3_STATE19:
							pop3_fun	= pop3_send_quit;
							break;
						case POP3_STATE23:							
							break;
						default:
							break;
					}

					break;
				case STS_N_ERR:
					src_action	= EX_TIME;		// 221
					break;
				default:
					break;
			}

			//-------------------------------------------------------------------------
			// pop3 state change(change to POP3_STATE3/POP3_STATE4::WAIT_C/WAIT_S)
			//-------------------------------------------------------------------------
			pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);
			
			if(pop3_state != POP3_STATE25)
			{
				pop3_fun(pop3_dest_ip, src_pkg, dst_pkg);
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
			// 1)get pop3 cmd
			//------------------------------------------------------------------
			for(j=0; i<512; i++,j++)
			{
				if((*(src_d+i) == 0x20) || (*(src_d+i) == 0x0d))	// space or \r
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
			for(i=0; i<NR_POP3_CMD; i++)
			{
				if(flg_tmp = strcmp(t_cmd, (p_pop3_cmd + i*POP3_CMD_SIZE)))
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
				case CMD_USER:
					src_action	= RECV_USERNAME;
					pop3_fun	= pop3_send_user_ok;
					break;
				case CMD_PASS:
					src_action	= RECV_PASSWORD;
					pop3_fun	= pop3_send_pass_ok;					
					break;
				case CMD_STAT:
					src_action	= RECV_MAILINFORM;
					pop3_fun	= pop3_send_mailinform_ok;					
					break;
				case CMD_LIST:
					src_action	= RECV_MAILLIST;
					pop3_fun	= pop3_send_maillist_ok;					
					break;
				case CMD_RETR:
					src_action	= RECV_RETR;
					pop3_fun	= pop3_send_mail;					
					break;
				case CMD_QUIT:
					src_action	= RECV_QUIT;
					pop3_fun	= pop3_send_quit_ok;					
					break;
				default:
					break;
			}

			//-------------------------------------------------------------------------
			// pop3 state change(change to POP3_STATE3/POP3_STATE4::WAIT_C/WAIT_S)
			//-------------------------------------------------------------------------
			pop3_state	= *(p_pop3_state_tab + pop3_state * POP3_NR_ACTION + src_action);

			pop3_fun(pop3_dest_ip, src_pkg, dst_pkg);

			return;
		}			
	}


	tcp_process(src_pkg, dst_pkg, src_tcp_data_len, dst_tcp_data_len, 0);

	if((pop3_state == POP3_STATE1) && (tcp_state == TCP_STATE5) && (flg_pop3_client == FALSE))
	{
		pop3_send_ready(pop3_dest_ip, src_pkg, dst_pkg);

		return;
	}
}
