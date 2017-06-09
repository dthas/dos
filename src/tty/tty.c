//===========================================================================
// tty.c
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
#include	"global.h"
#include	"prototype.h"
#include	"kernel.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"proc.h"
#include	"keyboard.h"
#include	"uproc.h"
#include	"tty.h"

extern	current_tty;
extern	pos;

PRIVATE	struct tty_m	*tty;
PRIVATE	struct tty_m	tty0;
PRIVATE	struct tty_m	tty1;
PRIVATE	struct tty_m	tty2;
PRIVATE	struct tty_m	tty3;

PUBLIC	u8	tty_stack[STACK_SIZE];
//===========================================================================
// init tty
//===========================================================================
PUBLIC	void init_tty()
{
	p_tty_stack = tty_stack;


//-------------------------------------------------------------------------
// init tty0
//-------------------------------------------------------------------------
	// init tty structure
	tty0.write_ptr 	= tty0.buffer;
	tty0.read_ptr 	= tty0.buffer;
	tty0.count	= 0;
	tty0.v_addr	= VIDEO_BASE;			//0xb8000
	tty0.s_addr	= VIDEO_BASE;
	tty0.base_addr	= VIDEO_BASE;
	tty0.pos	= pos;
	tty0.pos_offset = 0;
	
	tty0.size	= VIDEO_SIZE;	

//-------------------------------------------------------------------------
// init tty1
//-------------------------------------------------------------------------
	// init tty structure
	tty1.write_ptr 	= tty1.buffer;
	tty1.read_ptr 	= tty1.buffer;
	tty1.count	= 0;
	tty1.size	= TTY_SIZE;
	tty1.pos	= 0;
	tty1.v_addr	= VIDEO_BASE + tty0.size/2;	//0xba000
	tty1.s_addr	= VIDEO_BASE + tty0.size/2;
	tty1.base_addr	= tty0.base_addr + tty0.size;
	tty1.pos_offset	= tty0.size;
	
//-------------------------------------------------------------------------
// init tty2
//-------------------------------------------------------------------------
	// init tty structure
	tty2.write_ptr 	= tty2.buffer;
	tty2.read_ptr 	= tty2.buffer;
	tty2.count	= 0;
	tty2.size	= TTY_SIZE;
	tty2.pos	= 0;
	tty2.v_addr	= VIDEO_BASE + (tty0.size + tty1.size)/2;	//0xbc000
	tty2.s_addr	= VIDEO_BASE + (tty0.size + tty1.size)/2;
	tty2.base_addr	= tty1.base_addr + tty1.size;
	tty2.pos_offset	= tty0.size + tty1.size;
	
	
//-------------------------------------------------------------------------
// init tty3
//-------------------------------------------------------------------------
	// init tty structure
	tty3.write_ptr 	= tty3.buffer;
	tty3.read_ptr 	= tty3.buffer;
	tty3.count	= 0;
	tty3.size	= TTY_SIZE;
	tty3.pos	= 0;
	tty3.v_addr	= VIDEO_BASE + (tty0.size + tty1.size + tty2.size)/2;	//0xbe000
	tty3.s_addr	= VIDEO_BASE + (tty0.size + tty1.size + tty2.size)/2;
	tty3.base_addr	= tty2.base_addr + tty2.size;
	tty3.pos_offset	= tty0.size + tty1.size + tty2.size;	

//-------------------------------------------------------------------------
// init current tty
//-------------------------------------------------------------------------

	switch(current_tty)
	{
		case	0:
			tty	= &tty0;
			break;
		case	1:
			tty	= &tty1;
			break;
		case	2:
			tty	= &tty2;
			break;
		case	3:
			tty	= &tty3;
			break;
		default:
			disp_num(current_tty);
			break;
	}



//-------------------------------------------------------------------------
// init cursor location
//-------------------------------------------------------------------------
	dis_int();
	out_8(ADDR_REG, CURSOR_LOCATION_H_IDX);
	out_8(DATA_REG, ((pos/2)>>8)&0xFF);
	out_8(ADDR_REG, CURSOR_LOCATION_L_IDX);
	out_8(DATA_REG, (pos/2)&0xFF);
	en_int();
	
//------for test
	char *str = "-init tty-";
	disp_str(str);	
//------for test

	reset_cmdlb();
}

