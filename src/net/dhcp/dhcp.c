//===========================================================================
// dhcp.c
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
#include	"udp.h"
#include	"dhcp.h"
#include	"prototype.h"

PRIVATE	s32	dhcp_state_tab[DHCP_NR_STATUS][DHCP_NR_ACTION];

PRIVATE	void	dhcp_statetab_init();

PUBLIC	u8	dhcp_stack[STACK_SIZE];

//===========================================================================
// tftp main
//===========================================================================

PUBLIC	void 	dhcp_main()
{
	while(1)
	{
	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		struct iaddr ip;

		mesg.src_pid 	= PROC_DHCP;
		mesg.dest_pid 	= PROC_DHCP;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case DHCP_PROCESS:
					dhcp_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case DHCP_DISCOVER:
					s2ip(&ip, mesg.msg.buf1);
					dhcp_send_discover(ip, &dst_pkg);
					break;
				case DHCP_REQUEST:
					s2ip(&ip, mesg.msg.buf1);
					dhcp_send_request(ip, &dst_pkg, req_ip);
					break;	
				case DHCP_RELEASE:
					s2ip(&ip, mesg.msg.buf1);
					dhcp_send_release(ip, &dst_pkg);
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
// init dhcp
//===========================================================================
PUBLIC	void	init_dhcp()
{
	p_ddhcp_stack 	= dhcp_stack;

	dhcp_statetab_init();

	dhcp_state	= DHCP_STATE1;

	g_trans_id	= 0;

//------for test------------
	iden_server_ip.addr1	= 192;
	iden_server_ip.addr2	= 168;
	iden_server_ip.addr3	= 1;
	iden_server_ip.addr4	= 1;	

	req_ip.addr1		= 192;
	req_ip.addr2		= 168;
	req_ip.addr3		= 1;
	req_ip.addr4		= 140;	
//------for test------------
	
//------for test
	char *str = "-init dhcp-";
	disp_str(str);
//------for test
}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	dhcp_statetab_init()
{
	s32 i,j;

	p_dhcp_state_tab = dhcp_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<DHCP_NR_STATUS; i++)
	{
		for(j=0; j<DHCP_NR_ACTION; j++)
		{
			dhcp_state_tab[i][j]	= DHCP_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// DHCP_STATE1 
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE1][SEND_DHCPDISCOVER]	= DHCP_STATE2;
	dhcp_state_tab[DHCP_STATE1][RECV_DHCPDISCOVER]	= DHCP_STATE9;

	//------------------------------------------------------------------
	// DHCP_STATE2 
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE2][RECV_DHCPOFFER]	= DHCP_STATE3;

	//------------------------------------------------------------------
	// DHCP_STATE3 
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE3][SEND_DHCPREQUEST]	= DHCP_STATE5;

	//------------------------------------------------------------------
	// DHCP_STATE4
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE4][RECV_DHCPREQUEST]	= DHCP_STATE5;

	//------------------------------------------------------------------
	// DHCP_STATE5 
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE5][SEND_DHCPACK]	= DHCP_STATE6;
	dhcp_state_tab[DHCP_STATE5][RECV_DHCPACK]	= DHCP_STATE6;

	//------------------------------------------------------------------
	// DHCP_STATE6 
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE6][SEND_DHCPREQUEST]	= DHCP_STATE7;
	dhcp_state_tab[DHCP_STATE6][RECV_DHCPREQUEST]	= DHCP_STATE7;

	dhcp_state_tab[DHCP_STATE6][EX_TIME]		= DHCP_STATE8;

	dhcp_state_tab[DHCP_STATE6][SEND_DHCPRELEASE]	= DHCP_STATE1;

	//------------------------------------------------------------------
	// DHCP_STATE7 
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE7][SEND_DHCPACK]	= DHCP_STATE6;
	dhcp_state_tab[DHCP_STATE7][RECV_DHCPACK]	= DHCP_STATE6;
	dhcp_state_tab[DHCP_STATE7][EX_TIME]		= DHCP_STATE8;

	//------------------------------------------------------------------
	// DHCP_STATE8 
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE8][SEND_DHCPNACK]	= DHCP_STATE1;
	dhcp_state_tab[DHCP_STATE8][RECV_DHCPNACK]	= DHCP_STATE1;

	dhcp_state_tab[DHCP_STATE8][SEND_DHCPACK]	= DHCP_STATE6;

	//------------------------------------------------------------------
	// DHCP_STATE9
	//------------------------------------------------------------------
	dhcp_state_tab[DHCP_STATE9][SEND_DHCPOFFER]	= DHCP_STATE4;
}


//===========================================================================
// dhcp_process
//===========================================================================
PUBLIC	void 	dhcp_process(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_ipv4_header* iph	= (struct s_ipv4_header*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH]));

	struct s_dhcp_header* dhcp_src	= (struct s_dhcp_header*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));
	struct s_dhcp_option* dhcp_op	= (struct s_dhcp_header*)(&(src_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH + DHCP_HEADER_LENGTH + 4]));	//magic_code(4)

	u8 src_action		= -1;
	u8 dst_action		= -1;

	u8	src_option	= dhcp_op->val;

	switch(src_option)
	{
		//------------------------------------------------------------------
		// server
		//------------------------------------------------------------------
		case	DHCPDISCOVER:
			src_action	= RECV_DHCPDISCOVER;
			dst_action	= SEND_DHCPOFFER;
			break;
		case	DHCPREQUEST:
			src_action	= RECV_DHCPREQUEST;
			dst_action	= SEND_DHCPACK;
			break;
		//------------------------------------------------------------------
		// client
		//------------------------------------------------------------------
		case	DHCPOFFER:
			src_action	= RECV_DHCPOFFER;
			dst_action	= SEND_DHCPREQUEST;

			req_ip		= dhcp_src->my_ip;
			break;
		case	DHCPACK:
			src_action	= RECV_DHCPACK;			
			dst_action	= -1;
			break;
		default:
			break;
	}
	
	if(src_action)
	{		
		s32 old_state	= dhcp_state;
		dhcp_state	= *(p_dhcp_state_tab + dhcp_state * DHCP_NR_ACTION + src_action);		
	}

	if(dhcp_state && dst_action)
	{
		//-------------------------------------------------------------------------
		// add dhcp option
		//-------------------------------------------------------------------------
		u16 option_len	= 0;

		//-------------------------------------------------------------------------
		// dhcp
		//-------------------------------------------------------------------------
		u16 dhcp_data_len = 0;

		struct iaddr res_ip;

		res_ip.addr1	= 192;
		res_ip.addr2	= 168;
		res_ip.addr3	= 1;
		res_ip.addr4	= 130;
		
		struct iaddr dst_ip;
		dst_ip.addr1	= 255;
		dst_ip.addr2	= 255;
		dst_ip.addr3	= 255;
		dst_ip.addr4	= 255;
	
		
		switch(dst_action)
		{
			//------------------------------------------------------------------
			// server
			//------------------------------------------------------------------
			case	SEND_DHCPOFFER:				
				dhcp_send_offer(src_pkg, dst_pkg, res_ip);
				break;			
			case	SEND_DHCPACK:				
				dhcp_send_ack(src_pkg, dst_pkg, res_ip);
				break;
			//------------------------------------------------------------------
			// client
			//------------------------------------------------------------------
			case	SEND_DHCPREQUEST:
				dhcp_send_request(dst_ip, dst_pkg, req_ip);
				break;
			default:
				break;
		}

	}
}
