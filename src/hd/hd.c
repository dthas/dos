//===========================================================================
// hd.c
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
#include	"kernel.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"proc.h"
#include	"mesg.h"
#include	"prototype.h"

// variable list
struct s_hdinfo	shdinfo;
struct s_hdinfo * hdinfo = &shdinfo;

PRIVATE u8	hd_sts;

PUBLIC	u8	hd_stack[STACK_SIZE];

// function list
PRIVATE void cmd_send(struct s_cmd_blk * cmd_blk);

// for test
extern 	gdt_desc;

//===========================================================================
// hd initiation
//===========================================================================
PUBLIC	void hd_main()
{
	struct s_mesg mesg;

	while(1)
	{
		mesg.src_pid	= PROC_HD;
		mesg.dest_pid 	= PROC_HD;
		s32 flags 	= get_msg(&mesg);

		if(flags == HAVE_MSG)
		{			
			u32 start_sec;
			s16 *buf;
			u32 blk_cnt;
	
			switch(mesg.type)
			{
				case HARDDISK_READ:
					start_sec 	= mesg.msg.parm1;
					buf		= (s16*)mesg.msg.buf1;		// the paddr of fsbuf
					blk_cnt		= mesg.msg.parm2;	
					hd_read_blk(start_sec, buf, blk_cnt);
					break;
				case HARDDISK_WRITE:
					start_sec 	= mesg.msg.parm1;
					buf		= (s16*)mesg.msg.buf1;		// the paddr of fsbuf
					blk_cnt		= mesg.msg.parm2;	
					hd_write_blk(start_sec, buf, blk_cnt);
					break;
				case HD_INITFREELIST:
					init_hd_freelist();
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
// hd initiation
//===========================================================================
PUBLIC	void init_hd(void)
{	
	p_hd_stack = hd_stack;
	
	flg_int	= FALSE;

	en_irq(CONCATE_IRQ);
	en_irq(HARDDISK_IRQ);
	
	init_part();

//-----for test
	char *str = "-init hd-";
	disp_str(str);
} 


//===========================================================================
// hd_read_blk
//===========================================================================
PUBLIC	s32 hd_read_blk(u32 start_sector, s16 * buf, u32 blk_cnt)
{
	//-----------------------------------------------------------------
	//check hd if it's busy( if hd_sts == 0XXXXXXX then go on else loop)
	//-----------------------------------------------------------------

	//-----------------------------------------------------------------
	//fill command 
	//-----------------------------------------------------------------
	struct	s_cmd_blk	cmd_blk;

	cmd_blk.feature	= 0;
	cmd_blk.sector_count	= blk_cnt * SECTOR_PER_BLOCK;			// blk_cnt * 8
	cmd_blk.lba_low		= start_sector & 0xFF;
	cmd_blk.lba_mid		= (start_sector >> 8) & 0xFF;
	cmd_blk.lba_high	= (start_sector >> 16) & 0xFF;
	cmd_blk.dev		= ((start_sector >> 24) & 0xF) | 0xA0 | 0x40 ;
	cmd_blk.cmd		= HD_READ;					// HD_WRITE / HD_READ

	cmd_send(&cmd_blk);

	//-----------------------------------------------------------------
	//get result
	//-----------------------------------------------------------------
	s32 i,j;
	s32 total_size 	= blk_cnt * BLOCK_SIZE / 2;		// blk_cnt * 4096 bytes / 2 = blk_cnt * 2048 words
	
	for(i=0; i<total_size; )
	{
		waitfor_int();
		
		for(j=0; j<SECTOR_SIZE; j++, i++)
		{
			*(buf+i) = in_16(CBP_PORT0);
		}		
	}	

	return	i;
} 


//===========================================================================
// hd_write_blk
//===========================================================================
PUBLIC	s32 hd_write_blk(u32 start_sector, s16 *buf, u32 blk_cnt)
{
	struct	s_cmd_blk	cmd_blk;

	//-----------------------------------------------------------------
	//fill command 
	//-----------------------------------------------------------------
	cmd_blk.feature	= 0;
	cmd_blk.sector_count	= blk_cnt * SECTOR_PER_BLOCK;			// blk_cnt * 8
	cmd_blk.lba_low		= start_sector & 0xFF;
	cmd_blk.lba_mid		= (start_sector >> 8) & 0xFF;
	cmd_blk.lba_high	= (start_sector >> 16) & 0xFF;
	cmd_blk.dev		= ((start_sector >> 24) & 0xF) | 0xA0 | 0x40 ;
	cmd_blk.cmd		= HD_WRITE;					// HD_WRITE / HD_READ

	cmd_send(&cmd_blk);

	//-----------------------------------------------------------------
	//get result
	//-----------------------------------------------------------------
	s32 i,j;
	s32 total_size 	= blk_cnt * BLOCK_SIZE / 2;		// blk_cnt * 4096 bytes / 2 = blk_cnt * 2048 words
	
	for(i=0; i<total_size; )
	{
		waitfor_int();
		
		for(j=0; j<SECTOR_SIZE; j++, i++)
		{
			out_16(CBP_PORT0, *(buf+i));
		}		
	}

	return	i;
} 

//===========================================================================
// cmd_send
//===========================================================================
PRIVATE void cmd_send(struct s_cmd_blk * cmd_blk)
{
	//send write to control block register
	out_8(CBTP_PORT0, 0);

	//send command block registers
	out_8(CBP_PORT1, cmd_blk->feature);
	out_8(CBP_PORT2, cmd_blk->sector_count);
	out_8(CBP_PORT3, cmd_blk->lba_low);
	out_8(CBP_PORT4, cmd_blk->lba_mid);
	out_8(CBP_PORT5, cmd_blk->lba_high);
	out_8(CBP_PORT6, cmd_blk->dev);
	out_8(CBP_PORT7, cmd_blk->cmd);
}

//===========================================================================
// get hd information
//===========================================================================
PUBLIC	void get_hd_info(s16 *buf)
{	
	struct	s_cmd_blk	cmd_blk;

	//-----------------------------------------------------------------
	//fill command 
	//-----------------------------------------------------------------
	cmd_blk.feature		= 0;
	cmd_blk.sector_count	= 0;			
	cmd_blk.lba_low		= 0;
	cmd_blk.lba_mid		= 0;
	cmd_blk.lba_high	= 0;
	cmd_blk.dev		= 0xA0;
	cmd_blk.cmd		= HD_IDEN;					// HD_WRITE / HD_READ

	cmd_send(&cmd_blk);	

	//-----------------------------------------------------------------
	//get result
	//-----------------------------------------------------------------
	s32 i;
	s32 total_size = SECTOR_SIZE / 2;		// 512 bytes / 2 = 256 words

	for(i=0; i<total_size; i=i+1)
	{
		*(buf+i) = in_16(CBP_PORT0);
	}
	
	//-----------------------------------------------------------------
	//fill s_hdinfo
	//-----------------------------------------------------------------
	s8 * p;
	s32 j;
	
//seq	
	p = (s8*)buf + NUM_SEQ_START;
	for(j=0; j < 20 ; p=p+2, j=j+2)
	{
		hdinfo->seq[j] = *(p+1);			// high 8 bit
		hdinfo->seq[j+1] = *p;				// low 8 bit
	}
	hdinfo->seq[19] = '\0';
	
//type
	p = (s8*)buf + TYPE_START;
	for(j=0; j < 20 ; p=p+2, j=j+2)
	{
		hdinfo->type[j] = *(p+1);			// high 8 bit
		hdinfo->type[j+1] = *p;				// low 8 bit
	}
	hdinfo->type[39] = '\0';

//sec
	p = (s8*)buf + NUM_SEC_START;
	hdinfo->sec = *(s32*)p;

//lba
	p = (s8*)buf + LBA_SUP_START;
	s16 tlba = *(s16*)p;
	tlba = tlba & 0x200;
	if(!tlba)
	{
		hdinfo->lba[0] = 'N';
		hdinfo->lba[1] = '\0';
	}
	else
	{
		hdinfo->lba[0] = 'Y';
		hdinfo->lba[1] = '\0';
	}

//ins
	p = (s8*)buf + INS_SET_START;
	s16 tins = *(s16*)p;
	tins = tins & 0x400;
	if(!tins)
	{
		hdinfo->ins[0] = 'N';
		hdinfo->ins[1] = '\0';
	}
	else
	{
		hdinfo->ins[0] = 'Y';
		hdinfo->ins[1] = '\0';
	}

} 

//===========================================================================
// print hd information
//===========================================================================
PUBLIC	void print_hd_info(struct s_hdinfo * hdinfo)
{
	disp_str("HD info: [seq :");
	disp_str(hdinfo->seq);
	disp_str("], ");

	disp_str("[type :");
	disp_str(hdinfo->type);
	disp_str("], ");

	disp_str("[sec :");
	disp_num(hdinfo->sec);
	disp_str("], ");

	disp_str("[lba :");
	disp_str(hdinfo->lba);
	disp_str("], ");

	disp_str("[ins :");
	disp_str(hdinfo->ins);
	disp_str("]");
}


//===========================================================================
// hd handler
//===========================================================================
PUBLIC	void harddisk_driver(void)
{	
	hd_sts = in_8(CBP_PORT7);
} 


//===========================================================================
// waitfor_int();
//===========================================================================
PUBLIC	void waitfor_int(void)
{
	while(((hd_sts >> 7) == 1) || ((hd_sts >> 3) == 0))
	{};
} 