//===========================================================================
// tty main
//===========================================================================
PUBLIC	void tty_main()
{
	while(1)
	{
		tty_read();
		tty_write();
		
		struct s_mesg mesg;
		mesg.src_pid 	= PROC_TTY;
		mesg.dest_pid 	= PROC_TTY;
		s32 flags 	= get_msg(&mesg);

		if(flags == HAVE_MSG)
		{
			switch(mesg.type)
			{
				case TTY_WRITE:	
					sys_puts(mesg.msg.buf1, mesg.msg.parm1);
					break;
				case RESET_CMDL:
					reset_cmdlb();
					break;
				default:
					break;
			}
			
			if(mesg.func == SYN)
			{
				struct s_mesg mesg1;

				mesg1.dest_pid	= mesg.src_pid;
				mesg1.type	= PROC_READY;
		
				setstatus(&mesg1);
			}			
		}
	}
}

//===========================================================================
// tty read from keyboard buffer
//===========================================================================
PUBLIC	void tty_read()
{
	keyb_read();
			
}

//===========================================================================
// tty write from tty buffer
//===========================================================================
PUBLIC	void tty_write()
{
	char *ptr = tty->buffer;

	while(tty->count > 0)
	{
		u8 schar = *ptr;
		print_char(schar);
		ptr++;
		tty->count--;
	}

	tty->read_ptr 	= tty->buffer;
	tty->write_ptr 	= tty->buffer;
}

//===========================================================================
// write 1 byte into tty buffer
//===========================================================================
PUBLIC	void tty_buf_write(u8 acode)
{
	if (tty->count < TTY_INPUT_SIZE)
	{
		*(tty->write_ptr)	= acode;
		tty->write_ptr++;
		tty->count++;
	}

	if (cmdl->count < CMDLINE_SIZE)
	{
		*(cmdl->write_ptr)	= acode;
		(cmdl->write_ptr)++;
		(cmdl->count)++;
	}
} 
 

//===========================================================================
// output 1 char to screen
//===========================================================================
PUBLIC	void print_char(u8 schar)
{
	u8* addr;
	u32 line_num;

	switch(schar)
	{
		case '\n':
			line_num 	= (tty->pos/2) / LINE_SIZE;
			line_num++;
			tty->pos	= 2 * line_num * LINE_SIZE;

			addr= VIDEO_BASE + tty->pos + tty->pos_offset;

			if((addr < (tty->base_addr + tty->size)) && (addr >= tty->base_addr ))
			{
				reset_cursor((tty->pos + tty->pos_offset)/2);
		
				if((tty->pos/2 - (tty->v_addr - tty->s_addr)) >= SCREEN_SIZE)
				{
					tty->v_addr = tty->v_addr + LINE_SIZE;
	
					reset_video(tty->v_addr);
				}
		
			}	
			else
			{
				tty->pos	= 0;
				tty->v_addr	= tty->s_addr;				
				reset_cursor((tty->pos + tty->pos_offset)/2);
				reset_video(tty->s_addr);

				clear_screen();
			}
			
			break;

		default:
			addr= VIDEO_BASE + tty->pos + tty->pos_offset;

			if((addr < (tty->base_addr + tty->size)) && (addr >= tty->base_addr ))
			{
				*addr 		= schar;
				*(addr+1)	= COLOR_B_W;

				tty->pos  = tty->pos + 2;
				reset_cursor((tty->pos + tty->pos_offset)/2);
		
				if((tty->pos/2 - (tty->v_addr - tty->s_addr)) >= SCREEN_SIZE)
				{
					tty->v_addr = tty->v_addr + LINE_SIZE;
	
					reset_video(tty->v_addr);
				}
		
			}			
			else
			{
				tty->pos	= 0;
				tty->v_addr	= tty->s_addr;
				reset_cursor((tty->pos + tty->pos_offset)/2);
				reset_video(tty->s_addr);

				clear_screen();

				addr= VIDEO_BASE + tty->pos + tty->pos_offset;

				*addr 		= schar;
				*(addr+1)	= COLOR_B_W;

				tty->pos  = tty->pos + 2;
				reset_cursor((tty->pos + tty->pos_offset)/2);
		
				if((tty->pos/2 - (tty->v_addr - tty->s_addr)) >= SCREEN_SIZE)
				{
					tty->v_addr = tty->v_addr + LINE_SIZE;
	
					reset_video(tty->v_addr);
				}
			}			
			break;
	}	
} 

