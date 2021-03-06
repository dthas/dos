//===========================================================================
// pci.c
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
#include	"stdio.h"
#include	"signal.h"
#include	"kernel.h"
#include	"global.h"
#include	"sys.h"
#include	"tty.h"
#include	"keyboard.h"
#include	"mesg.h"
#include	"dtsfs.h"
#include	"fs.h"
#include	"vfs.h"
#include	"hd.h"
#include	"partition.h"
#include	"proc.h"
#include	"pci.h"
#include	"prototype.h"

PRIVATE struct pci_device pdevice[PCI_BUS_MAX * PCI_DEVICE_MAX * PCI_FUNCTION_MAX];

//===========================================================================
// init pci
//===========================================================================
PUBLIC	void 	init_pci()
{
	pcidev = pdevice;		

	pci_scan(pcidev);

//------for test
	char *str = "-init pci-";
	disp_str(str);	
//------for test
	
}

//===========================================================================
// init pci
//===========================================================================
PUBLIC	void 	pci_scan(struct pci_device *pdev)
{
	s32 bus, dev, fun;

	for(bus=0; bus<PCI_BUS_MAX; bus++)
	{
		for(dev=0; dev<PCI_DEVICE_MAX; dev++)
		{
			for(fun=0; fun<PCI_FUNCTION_MAX; fun++)
			{
				u32 addr = PCI_BASE_ADDR + bus*0x10000 + dev*0x100 + fun * 8;

				out_32(PCI_CONFIG_ADDR, addr);

				u32 data = in_32(PCI_CONFIG_DATA);

				if(data != 0xFFFFFFFF)
				{
					pdev->device_id = data >> 16 ;
					pdev->vendor_id = data & 0x0000FFFF;

					out_32(PCI_CONFIG_ADDR, addr + 0x10);
					pdev->iobase    = in_32(PCI_CONFIG_DATA) & 0xFFFFFFFC;

					out_32(PCI_CONFIG_ADDR, addr + 0x3C);
					pdev->irq    = in_32(PCI_CONFIG_DATA) & 0xFF;

					pdev++;
				}
			}
		}
	}	
}


//===========================================================================
// init pci
//===========================================================================
PUBLIC	void 	pci_disp()
{
	struct pci_device *pdev = pcidev;

	printk("\ndevice id        vendor id          iobase         irq");
	printk("\n");
	printk("------------------------------------------------------");
	printk("\n");

	s32 i;
	s32 total = PCI_BUS_MAX * PCI_DEVICE_MAX * PCI_FUNCTION_MAX;
	for(i=0; i<total, pdev; i++, pdev++)
	{		
		if((pdev->vendor_id == PCI_VENDOR_ID_8029) &&(pdev->device_id == PCI_DEVICE_ID_8029))
		{
			printk("  %x,  %x,", pdev->device_id, pdev->vendor_id);
			printk("  %x,  %d", pdev->iobase, pdev->irq);
			printk("\n");
		}
	}
}
