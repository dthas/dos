//===========================================================================
// telnet.c
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

PRIVATE	s32	telnet_state_tab[TELNET_NR_STATUS][TELNET_NR_ACTION];

PRIVATE	void	telnet_statetab_init();

PUBLIC	u8	telnet_stack[STACK_SIZE];

PRIVATE	s8	telnet_cmd[NR_TELNET_CMD][TELNET_CMD_SIZE]	= {"ls -l","quit", "pwd"};


struct iaddr 	telnet_dest_ip;
telnet_op	telnet_fun;
//===========================================================================
// telnet main
//===========================================================================

PUBLIC	void 	telnet_main()
{
	while(1)
	{
	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		struct iaddr ip;

		mesg.src_pid 	= PROC_TELNET;
		mesg.dest_pid 	= PROC_TELNET;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case TELNET_PROCESS:
					telnet_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case TELNET_OPEN:
					flg_telnet_client	= TRUE;
					s2ip(&ip, mesg.msg.buf1);
					telnet_open_sys(ip, &dst_pkg);
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
// init telnet
//===========================================================================
PUBLIC	void	init_telnet()
{
	p_telnet_stack 	= telnet_stack;

	telnet_statetab_init();

	telnet_state	= TELNET_STATE1;


	flg_telnet_client	= FALSE;

	p_telnet_cmd	= telnet_cmd;
	
//------for test
	char *str = "-init telnet-";
	disp_str(str);
//------for test
}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	telnet_statetab_init()
{
	s32 i,j;

	p_telnet_state_tab = telnet_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<TELNET_NR_STATUS; i++)
	{
		for(j=0; j<TELNET_NR_ACTION; j++)
		{
			telnet_state_tab[i][j]	= TELNET_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// TELNET_STATE1 
	//------------------------------------------------------------------
	
	//------------------------------------------------------------------
	// TELNET_STATE2 
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE2][SEND_USERTITLE]		= TELNET_STATE3;
	telnet_state_tab[TELNET_STATE2][RECV_USERTITLE]		= TELNET_STATE4;

	telnet_state_tab[TELNET_STATE2][SEND_CTRL]		= TELNET_STATE2;
	telnet_state_tab[TELNET_STATE2][RECV_CTRL]		= TELNET_STATE2;

	//------------------------------------------------------------------
	// TELNET_STATE3 
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE3][RECV_USERNAME]		= TELNET_STATE6;

	telnet_state_tab[TELNET_STATE3][SEND_CTRL]		= TELNET_STATE3;
	telnet_state_tab[TELNET_STATE3][RECV_CTRL]		= TELNET_STATE3;
	
	//------------------------------------------------------------------
	// TELNET_STATE4
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE4][SEND_USERNAME]		= TELNET_STATE5;

	telnet_state_tab[TELNET_STATE4][SEND_CTRL]		= TELNET_STATE4;
	telnet_state_tab[TELNET_STATE4][RECV_CTRL]		= TELNET_STATE4;

	//------------------------------------------------------------------
	// TELNET_STATE5
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE5][RECV_PASSTITLE]		= TELNET_STATE8;

	telnet_state_tab[TELNET_STATE5][SEND_CTRL]		= TELNET_STATE5;
	telnet_state_tab[TELNET_STATE5][RECV_CTRL]		= TELNET_STATE5;

	//------------------------------------------------------------------
	// TELNET_STATE6
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE6][SEND_PASSTITLE]		= TELNET_STATE7;

	telnet_state_tab[TELNET_STATE6][SEND_CTRL]		= TELNET_STATE6;
	telnet_state_tab[TELNET_STATE6][RECV_CTRL]		= TELNET_STATE6;
	
	//------------------------------------------------------------------
	// TELNET_STATE7
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE7][RECV_PASSWORD]		= TELNET_STATE9;

	telnet_state_tab[TELNET_STATE7][SEND_CTRL]		= TELNET_STATE7;
	telnet_state_tab[TELNET_STATE7][RECV_CTRL]		= TELNET_STATE7;

	//------------------------------------------------------------------
	// TELNET_STATE8
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE8][SEND_PASSWORD]		= TELNET_STATE9;

	telnet_state_tab[TELNET_STATE8][SEND_CTRL]		= TELNET_STATE8;
	telnet_state_tab[TELNET_STATE8][RECV_CTRL]		= TELNET_STATE8;

	//------------------------------------------------------------------
	// TELNET_STATE9
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE9][SEND_CMD]		= TELNET_STATE10;
	telnet_state_tab[TELNET_STATE9][RECV_CMD]		= TELNET_STATE11;

	telnet_state_tab[TELNET_STATE9][SEND_QUIT]		= TELNET_STATE12;
	telnet_state_tab[TELNET_STATE9][RECV_QUIT]		= TELNET_STATE12;

	telnet_state_tab[TELNET_STATE9][SEND_CTRL]		= TELNET_STATE9;
	telnet_state_tab[TELNET_STATE9][RECV_CTRL]		= TELNET_STATE9;

	//------------------------------------------------------------------
	// TELNET_STATE10
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE10][RECV_RES]		= TELNET_STATE9;

	telnet_state_tab[TELNET_STATE10][SEND_CTRL]		= TELNET_STATE10;
	telnet_state_tab[TELNET_STATE10][RECV_CTRL]		= TELNET_STATE10;

	//------------------------------------------------------------------
	// TELNET_STATE11
	//------------------------------------------------------------------
	telnet_state_tab[TELNET_STATE11][SEND_RES]		= TELNET_STATE9;

	telnet_state_tab[TELNET_STATE11][SEND_CTRL]		= TELNET_STATE11;
	telnet_state_tab[TELNET_STATE11][RECV_CTRL]		= TELNET_STATE11;

	//------------------------------------------------------------------
	// TELNET_STATE12
	//------------------------------------------------------------------	
}