//===========================================================================
// clear
//===========================================================================
PUBLIC	void clear_screen()
{
	u8 *addr;
	for(addr=tty->v_addr; addr<(tty->v_addr + SCREEN_SIZE * 2); addr=addr+2)
	{
		*addr 		= 0x20;
		*(addr+1)	= COLOR_B_W;
	}
} 

//===========================================================================
// scroll_down
//===========================================================================
PUBLIC	void scroll_down()
{
	tty->v_addr = tty->v_addr + SCREEN_SIZE;
	
	if(tty->v_addr > (tty->s_addr + (tty->size/2)))
	{
		tty->v_addr = tty->s_addr ;		
	}

	reset_video(tty->v_addr);
} 

//===========================================================================
// scroll_up
//===========================================================================
PUBLIC	void scroll_up()
{
	tty->v_addr = tty->v_addr - SCREEN_SIZE;

	if(tty->v_addr < tty->s_addr)
	{			
		tty->v_addr = tty->s_addr + (tty->size/2) - SCREEN_SIZE ;		
	}
		
	reset_video(tty->v_addr);	
} 


//===========================================================================
// reset_cursor
//===========================================================================
PUBLIC	void reset_cursor(u32 addr)
{
	dis_int();
	out_8(ADDR_REG, CURSOR_LOCATION_H_IDX);
	out_8(DATA_REG, (addr>>8)&0xFF);
	out_8(ADDR_REG, CURSOR_LOCATION_L_IDX);
	out_8(DATA_REG, (addr &0xFF));
	en_int();
	
	pos = tty->pos;
} 

//===========================================================================
// reset_cursor
//===========================================================================
PUBLIC	void reset_video(u32 addr)
{
	dis_int();
	out_8(ADDR_REG, START_ADDR_H_IDX);
	out_8(DATA_REG, ((addr)>>8)&0xFF);
	out_8(ADDR_REG, START_ADDR_L_IDX);
	out_8(DATA_REG, (addr)&0xFF);
	en_int();
} 


//===========================================================================
// tab exec
//===========================================================================
PUBLIC	void tab_exec()
{
	int i;
	for(i=0; i<6; i++)
	{
		print_char(ASC_SPACE);
	}
}

//===========================================================================
// backspace exec
//===========================================================================
PUBLIC	void backspace_exec()
{
	tty->pos = tty->pos - 2;

	u8* addr= VIDEO_BASE + tty->pos + tty->pos_offset;

	*addr 		= ASC_SPACE;
	*(addr+1)	= COLOR_B_W;

	reset_cursor((tty->pos + tty->pos_offset)/2);
		
	(cmdl->write_ptr)--;
	(cmdl->count)--;	
}

//===========================================================================
// delete exec
//===========================================================================
PUBLIC	void delete_exec()
{
	tty->pos = tty->pos + 2;

	u8* addr= VIDEO_BASE + tty->pos + tty->pos_offset;

	*addr 		= ASC_SPACE;
	*(addr+1)	= COLOR_B_W;

	reset_cursor((tty->pos + tty->pos_offset)/2);
		
}

