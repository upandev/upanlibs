/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
 *
 *  I am making my contributions/submissions to this project solely in
 *  my personal capacity and am not conveying any rights to any
 *  intellectual property of any third parties.
 *                                                                          
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                          
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                          
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */
#ifndef _DRIVE_H_
#define _DRIVE_H_

#if defined __cplusplus
extern "C" {
#endif

typedef enum
{
	FS_UNKNOWN = 0x0,
	MOSFS = 0x83
} FS_TYPE ;

typedef enum
{
	DEV_FLOPPY = 0x0,
	DEV_CD,
	DEV_ATA_IDE = 0x80,
	DEV_ATAPI,
	DEV_SCSI_USB_DISK = 0x90
} DEVICE_TYPE ;

typedef enum
{
  FROM_FILE = -3,
  ROOT_DRIVE = -2,
  CURRENT_DRIVE = -1,
	FD_DRIVE0 = 0,
	FD_DRIVE1,
	FD_DRIVE2,
	FD_DRIVE3,
	CD_DRIVE0,
	CD_DRIVE1,
	HDD_DRIVE0,
	HDD_DRIVE1,
	USD_DRIVE0,
	USD_DRIVE1,
	USD_DRIVE2,
	USD_DRIVE3,
	USD_DRIVE4,
	USD_DRIVE5,
} DRIVE_NO ;

typedef struct 
{
	char			    driveName[33];
	byte			    bMounted;
	unsigned		  uiSizeInSectors ;
	unsigned long	ulTotalSize ;
	unsigned long	ulUsedSize ;
} DriveStat;

extern int SysDrive_ChangeDrive(const char* szDriveName) ;
extern int SysDrive_ShowDrives(DriveStat** pDriveList, int* iListSize) ;
extern int SysDrive_Mount(const char* szDriveName) ;
extern int SysDrive_UnMount(const char* szDriveName) ;
extern int SysDrive_Format(const char* szDriveName) ;
extern int SysDrive_GetCurrentDriveStat(DriveStat* pDriveStat) ;

#define chdrive(drive) SysDrive_ChangeDrive(drive)
#define get_drive_list(drive_list, list_size) SysDrive_ShowDrives(drive_list, list_size)
#define mount(drive) SysDrive_Mount(drive) 
#define umount(drive) SysDrive_UnMount(drive) 
#define format(drive) SysDrive_Format(drive) 
#define getcurdrive(pdrive) SysDrive_GetCurrentDriveStat(pdrive)

#if defined __cplusplus
}
#endif

#endif
