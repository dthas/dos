//===========================================================================
// dns.c
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
#include	"dns.h"
#include	"prototype.h"

PRIVATE	s32	dns_state_tab[DNS_NR_STATUS][DNS_NR_ACTION];

PRIVATE	void	dns_statetab_init();

PUBLIC	u8	dns_stack[STACK_SIZE];

s8 domain_name[64]		= "www.gmail.com";
s8 domain_name_format[64]	= "\003www\005gmail\003com\000";
s8 ip_format[64]		= "\00255\00256\0014\00264\007in-addr\004arpa\000";

//===========================================================================
// tftp main
//===========================================================================
PUBLIC	void 	dns_main()
{
	while(1)
	{
	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		struct iaddr ip;

		mesg.src_pid 	= PROC_DNS;
		mesg.dest_pid 	= PROC_DNS;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case DNS_PROCESS:
					dns_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case DNS_IP_2_NAME:
					s2ip(&ip, mesg.msg.buf1);
					dns_send_ip(ip, &dst_pkg, pnet_dev->ip);
					break;	
				case DNS_NAME_2_IP:
					s2ip(&ip, mesg.msg.buf1);
					dns_send_name(ip, &dst_pkg, p_domain_name_format);
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
// init dns
//===========================================================================
PUBLIC	void	init_dns()
{
	p_dns_stack 	= dns_stack;


	dns_statetab_init();

	dns_state	= DNS_STATE1;

	p_domain_name			= domain_name;
	p_domain_name_format		= domain_name_format;
	p_ip_format			= ip_format;


//------for test
	char *str = "--dns init--";
	disp_str(str);
//------for test
}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	dns_statetab_init()
{
	s32 i,j;

	p_dns_state_tab = dns_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<DNS_NR_STATUS; i++)
	{
		for(j=0; j<DNS_NR_ACTION; j++)
		{
			dns_state_tab[i][j]	= DNS_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// DNS_STATE1 
	//------------------------------------------------------------------
	dns_state_tab[DNS_STATE1][SEND_IP]	= DNS_STATE2;
	dns_state_tab[DNS_STATE1][SEND_NAME]	= DNS_STATE2;

	dns_state_tab[DNS_STATE1][RECV_IP]	= DNS_STATE3;
	dns_state_tab[DNS_STATE1][RECV_NAME]	= DNS_STATE3;
	
	//------------------------------------------------------------------
	// DNS_STATE2 
	//------------------------------------------------------------------
	dns_state_tab[DNS_STATE2][RECV_IP]	= DNS_STATE1;
	dns_state_tab[DNS_STATE2][RECV_NAME]	= DNS_STATE1;

	//------------------------------------------------------------------
	// DNS_STATE3 
	//------------------------------------------------------------------
	dns_state_tab[DNS_STATE3][RECV_IP]	= DNS_STATE1;
	dns_state_tab[DNS_STATE3][SEND_NAME]	= DNS_STATE1;
}


//===========================================================================
// dns_process
//===========================================================================
PUBLIC	void 	dns_process(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	struct s_dns_header* dns_h	= (struct s_dns_header*)(&(dst_pkg->buffer[FRAME_HEADER_LENGTH + IPV4_HEADER_LENGTH + UDP_HEADER_LENGTH]));
	u8 src_action		= -1;
	u8 dst_action		= -1;

	u8	src_optcode	= ((little_big_16(dns_h->flag)) & OPCODE)>>11;

	switch(src_optcode)
	{
		case	OP_STANDER:
			src_action	= RECV_NAME;
			dst_action	= SEND_IP;
			break;
		case	OP_REVERSE:
			src_action	= RECV_IP;
			dst_action	= SEND_NAME;
			break;
		case	OP_STATUS:			
			break;
		default:
			break;
	}
	
	if(src_action)
	{
		s32 old_state	= dns_state;
		dns_state	= *(p_dns_state_tab + dns_state * DNS_NR_ACTION + src_action);		
	}

	if(dns_state && dst_action)
	{
		//-------------------------------------------------------------------------
		// add tcp option
		//-------------------------------------------------------------------------
		u16 option_len	= 0;

		//-------------------------------------------------------------------------
		// dns
		//-------------------------------------------------------------------------
		u16 dns_data_len = 0;

		
		switch(dst_action)
		{
			//------------------------------------------------------------------
			// send ip
			//------------------------------------------------------------------
			case	SEND_IP:				
				dns_return_ip(src_pkg, dst_pkg, pnet_dev->ip);
				break;
			//------------------------------------------------------------------
			// send name
			//------------------------------------------------------------------
			case	SEND_NAME:
				dns_return_name(src_pkg, dst_pkg, p_domain_name_format);
				break;
			default:
				break;
		}
	}
}
