//===========================================================================
// vfs.c
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
#include	"stdio.h"
#include	"kernel.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"proc.h"
#include	"mesg.h"
#include	"prototype.h"

//variable
PRIVATE	struct fstype	fstype_tab[NUM_FS_TYPE_SUPPORT] = 
{
	{ FS_TYPE_DTS, 	  FS_DTS_MAGIC_CODE },
	{ FS_TYPE_FAT32,  FS_FAT32_MAGIC_CODE}	
};

PRIVATE struct	s_v_super_blk * super_blk;

PRIVATE struct s_vfs svfs;

PUBLIC	u8	fs_stack[STACK_SIZE];
PRIVATE	void open_detail(u32 magic_code);
PRIVATE	void init_vfs_a();

// for test
extern 	gdt_desc;

//===========================================================================
// init_vfs()
//===========================================================================
PUBLIC	void vfs_main()
{
	init_vfs_a();		// some initial operation need to finish after clock set.

	while(1)
	{
		struct s_mesg mesg;
		mesg.src_pid 	= PROC_FS;
		mesg.dest_pid 	= PROC_FS;
		s32 flags 	= get_msg(&mesg);		

		if(flags == HAVE_MSG)
		{			
			switch(mesg.type)
			{
				case FS_READ:
					sys_read(mesg.msg.buf1, mesg.msg.parm1);
					break;
				case FS_CAT:
					sys_cat(mesg.msg.buf1, mesg.msg.buf2, mesg.msg.parm1);				
					break;
				case FS_MKDIR:
					sys_mkdir(mesg.msg.buf1);				
					break;
				case FS_MKFILE:
					sys_mkfile(mesg.msg.buf1);				
					break;
				case FS_RMDIR:
					sys_rmdir(mesg.msg.buf1);				
					break;
				case FS_RMFILE:
					sys_rmfile(mesg.msg.buf1);				
					break;
				case FS_LS:
					sys_ls(mesg.msg.buf1, mesg.msg.parm1);				
					break;
				case FS_CD:
					sys_cd(mesg.msg.buf1);				
					break;
				case FS_PWD:
					sys_pwd();				
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
// init_vfs_a()
//===========================================================================
PRIVATE	void init_vfs_a()
{
	// some initial operation need to finish after clock set.
	vfs_open();	

	
	//init hd_freelist
	struct s_mesg mesg1;

	mesg1.src_pid	= cpid();
	mesg1.dest_pid	= PROC_HD;
	mesg1.type	= HD_INITFREELIST;
	mesg1.func	= SYN;
	
	send_msg(&mesg1);

}

//===========================================================================
// init_vfs()
//===========================================================================
PUBLIC	void init_vfs()
{
//------for test
	char *str1 = "-init vfs(b)-";
	disp_str(str1);
//------for test
	

	
	p_fs_stack 	= fs_stack;	
	p_tmp_data	= VFS_TMP_DATA;		//7MB, global.h
}


//===========================================================================
// init_vfs()
//===========================================================================
PUBLIC	void vfs_open()
{
	char *str = "--unknown fs--";
	s8 flg = 0;

	vfs	= &svfs;

	// the 0th block is boot block while 1th is the super block; 
	// so need to and 1 block(1 block = 8 sector) 
	u32	start_sec = LOG_SEC_START + SECTOR_PER_BLOCK; 		// 0x4EFF + 8 = 0x4F07

	//-----------------------------------------------------------------
	// send a message to HD, which need to read a block
	//-----------------------------------------------------------------
	struct s_mesg mesg;

	mesg.msg.buf1	= laddr_to_paddr(PROC_FS, fsbuf);

	mesg.msg.parm1	= start_sec;
	mesg.msg.parm2	= 1;			// read 1 sector
	mesg.src_pid	= PROC_FS;
	mesg.dest_pid	= PROC_HD;
	mesg.type	= HARDDISK_READ;
	mesg.func	= SYN;
		
	send_msg(&mesg);

	//-----------------------------------------------------------------
	// init fs
	//-----------------------------------------------------------------
	struct fstype * fst;
	s8 i;

	for(fst= fstype_tab, i = 0; i < NUM_FS_TYPE_SUPPORT; i++, fst++)
	{
		switch(fst->type)
		{
			case FS_TYPE_DTS:
				super_blk = (struct s_dts_super_blk*)super_blk;
				super_blk = fsbuf;

				if(super_blk->flag_magic == fst->magic_code)
				{
					open_detail(FS_TYPE_DTS);
					flg = 1;
				}
				break;
			default:
				break;
		}

		if(flg)
		{
			break;
		}

	}

	if(!flg)
	{		
		printk("%s\n",str);
	}
}


//===========================================================================
// open_detail(u32 magic_code)
//===========================================================================
PRIVATE	void open_detail(u32 magic_type)
{
	fs_op * fs_tab	;
	struct	s_v_super_blk * src_super_blk;
	
	u32	dev = DEV_DTS;

	switch(magic_type)
	{
		case FS_TYPE_DTS:
			fs_tab	= dts_get_fstab();
			src_super_blk = super_blk;			
			break;
		default:
			break;
	}



	//-----------------------------------------------------------------
	// init vfs functions
	//-----------------------------------------------------------------
	vfs->add_inode		= fs_tab[0];
	vfs->add_block		= fs_tab[1];
	vfs->add_imap_bit	= fs_tab[2];
	vfs->add_bmap_bit	= fs_tab[3];
	vfs->add_dir_ent 	= fs_tab[4];

	vfs->del_inode		= fs_tab[5];
	vfs->del_block		= fs_tab[6];
	vfs->del_imap_bit	= fs_tab[7];
	vfs->del_bmap_bit	= fs_tab[8];
	vfs->del_dir_ent 	= fs_tab[9];

	vfs->get_inode		= fs_tab[10];
	vfs->get_super_blk 	= fs_tab[11];
	vfs->update_inode 	= fs_tab[12];


	//-----------------------------------------------------------------
	// init vfs super block
	//-----------------------------------------------------------------
	vfs->spk.num_inode 		= src_super_blk->num_inode; 
	vfs->spk.num_inode_blocks 	= src_super_blk->num_inode_blocks;
	vfs->spk.num_imap_blocks 	= src_super_blk->num_imap_blocks;
	vfs->spk.num_blocks	 	= src_super_blk->num_blocks;
	vfs->spk.num_bmap_blocks 	= src_super_blk->num_bmap_blocks;
	vfs->spk.block_size	 	= src_super_blk->block_size;
	vfs->spk.inode_size	 	= src_super_blk->inode_size;
	vfs->spk.flag_magic	 	= src_super_blk->flag_magic;
	vfs->spk.n_1st_dbblock 		= src_super_blk->n_1st_dbblock;

	//-----------------------------------------------------------------
	// init partition 
	//-----------------------------------------------------------------
	part->part_start_sec		= LOG_SEC_START;					// 0x4EFF
	part->part_size_in_sec		= vfs->spk.num_blocks * SECTOR_PER_BLOCK;		// 0x2D050 = 0x5A0A * 8

	part->boot_start_sec		= part->part_start_sec;					// 0x4EFF
	part->boot_size_in_sec		= 1 * SECTOR_PER_BLOCK;					// 8

	part->spbk_start_sec		= part->boot_start_sec + part->boot_size_in_sec ;	// 0x4F07
	part->spbk_size_in_sec		= 1 * SECTOR_PER_BLOCK;					// 8

	part->imap_start_sec		= part->spbk_start_sec + part->spbk_size_in_sec;	// 0x4F0F
	part->imap_size_in_sec		= vfs->spk.num_imap_blocks * SECTOR_PER_BLOCK;		// 16 = 2 * 8

	part->bmap_start_sec		= part->imap_start_sec + part->imap_size_in_sec;	// 0x4F1F
	part->bmap_size_in_sec		= vfs->spk.num_bmap_blocks * SECTOR_PER_BLOCK;		// 48 = 6 * 8

	part->inode_start_sec		= part->bmap_start_sec + part->bmap_size_in_sec;	// 0x4F4F
	part->inode_size_in_sec		= vfs->spk.num_inode_blocks * SECTOR_PER_BLOCK;		// 4096 = 0x1000 = 0x200 * 8

	part->data_start_sec		= part->inode_start_sec + part->inode_size_in_sec;	// 0x5F4F
	part->data_size_in_sec		= part->part_size_in_sec - (part->boot_size_in_sec + part->spbk_size_in_sec + 
					  part->imap_size_in_sec + part->bmap_size_in_sec + 
					  part->inode_size_in_sec);				// 0x2CE46 = 0x2D050 - 0x20A

}



//===========================================================================
// vfs_test()
//===========================================================================
PUBLIC	void vfs_test()
{
}




//===========================================================================
// 0: sys_mkdir
//===========================================================================
PUBLIC	s32 sys_mkdir(s8 *dir_name)
{
	struct s_v_inode src_inode;

	s8	*str1 = "[err:dir not found(mkdir)]";
	s8	*str2 = "[err:dir/file existed(mkdir)]";


	u32	dev = DEV_DTS;

	//-----------------------------------------------------------------
	// fill dir[] and file[]
	//-----------------------------------------------------------------
	s8	dir[256];
	s8	file[DIR_ENT_NAME_LEN];
	s8	tmp_ent[DIR_ENT_NAME_LEN];

	s32 	x, y,z;
	s32	len = strlen(dir_name);

	for(x = len; x >= 0; x--)
	{
		if(dir_name[x] == '/')
		{
			break;
		}
	}

	for(y = 0; y <= x; y++)
	{
		dir[y] = dir_name[y];
	}
	dir[y] = '\0';

	for(z = 0, y = x + 1; y < len; y++, z++)
	{
		file[z] = dir_name[y];
	}
	file[z] = '\0';


	//-----------------------------------------------------------------
	// file exist ?
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	// dir exist ?
	//-----------------------------------------------------------------
	s32 	dir_inode_seq = k_fopen(dev, dir, &src_inode);

	if(dir_inode_seq == DIR_NOT_FOUND)
	{
		printk("%s",str1);
		printk("\n");
		return 0;
	}

	if(dir_inode_seq == ENTRY_EXIST)
	{		
	}

	s32	flg = src_inode.i_block_0;

	k_mkinode(flg, dir_name);
}

//===========================================================================
// 0: sys_mkfile
//===========================================================================
PUBLIC	s32 	sys_mkfile(s8 *dir_name)
{
	struct s_v_inode src_inode;

	s8	*str1 = "[err:dir not found(mkfile)]";
	s8	*str2 = "[err:dir/file existed(mkfile)]";

	u32	dev = DEV_DTS;

	//-----------------------------------------------------------------
	// fill dir[] and file[]
	//-----------------------------------------------------------------
	s8	dir[256];
	s8	file[DIR_ENT_NAME_LEN];
	s8	tmp_ent[DIR_ENT_NAME_LEN];

	s32 	x, y,z;
	s32	len = strlen(dir_name);

	for(x = len; x >= 0; x--)
	{
		if(dir_name[x] == '/')
		{
			break;
		}
	}

	for(y = 0; y <= x; y++)
	{
		dir[y] = dir_name[y];
	}
	dir[y] = '\0';

	for(z = 0, y = x + 1; y < len; y++, z++)
	{
		file[z] = dir_name[y];
	}
	file[z] = '\0';


	//-----------------------------------------------------------------
	// file exist ?
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	// dir exist ?
	//-----------------------------------------------------------------
	//s32 	dir_inode_seq = vfs->get_inode(dev, dir, &src_inode);
	s32 	dir_inode_seq = k_fopen(dev, dir, &src_inode);

	if(dir_inode_seq == DIR_NOT_FOUND)
	{
		printk("%s",str1);
		printk("\n");
		return 0;
	}

	if(dir_inode_seq == ENTRY_EXIST)
	{
	}

	s32	flg = src_inode.i_block_0;

	k_mkinode(flg, dir_name);
}

//===========================================================================
// 0: sys_rmdir
//===========================================================================
PUBLIC	s32 	sys_rmdir(s8 *dir_name)
{
	struct s_v_inode src_inode;

	s8	*str1 = "[err:dir not found(rmdir)]";
	s8	*str2 = "[err:dir/file existed(rmdir)]";


	u32	dev = DEV_DTS;

	//-----------------------------------------------------------------
	// fill dir[] and file[]
	//-----------------------------------------------------------------
	s8	dir[256];
	s8	file[DIR_ENT_NAME_LEN];
	s8	tmp_ent[DIR_ENT_NAME_LEN];

	s32 	x, y,z;
	s32	len = strlen(dir_name);

	for(x = len; x >= 0; x--)
	{
		if(dir_name[x] == '/')
		{
			break;
		}
	}

	for(y = 0; y <= x; y++)
	{
		dir[y] = dir_name[y];
	}
	dir[y] = '\0';

	for(z = 0, y = x + 1; y < len; y++, z++)
	{
		file[z] = dir_name[y];
	}
	file[z] = '\0';


	//-----------------------------------------------------------------
	// file exist ?
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	// dir exist ?
	//-----------------------------------------------------------------
	s32 	dir_inode_seq = k_fopen(dev, dir, &src_inode);

	if(dir_inode_seq == DIR_NOT_FOUND)
	{
		printk("%s",str1);
		printk("\n");
		return 0;
	}

	if(dir_inode_seq == ENTRY_EXIST)
	{		
	}

	s32	flg = src_inode.i_block_0;

	k_rminode(flg, dir_name);
}



//===========================================================================
// 0: sys_rmfile
//===========================================================================
PUBLIC	s32 	sys_rmfile(s8 *dir_name)
{
	struct s_v_inode src_inode;

	s8	*str1 = "[err:dir not found(rmfile)]";
	s8	*str2 = "[err:dir/file existed(rmfile)]";


	u32	dev = DEV_DTS;

	//-----------------------------------------------------------------
	// fill dir[] and file[]
	//-----------------------------------------------------------------
	s8	dir[256];
	s8	file[DIR_ENT_NAME_LEN];
	s8	tmp_ent[DIR_ENT_NAME_LEN];

	s32 	x, y,z;
	s32	len = strlen(dir_name);

	for(x = len; x >= 0; x--)
	{
		if(dir_name[x] == '/')
		{
			break;
		}
	}

	for(y = 0; y <= x; y++)
	{
		dir[y] = dir_name[y];
	}
	dir[y] = '\0';

	for(z = 0, y = x + 1; y < len; y++, z++)
	{
		file[z] = dir_name[y];
	}
	file[z] = '\0';


	//-----------------------------------------------------------------
	// file exist ?
	//-----------------------------------------------------------------
	
	//-----------------------------------------------------------------
	// dir exist ?
	//-----------------------------------------------------------------
	s32 	dir_inode_seq = k_fopen(dev, dir, &src_inode);

	if(dir_inode_seq == DIR_NOT_FOUND)
	{
		printk("%s",str1);
		printk("\n");
		return 0;
	}

	if(dir_inode_seq == ENTRY_EXIST)
	{		
	}

	s32	flg = src_inode.i_block_0;

	k_rminode(flg, dir_name);
}

//===========================================================================
// 0: sys_ls
//===========================================================================
PUBLIC	s32 	sys_ls(s8 *file_name, s8 mode)
{
	struct s_v_inode src_inode;

	s8	*str1 = "err:file not found(ls)";
	s8	*str2 = "err:dir/file existed(ls)";


	u32	dev = DEV_DTS;

	if(file_name == NULL)
	{
		strcopy(file_name, proc_ready->workdir);
	}	

	//-----------------------------------------------------------------
	// init
	//-----------------------------------------------------------------
	s32	len = strlen(file_name);

	file_name[len] = '/';
	file_name[len+1] = NULL;

	//-----------------------------------------------------------------
	// file exist ?
	//-----------------------------------------------------------------
	
	s32 	dir_inode_seq = k_fopen(dev, file_name, &src_inode);

	if(dir_inode_seq != ENTRY_EXIST)
	{		
		printk("%s,filename:%s\n",str1,file_name);

		printk("\n");
		return 0;
	}
	
	switch(mode)
	{
		case LIST_MODE_L:
			k_ls(dev, file_name, &src_inode);
			break;
		default:
			break;
	}	
}

//===========================================================================
// 0: sys_cat
//===========================================================================
PUBLIC	s32 sys_cat(s8 *file_name, s8 *data, s8 mode)
{
	struct s_v_inode src_inode;

	s8	*str1 = "[err:file not found(cat)]";
	s8	*str2 = "[err:dir/file existed(cat)]";


	u32	dev = DEV_DTS;

	//-----------------------------------------------------------------
	// init
	//-----------------------------------------------------------------
	s32	len = strlen(file_name);

	file_name[len] = '/';
	file_name[len+1] = NULL;

	//-----------------------------------------------------------------
	// file exist ?
	//-----------------------------------------------------------------
	
	s32 	dir_inode_seq = k_fopen(dev, file_name, &src_inode);

	if(dir_inode_seq != ENTRY_EXIST)
	{		
		printk("%s,filename:%s\n",str1,file_name);
		return 0;
	}
	
	switch(mode)
	{
		case FILE_MODE_R:
			k_fread(dev, &src_inode);			
			
			printk("%s\n",p_tmp_data);			
			break;
		case FILE_MODE_W:
			k_fwrite(dev, &src_inode, data);
			break;
		case FILE_MODE_A:
			k_fappend(dev, &src_inode, data);
			break;
		case FILE_MODE_X:
			break;
		case FILE_MODE_RW:
			break;
		case FILE_MODE_RX:
			break;
		case FILE_MODE_WX:
			break;
		case FILE_MODE_RWX:
			break;
		default:
			break;
	}
		
}

//===========================================================================
// 0: sys_read
//===========================================================================
PUBLIC	s32 sys_read(s8 *file_name, s8 mode)
{
	struct s_v_inode src_inode;

	s8	*str1 = "[err:file not found(read)]";
	s8	*str2 = "[err:dir/file existed(read)]";


	u32	dev = DEV_DTS;

	//-----------------------------------------------------------------
	// init
	//-----------------------------------------------------------------
	s32	len = strlen(file_name);

	file_name[len] = '/';
	file_name[len+1] = NULL;

	//-----------------------------------------------------------------
	// file exist ?
	//-----------------------------------------------------------------

	s32 	dir_inode_seq = k_fopen(dev, file_name, &src_inode);

	if(dir_inode_seq != ENTRY_EXIST)
	{	
		printk("%s,filename:%s\n",str1,file_name);
	}
	
	k_fread(dev, &src_inode);
		
}

//===========================================================================
// 0: sys_cd
//===========================================================================
PUBLIC	s32 	sys_cd(s8 *file_name)
{
	u32	dev = DEV_DTS;

	k_cd(dev, file_name);
	
	printk("\n");	
}

//===========================================================================
// 0: sys_cd
//===========================================================================
PUBLIC	s32 	sys_pwd(void)
{
	u32	dev = DEV_DTS;

	k_pwd(dev);
	
	printk("\n");	
}
