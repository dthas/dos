//===========================================================================
// libsys.c
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

//===========================================================================
// laddr_to_paddr
//===========================================================================
PUBLIC	s32	cpid()
{
	return proc_ready->pid;
}


//===========================================================================
// laddr_to_paddr
//===========================================================================
PUBLIC	u32	l2p_addr(u32 pid, u32 laddr)
{
	u32 base_addr 	= u_desc_baseaddr_get(pid, proc_ready->gdt_desc_addr);

	u32 paddr	= base_addr + laddr;

	return paddr;
}