//===========================================================================
// telnet_process
//===========================================================================
PUBLIC	void 	telnet_process(struct s_package *src_pkg, struct s_package *dst_pkg)
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
		s8 t_cmd[TELNET_CMD_SIZE];

		s8 flg_response = FALSE;
		s8 flg_tmp	= FALSE;
		s8 flg_ctrl	= FALSE;

		
		s32 i,j, cmd_id;
		u8 src_action;


		if(flg_telnet_client)
		{
			flg_response	= TRUE;			
		}
		else
		{
			flg_response	= FALSE;
		}

		if(flg_response)
		{
		//##################################################################
		// RECEIVE RESPONSE (client)
		//##################################################################
			//------------------------------------------------------------------
			// 1)get telnet cmd
			//------------------------------------------------------------------
			if(*src_d == 0xFF)
			{
				flg_ctrl	= TRUE;
			}
			else
			{
				flg_ctrl	= FALSE;				
			}

			if(flg_ctrl)
			{
				telnet_fun	= telnet_res_ctrl;
				src_action	= RECV_CTRL;
			}
			else
			{
				switch(telnet_state)
				{
					case TELNET_STATE2:
						telnet_fun	= telnet_send_username;
						src_action	= RECV_USERTITLE;
						break;
					case TELNET_STATE5:
						telnet_fun	= telnet_send_password;
						src_action	= RECV_PASSTITLE;
						break;
					case TELNET_STATE9:
						telnet_fun	= telnet_send_ls;	//for test;
						src_action	= SEND_CMD;
						break;
					case TELNET_STATE10:
						telnet_fun	= telnet_send_quit;	//for test
						src_action	= RECV_RES;
						break;
					default:
						break;
				}
				
			}

			//-------------------------------------------------------------------------
			// telnet state change(change to TELNET_STATE3/TELNET_STATE4::WAIT_C/WAIT_S)
			//-------------------------------------------------------------------------
			telnet_state	= *(p_telnet_state_tab + telnet_state * TELNET_NR_ACTION + src_action);

			telnet_fun(telnet_dest_ip, src_pkg, dst_pkg);

			return;
			
		}
		else
		{
		//##################################################################
		// RECEIVE REQUEST (server)
		//##################################################################

			i = 0;

			//------------------------------------------------------------------
			// 1)get telnet cmd
			//------------------------------------------------------------------
			if(*src_d == 0xFF)
			{
				flg_ctrl	= TRUE;
			}
			else
			{
				flg_ctrl	= FALSE;

				//------------------------------------------------------------------
				//1)get cmd 
				//------------------------------------------------------------------
				for(j=0; i<512; i++,j++)
				{
					if(*(src_d+i) == 0x0d)	// space or \r
					{
						break;
					}
					else
					{
						t_cmd[j]	= *(src_d + i);
					}
				}				
				t_cmd[j]	= NULL;

				//------------------------------------------------------------------
				//5)get sts_code
				//------------------------------------------------------------------
				if(telnet_state == TELNET_STATE9)
				{
					for(i=0; i<NR_TELNET_CMD; i++)
					{
						if(flg_tmp = strcmp(t_cmd, telnet_cmd[i]))
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
				}
				else
				{
					//ignore for username and password				
				}

				//------------------------------------------------------------------
				//5)get sts_code
				//------------------------------------------------------------------
				cmd_id	= i;
			}

			if(flg_ctrl)
			{
				telnet_fun	= telnet_res_ctrl;
				src_action	= RECV_CTRL;
			}
			else
			{
				switch(telnet_state)
				{
					case TELNET_STATE3:
						telnet_fun	= telnet_send_passtitle;
						src_action	= RECV_USERNAME;
						break;
					case TELNET_STATE7:
						telnet_fun	= telnet_send_logintitle;
						src_action	= RECV_PASSWORD;
						break;
					case TELNET_STATE9:
						switch(cmd_id)
						{
							case TELNET_CMD_LS:
								telnet_fun	= telnet_res_ls;
								src_action	= RECV_CMD;
								break;
							case TELNET_CMD_QUIT:
								telnet_fun	= telnet_res_quit;
								src_action	= RECV_CMD;
								break;
							default:
								break;
						}						
						break;
					default:
						break;
				}
				
			}

			//-------------------------------------------------------------------------
			// telnet state change(change to TELNET_STATE3/TELNET_STATE4::WAIT_C/WAIT_S)
			//-------------------------------------------------------------------------
			telnet_state	= *(p_telnet_state_tab + telnet_state * TELNET_NR_ACTION + src_action);

			if(telnet_fun)
			{
				telnet_fun(telnet_dest_ip, src_pkg, dst_pkg);				
			}
			
			if(telnet_state == TELNET_STATE2)
			{
				telnet_send_usertitle(telnet_dest_ip, src_pkg, dst_pkg);
			}

			return;
		}			
	}	

	tcp_process(src_pkg, dst_pkg, src_tcp_data_len, dst_tcp_data_len, 0);

	if((telnet_state == TELNET_STATE1) && (tcp_state == TCP_STATE5))
	{
		telnet_state = TELNET_STATE2;
	}

	if((telnet_state == TELNET_STATE2) && (flg_telnet_client == TRUE))
	{
		telnet_send_ctrl(telnet_dest_ip, src_pkg, dst_pkg);
	}
}