//===========================================================================
// enter exec
//===========================================================================
PUBLIC	void enter_exec()
{
	u32 line_num 	= (tty->pos/2) / LINE_SIZE;
	line_num++;
	tty->pos	= 2 * line_num * LINE_SIZE;

	u8* addr= VIDEO_BASE + tty->pos + tty->pos_offset;

	if((addr < (tty->base_addr + tty->size)) && (addr >= tty->base_addr ))
	{
		reset_cursor((tty->pos + tty->pos_offset)/2);
		
		if((tty->pos/2 - (tty->v_addr - tty->s_addr)) >= SCREEN_SIZE)
		{
			tty->v_addr = tty->v_addr + LINE_SIZE;
	
			reset_video(tty->v_addr);
		}
		
	}	
	else
	{
		tty->pos	= 0;
		tty->v_addr	= tty->s_addr;		
		reset_cursor((tty->pos + tty->pos_offset)/2);
		reset_video(tty->s_addr);

		clear_screen();		
	}
	
	if (cmdl->count < CMDLINE_SIZE)
	{
		*(cmdl->write_ptr)	= '\0';
		
		struct s_mesg mesg;

		mesg.msg.buf1	= laddr_to_paddr(PROC_TTY, cmdl->buffer);
		mesg.src_pid	= cpid();
		mesg.dest_pid	= PROC_UPROC;
		mesg.type	= CMD_EXEC;
		mesg.func	= SYN;	
		
		send_msg(&mesg);
		
		reset_cmdlb();
	}
}

//===========================================================================
// directiont_exec
//===========================================================================
PUBLIC	void direction_exec(u8 direction)
{
	u8 *addr;
	u32 tmp_pos = tty->pos;

	switch(direction)
	{
		case	UP:
			tty->pos= tty->pos - LINE_SIZE * 2;
			break;
		case	LEFT:
			tty->pos= tty->pos - 2;
			break;
		case	RIGHT:
			tty->pos= tty->pos + 2;
			break;
		case	DOWN:
			tty->pos= tty->pos + LINE_SIZE * 2;
			break;
		default:
			disp_num(direction);
			break;
	}

	addr	= VIDEO_BASE + tty->pos + tty->pos_offset;

	if((addr < (tty->base_addr + tty->size)) && (addr >= tty->base_addr ))
	{
		reset_cursor((tty->pos + tty->pos_offset)/2);
		
		if((tty->pos/2 - (tty->v_addr - tty->s_addr)) >= SCREEN_SIZE)
		{
			tty->v_addr = tty->v_addr + LINE_SIZE;
	
			reset_video(tty->v_addr);
		}
		
	}
	else
	{
		tty->pos = tmp_pos;
	}	
}


//===========================================================================
// home exec
//===========================================================================
PUBLIC	void home_exec()
{
	u32 line_num 	= (tty->pos/2) / LINE_SIZE;
	tty->pos	= 2 * line_num * LINE_SIZE;

	reset_cursor((tty->pos + tty->pos_offset)/2);
}

//===========================================================================
// end exec
//===========================================================================
PUBLIC	void end_exec()
{
	u32 line_num 	= (tty->pos/2) / LINE_SIZE;
	line_num++;
	tty->pos	= 2 * line_num * LINE_SIZE - 2;		// the last character of a line

	reset_cursor((tty->pos + tty->pos_offset)/2);
}


//===========================================================================
// change_tty
//===========================================================================
PUBLIC	void change_tty(u8 ttyn)
{
	switch(ttyn)
	{
		case	0:
			tty	= &tty0;
			break;
		case	1:
			tty	= &tty1;
			break;
		case	2:
			tty	= &tty2;
			break;
		case	3:
			tty	= &tty3;
			break;
		default:
			disp_num(ttyn);
			break;
	}

	reset_cursor((tty->pos + tty->pos_offset)/2);
	reset_video(tty->v_addr);
}


//===========================================================================
// init cmdline buffer 
//===========================================================================
PUBLIC	void reset_cmdlb()
{
	cmdl->write_ptr 	= cmdl->buffer;
	cmdl->count		= 0;
}

//===========================================================================
// 0: sys_puts
//===========================================================================
PUBLIC 	int 	sys_puts(char *buf, int len)
{
	int i;

	for(i = 0; i<len; i++)
	{
		tty_buf_write(*(buf+i));
		tty_write();			
	}
}

//===========================================================================
// 0: sys_prints:: just for printk
//===========================================================================
PUBLIC 	s32 	sys_prints(s8 *buf)
{
	s8 *pbuf= (void *)laddr_to_paddr(proc_ready->pid, buf);

	s32 len	= strlen(pbuf);

	s32 i;

	for(i = 0; i<len; i++)
	{
		print_char(*(pbuf+i));		
	}

	return i;
}
