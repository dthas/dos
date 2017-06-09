//===========================================================================
// hd.h
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

#ifndef	_X_HD_H_
#define	_X_HD_H_

//-------------------------------------------------------------------------
// Partition Infomation(In sectors)
//-------------------------------------------------------------------------
#define	PRI_TAB_START	0
#define	PRI_SEC_START	0x3F		//(0 + 0x3F)
#define	PRI_SEC_SIZE	0x4E81

#define	EXT_SEC_START	0x4EC0		//(0x4E81+0x3F)
#define	EXT_SEC_SIZE	0x2D090	

#define	LOG_TAB_START	0x4EC0		//EXT_SEC_START
#define	LOG_SEC_START	0x4EFF		//(0x4EC0+0x3F)
#define	LOG_SEC_SIZE	0x2D051

//-------------------------------------------------------------------------
// command block ports(primary)
//-------------------------------------------------------------------------
#define	CBP_PORT0	0x1f0		//data
#define	CBP_PORT1	0x1f1		//err/feature
#define	CBP_PORT2	0x1f2		//sector count
#define	CBP_PORT3	0x1f3		//lba low
#define	CBP_PORT4	0x1f4		//lba mid
#define	CBP_PORT5	0x1f5		//lba high
#define	CBP_PORT6	0x1f6		//device/command
#define	CBP_PORT7	0x1f7		//status

//-------------------------------------------------------------------------
// control block ports
//-------------------------------------------------------------------------
#define	CBTP_PORT0	0X3F6		//alternate/device,status/control

//-------------------------------------------------------------------------
// harddisk command
//-------------------------------------------------------------------------
#define	HD_READ		0x20
#define	HD_WRITE	0x30
#define	HD_IDEN		0xEC

//-------------------------------------------------------------------------
// dev parameter
//-------------------------------------------------------------------------
#define	DEV_DTS		0x5

//-------------------------------------------------------------------------
// IDENTITY returns : hardware parameters
//-------------------------------------------------------------------------
#define	NUM_SEQ_START	20	//10
#define	NUM_SEQ_END	40	//19

#define	TYPE_START	54	//27
#define	TYPE_END	94	//46

#define	NUM_SEC_START	120	//60
#define	NUM_SEC_END	124	//61

#define	LBA_SUP_START	98	//49
#define	LBA_SUP_END	102	//50

#define	INS_SET_START	166	//83
#define	INS_SET_END	170	//84


//-------------------------------------------------------------------------
// command block registers
//-------------------------------------------------------------------------	
struct s_cmd_blk
{
	s8	data;
	s8	feature;
	s8	sector_count;
	s8	lba_low;
	s8	lba_mid;
	s8	lba_high;
	s8	dev;
	s8	cmd;
};

//-------------------------------------------------------------------------
// control block registers
//-------------------------------------------------------------------------
struct s_cbr
{
	s8	dev;					//alternate/device
	s8	status;					//status/control
};

//-------------------------------------------------------------------------
// hard disk blocks management struct
//-------------------------------------------------------------------------
struct s_hdm
{
	struct s_hdm * 	head;
	struct s_hdm * 	tail;
	u32		addr;				//start address
	u32		nr_blk;				//size in block	
};

//-------------------------------------------------------------------------
// hard disk information struct
//-------------------------------------------------------------------------
struct s_hdinfo
{
	s8	seq[20];
	s8	type[40];
	s32	sec;
	s8	lba[2];
	s8	ins[2];
};

u8	*p_hd_stack;

PUBLIC s8 hdbuf[BLOCK_SIZE];

s8	flg_int;

#endif
