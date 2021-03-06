//===========================================================================
// uproc.c
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
#include	"uproc.h"
#include	"prototype.h"

extern	pos;

PUBLIC	u8	uproc_stack[STACK_SIZE];
PUBLIC	u8	vem_map[VMEM_HD_MAP_NUM];	//for virtual memory mapping

//===========================================================================
// init tty
//===========================================================================
PUBLIC	void init_uproc()
{
	p_uproc_stack = uproc_stack;

	int i;
	for(i=0; i<VMEM_HD_MAP_NUM; i++)
	{
		vem_map[i] = 0;
	}
	hd_vem_map = vem_map;

	mem_stratigic	= MEM_PAGE;
		
//------for test
	s8 *str = "-init uproc-";
	disp_str(str);
//------for test

}

//===========================================================================
// init shell
//===========================================================================
PUBLIC	void init_shell()
{
	s32 i 	= pos / 160 + 1;
	pos	= i * 160;
		
	s8 *str = "/>";
	disp_str(str);
}

//===========================================================================
// uproc main
//===========================================================================
PUBLIC	void 	uproc_main()
{
	init_mem_freelist();

	reset_cmdlineT();
	sys_reset_cmdline();

	while(1)
	{
		struct s_mesg mesg;
		mesg.src_pid 	= PROC_UPROC;
		mesg.dest_pid 	= PROC_UPROC;
		s32 flags 	= get_msg(&mesg);
		s32 val;

		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case CMD_EXEC:	
					cmd_exec(mesg.msg.buf1);
					break;
				case M_ALLOC:
					val = sys_alloc(mesg.msg.parm1);
					break;
				case EXIT:
					val = sys_exit(mesg.msg.parm1);	

					sys_reset_cmdline();				
					break;
				case ENPAGE:
					sys_enpage(mesg.msg.parm1);
					break;	
				case VM_ALLOC:
					val = sys_valloc(mesg.msg.parm1);
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
// reset cmdline
//===========================================================================
PUBLIC	void 	reset_cmdlineT()
{

	struct s_mesg mesg1;

	mesg1.dest_pid	= PROC_TTY;
	mesg1.type	= TTY_WRITE;
	mesg1.func	= SYN;
	mesg1.msg.buf1	= "\n\n";
	mesg1.msg.parm1	= 2;
		
	send_msg(&mesg1);
}

//===========================================================================
// reset cmdline
//===========================================================================
PUBLIC	void 	sys_reset_cmdline()
{

	struct s_mesg mesg1;

	mesg1.dest_pid	= PROC_TTY;
	mesg1.type	= TTY_WRITE;
	mesg1.func	= SYN;
	mesg1.msg.buf1	= "\n/>";
	mesg1.msg.parm1	= 3;
	
	send_msg(&mesg1);
	
	struct s_mesg mesg2;

	mesg2.dest_pid	= PROC_TTY;
	mesg2.type	= RESET_CMDL;
	mesg2.func	= SYN;
		
	send_msg(&mesg2);
}


//===========================================================================
// cmd_exec
//===========================================================================
PUBLIC	void 	cmd_exec(s8 *buf)
{	
	s8  argv[PARM_NUM][PARM_LEN];	

	s32 i, j, argc;
	s8 flg = FALSE;

	for(i=0;i<PARM_NUM ; i++)
	{
		if(flg == TRUE)
		{
			break;
		}

		for(j=0; j<PARM_LEN; j++, buf++)
		{
			if ((*buf != ' ') && (*buf != '\0'))
			{
				argv[i][j] = *buf;
			}
			else if (*buf == ' ') 
			{
				argv[i][j] = '\0';
				buf++;
				break;					
			}
			else if (*buf == '\0')
			{
				argv[i][j] = '\0';
				flg = TRUE;
				break;
			}
		}
	}

	argc = i;
	
	u32	argv_addr	= argv;	

	struct proc *new_p 	= sys_fork();
	
	sys_execl(new_p, &argv[0][0], argc, argv_addr);
}

//===========================================================================
// stack_setup
//===========================================================================
PUBLIC	void 	stack_setup(u32 stack_base_p, u32 stack_base_v, s32 argc, u32 argv_addr)
{
	s32 i, len;
	
	u32 p_point_start	= stack_base_p;
	
	s8 *p_parm_start	= stack_base_p + argc * sizeof(s32 *);
	s8 *v_parm_start	= stack_base_v + argc * sizeof(s32 *);

	s8 *p			= argv_addr;

	for(i=0; i<argc; i++)
	{			
		*(u32*)(p_point_start)	= v_parm_start;

		len	 	= strlen(p + i*PARM_LEN) + 1;

		strcpy(p_parm_start, (p + i*PARM_LEN), len);
					
		p_parm_start 	= p_parm_start + len;	
		p_point_start	= p_point_start + sizeof(s32 *);
		
		v_parm_start 	= v_parm_start + len;	
	}	
}

//===========================================================================
// print stack
//===========================================================================
PUBLIC	void 	stack_print(u32 stack_base, s32 argc)
{
	s32 i;

	for(i=0; i<argc; i++)
	{
		disp_str("[adr");
		disp_num(i);
		disp_str(":");
		s32	j	= i * sizeof(s32 *);
		u32	addr 	= stack_base + j;
		disp_num(*(u32*)(addr));
		disp_str(",str:");
		disp_str((u8*)(*(u32*)(addr)));
		disp_str("];");
	}
}
