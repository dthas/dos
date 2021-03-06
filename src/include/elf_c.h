//===========================================================================
// elf_c.h
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

#ifndef	_X_ELFC_H_
#define	_X_ELFC_H_

//==========================================================================
//ELF header
//==========================================================================
struct elf_header
{
	s8	ident[16];	//0x0  //(1) identify information(16 characters)
	s16	type;		//0x10 //(2) type
	s16	system;		//0x12 //(3) system architecture
	s32	version;	//0x14 //(4) version
	s32	entry;		//0x18 //(5) program entry
	s32	phtaboff;	//0x1c //(6) program header table offset
	s32	shtaboff;	//0x20 //(7) section header table offset
	s32	flag1;		//0x24 //(8)
	s16	elfhsize;	//0x28 //(9) the size of ELF header(default is 0x34)
	s16	phtabsize;	//0x2a //(10)the size of program header table(default is 0x20)
	s16	phtnrentry;	//0x2c //(11)how many entries in program header table
	s16	shtabsize;	//0x2e //(12)the size of section header table(default is 0x28)
	s16	shtnrentry;	//0x30 //(13)how many entries in section header table
	s16	flag2;		//0x32 //(14)
};


//==========================================================================
//program header table
//==========================================================================
struct prog_header
{		
	s32	type;		//0x0  //(1) type
	s32	offset;		//0x4  //(2) the offset of the first byte in the section
	s32	v_addr;		//0x8  //(3) the virtual address of the first byte in the section
	s32	p_addr;		//0xc  //(4) the physical address of the first byte in the section
	s32	file_len;	//0x10 //(5) the length of the section in file
	s32	men_len;	//0x14 //(6) the length of the section in memory
	s32	flag;		//0x18 //(7) flags
	s32	align;		//0x1c //(8) alignment	
};


#define	ELF_HEADER_SIZE		0x34
#define	PROG_HEADER_SIZE	0x20
#define	SEC_HEADER_SIZE		0x28
	
#endif
