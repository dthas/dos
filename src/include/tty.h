//===========================================================================
// tty.h
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
#ifndef	_X_TTY_H_
#define	_X_TTY_H_

#define	TTY_INPUT_SIZE	512

#define	VIDEO_BASE	0xb8000
#define	VIDEO_SIZE	32000

#define SCREEN_SIZE	2000		//(80 * 25)
#define	LINE_SIZE	80
#define	TTY_SIZE	8000

#define	COLOR_B_W	0x07		// black , white

// for video registers
#define	START_ADDR_H_IDX	0xC
#define	START_ADDR_L_IDX	0xD
#define	CURSOR_LOCATION_H_IDX	0xE
#define	CURSOR_LOCATION_L_IDX	0xF

#define	ADDR_REG		0x3D4
#define	DATA_REG		0x3D5

#define	TTY0	0
#define	TTY1	1
#define	TTY2	2
#define	TTY3	3

#define	UP	0
#define	LEFT	1
#define	DOWN	2
#define	RIGHT	3

//-------------------------------------------------------------------------
// variable definition
//-------------------------------------------------------------------------
u8	*p_tty_stack;

//-------------------------------------------------------------------------
// structure definition
//-------------------------------------------------------------------------

struct tty_m {
	char*	write_ptr;
	char*	read_ptr;			
	u8	cur_tty;
	u32	pos;
	u32	pos_offset;
	u32	v_addr;			//current video start address(in a screen)	
	u32	s_addr;			//current tty start address by words
	u32	base_addr;		//current tty start address by bytes
	int	size;
	int	count;			
	char	buffer[TTY_INPUT_SIZE];	
};


//-------------------------------------------------------------------------
// cmdline buffer for user program on main.c
//------------------------------------------------------------------------- 
#define	CMDLINE_SIZE	512
struct cmdline {
	s8*	write_ptr;
	s32	count;			
	s8	buffer[CMDLINE_SIZE];
};

PUBLIC struct cmdline *cmdl;

#endif
