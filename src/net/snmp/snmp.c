//===========================================================================
// snmp.c
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
#include	"snmp.h"
#include	"prototype.h"

PRIVATE	s32	snmp_state_tab[SNMP_NR_STATUS][SNMP_NR_ACTION];

PRIVATE	void	dhcp_statetab_init();

PUBLIC	u8	snmp_stack[STACK_SIZE];

//===========================================================================
// snmp main
//===========================================================================

PUBLIC	void 	snmp_main()
{
	while(1)
	{
	
		struct s_mesg mesg;
		struct s_package dst_pkg;

		struct iaddr ip;

		mesg.src_pid 	= PROC_SNMP;
		mesg.dest_pid 	= PROC_SNMP;
		s32 flags 	= get_msg(&mesg);
		
		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case SNMP_PROCESS:
					dhcp_process((struct s_package *)mesg.msg.buf1, &dst_pkg);
					break;
				case SNMP_GETREQUEST:
					s2ip(&ip, mesg.msg.buf1);
					getrequest(ip, &dst_pkg);
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
PUBLIC	void	init_snmp()
{
	p_snmp_stack 	= snmp_stack;

	dhcp_statetab_init();

	snmp_state	= SNMP_STATE1;
	
//------for test
	char *str = "-init snmp-";
	disp_str(str);
//------for test
}

//===========================================================================
// init statetab
//===========================================================================
PRIVATE	void	dhcp_statetab_init()
{
	s32 i,j;

	p_snmp_state_tab = snmp_state_tab;

	//------------------------------------------------------------------
	// all states are -1 
	//------------------------------------------------------------------
	for(i=0; i<SNMP_NR_STATUS; i++)
	{
		for(j=0; j<SNMP_NR_ACTION; j++)
		{
			snmp_state_tab[i][j]	= SNMP_STATE_END;
		}
	}

	//------------------------------------------------------------------
	// SNMP_STATE1 
	//------------------------------------------------------------------
	snmp_state_tab[SNMP_STATE1][SEND_GETREQUEST]	= SNMP_STATE2;
	snmp_state_tab[SNMP_STATE1][SEND_GETNEXTREQUEST]= SNMP_STATE2;
	snmp_state_tab[SNMP_STATE1][SEND_GETBULKREQUEST]= SNMP_STATE2;
	snmp_state_tab[SNMP_STATE1][SEND_SETREQUEST]	= SNMP_STATE2;

	snmp_state_tab[SNMP_STATE1][SEND_INFORMREQUEST]	= SNMP_STATE4;
	snmp_state_tab[SNMP_STATE1][SEND_REPORT]	= SNMP_STATE4;
	snmp_state_tab[SNMP_STATE1][SEND_TRAP]		= SNMP_STATE4;

	snmp_state_tab[SNMP_STATE1][RECV_GETREQUEST]	= SNMP_STATE3;
	snmp_state_tab[SNMP_STATE1][RECV_GETNEXTREQUEST]= SNMP_STATE3;
	snmp_state_tab[SNMP_STATE1][RECV_GETBULKREQUEST]= SNMP_STATE3;
	snmp_state_tab[SNMP_STATE1][RECV_SETREQUEST]	= SNMP_STATE3;

	//------------------------------------------------------------------
	// SNMP_STATE2 
	//------------------------------------------------------------------
	snmp_state_tab[SNMP_STATE2][RECV_RESPONSE]	= SNMP_STATE4;
	snmp_state_tab[SNMP_STATE2][RECV_TRAP]		= SNMP_STATE4;

	//------------------------------------------------------------------
	// SNMP_STATE3 
	//------------------------------------------------------------------
	snmp_state_tab[SNMP_STATE3][SEND_RESPONSE]	= SNMP_STATE4;
}


//===========================================================================
// dhcp_process
//===========================================================================
PUBLIC	void 	snmp_process(struct s_package *src_pkg, struct s_package *dst_pkg)
{
	u8 src_action		= -1;
	u8 dst_action		= -1;

	//now just a test
	u8	src_option	= GETREQUEST;

	switch(src_option)
	{
		//------------------------------------------------------------------
		// server
		//------------------------------------------------------------------
		case	GETREQUEST:
			src_action	= RECV_GETREQUEST;
			dst_action	= SEND_RESPONSE;
			break;
		//------------------------------------------------------------------
		// client
		//------------------------------------------------------------------
		
		default:
			break;
	}
	
	if(src_action)
	{
		s32 old_state	= snmp_state;
		snmp_state	= *(p_snmp_state_tab + snmp_state * SNMP_NR_ACTION + src_action);		
	}

	if(snmp_state && dst_action)
	{
		//-------------------------------------------------------------------------
		// add dhcp option
		//-------------------------------------------------------------------------
		u16 option_len	= 0;

		//-------------------------------------------------------------------------
		// dhcp
		//-------------------------------------------------------------------------
		u16 dhcp_data_len = 0;

		switch(dst_action)
		{
			//------------------------------------------------------------------
			// server
			//------------------------------------------------------------------
			case	SEND_RESPONSE:				
				
				break;			
			//------------------------------------------------------------------
			// client
			//------------------------------------------------------------------
			default:
				break;
		}
	}
}
