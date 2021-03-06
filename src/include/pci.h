//===========================================================================
// pci.h
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
#ifndef	_X_PCI_H_
#define	_X_PCI_H_

#define	PCI_BASE_ADDR		0x80000000	
#define	PCI_BUS_MAX		5
#define	PCI_DEVICE_MAX		32
#define	PCI_FUNCTION_MAX	8

#define	PCI_CONFIG_ADDR		0xCF8
#define	PCI_CONFIG_DATA		0xCFC

#define	PCI_DEVICE_ID_8029	0x8029
#define	PCI_VENDOR_ID_8029	0x10ec


struct pci_device
{
	u16 bus, dev, func;
	u16 vendor_id, device_id;
	u16 iobase;
	u8  irq;
};

struct pci_device *pcidev;

#endif
