//===========================================================================
// clock.c
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
#include	"kernel.h"
#include	"global.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"proc.h"
#include	"prototype.h"

extern	int 	flg_int_rein;
extern 	u32 	ticket;


//===========================================================================
// init clock
//===========================================================================
PUBLIC	void init_clock()
{
	wait_schd_tick	= PRI_TICK_BASE;
	proc_schd_next	=  &proc_tab[0];	//the first program should be searched in the program table

	u8 t1 = TIME_FREQUENCE / TIME_HZ;		// low  8 bit
	u8 t2 = (TIME_FREQUENCE / TIME_HZ) >> 8;	// high 8 bit

	out_8(TIME_43, TIME_RATE_GEN);
        out_8(TIME_40, t1);
        out_8(TIME_40, t2);

        en_irq(CLOCK_IRQ);
}


//===========================================================================
// init clock
//===========================================================================
PUBLIC void clock_driver()
{
	ticket++;

	if((--wait_schd_tick) <= 0)
	{
		schd();
		wait_schd_tick	= PRI_TICK_BASE;
	}

	if (proc_ready->ticks)
	{
		proc_ready->ticks--;
	}

	if(flg_int_rein != 0)
	{
		return;
	}

	if(!proc_ready->ticks)
	{
		schd();
	}
}

//===========================================================================
// schedule
//
// All the ready programs are scheduled in turns, not depend on their priority.
// The variable proc_schd_next is the next place that searched from proc_tab[].
// (proc_schd_next - 1) is the program that last scheduled. And the range of
// proc_schd_next is (proc_tab, &proc_tab[PROC_NUM - 1])
//===========================================================================
PUBLIC void schd()
{
	struct proc *p_head = proc_tab;
	struct proc *p_tail = &proc_tab[PROC_NUM - 1];
	struct proc *cur_proc;

	u8	flg_proc    = FALSE;
	
	//---------------------------------------------------
	// for(proc_schd_next ~ tail)
	//---------------------------------------------------
	for(cur_proc = proc_schd_next; cur_proc <= p_tail; cur_proc++)
	{
		if(cur_proc->status == PROC_READY)
		{
			if(cur_proc->ticks)
			{
				proc_ready 	= cur_proc;
				flg_proc	= TRUE;
				break; 
			}			
		}				
	}

	//---------------------------------------------------
	// for(head ~ proc_schd_next)
	//---------------------------------------------------
	if(!flg_proc)
	{
		for(cur_proc = p_head; cur_proc < proc_schd_next; cur_proc++)
		{
			if(cur_proc->status == PROC_READY)
			{
				if(cur_proc->ticks)
				{
					proc_ready 	= cur_proc;
					flg_proc	= TRUE;
					break; 
				} 			
			}				
		}
	}

	//---------------------------------------------------
	// if not in(head, tail)
	// {
	//	(1)all the ready programs need to reset their ticks(ticks_max)
	//	(2)pick up a ready program from all the ready programs
	// }
	//---------------------------------------------------
	if(!flg_proc)
	{
		for(cur_proc = p_head; cur_proc < p_tail; cur_proc++)
		{
			if(cur_proc->status == PROC_READY)
			{
				cur_proc->ticks	= cur_proc->ticks_max;
			}				
		}

		for(cur_proc = p_head; cur_proc <= p_tail; cur_proc++)
		{
			if(cur_proc->status == PROC_READY)
			{
				if(cur_proc->ticks)
				{
					proc_ready 	= cur_proc;
					break; 
				}			
			}				
		}
	}

	//---------------------------------------------------
	// setup proc_schd_next
	//---------------------------------------------------
	proc_schd_next	= cur_proc + 1;
	
	if(proc_schd_next > p_tail)
	{
		proc_schd_next	= p_head;
	}
}
