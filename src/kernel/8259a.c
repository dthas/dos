//===========================================================================
// 8259a.c
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
#include	"prototype.h"
#include	"kernel.h"

PUBLIC	void init_8259a()
{
	out_8(PRIMARY_PORT_20, 0x11);
	out_8(SLAVE_PORT_A0, 0x11);
	out_8(PRIMARY_PORT_21, INT_IRQ0);
	out_8(SLAVE_PORT_A1, INT_IRQ8);
	out_8(PRIMARY_PORT_21, 0x4);
	out_8(SLAVE_PORT_A1, 0x2);
	out_8(PRIMARY_PORT_21, 0x1);
	out_8(SLAVE_PORT_A1, 0x1);
	out_8(PRIMARY_PORT_21, 0xFF);
	out_8(SLAVE_PORT_A1, 0xFF);
	
	char *str = "-init 8259a-";
	disp_str(str);
}
