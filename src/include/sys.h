//===========================================================================
// sys.h
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

#ifndef	_X_SYS_H_
#define	_X_SYS_H_

u8	*p_sys_stack;

#define CMOS_INFO_LEN	64	//total is 64 bytes

//--------------------------------------------------------------------------------
// time setting(should be in time.h)
//--------------------------------------------------------------------------------
#define	JAN	1
#define	FEB	2
#define	MAR	3
#define	APL	4
#define	MAY	5
#define	JUN	6
#define	JUL	7
#define	AUG	8
#define	SEP	9
#define	OCT	10
#define	NOV	11
#define	DEC	12

#define	A_MIN_SECONDS		60
#define	A_HOUR_SECONDS		3600		//60 * 60
#define	A_DAY_SECONDS		86400		//3600 * 24
#define	A_MON_28_SECONDS	2419200		//86400 * 28
#define	A_MON_29_SECONDS	2505600		//86400 * 29
#define	A_MON_30_SECONDS	2592000		//86400 * 30
#define	A_MON_31_SECONDS	2678400		//86400 * 31
#define	A_SIMP_YEAR_SECONDS	31536000	//86400 * 365
#define	A_LEAP_YEAR_SECONDS	31622400	//86400 * 366

#define	BASIC_YEAR	2000			// basic date : 2000-1-1 0:0:0
#define	BASIC_MON	1
#define	BASIC_DAY	1
#define	BASIC_HOUR	0
#define	BASIC_MIN	0
#define	BASIC_SEC	0

#define	DATE_OFF_CENT	0x32
#define	DATE_OFF_YEAR	0x9
#define	DATE_OFF_MON	0x8
#define	DATE_OFF_DAY	0x7
#define	DATE_OFF_HOUR	0x4
#define	DATE_OFF_MIN	0x2
#define	DATE_OFF_SEC	0x0

struct s_time
{
	u16	year;
	u8	mon;
	u8	day;
	u8	hour;
	u8	min;
	u8	sec;
};

struct s_time	*sys_rtime;	//is the time that the system start to run
struct s_time	*sys_ntime;	//now

u32	sysrtime_sec;
u32	sysntime_sec;

#endif
