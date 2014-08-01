/*
    This file is part of 3d'oh, a multiplatform 3do emulator written by Gabriel Ernesto Cabral.

    3d'oh is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    3d'oh is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with 3d'oh.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"

FILE *fp;

int fsInit()
{
    kos_blockdev_t sd_dev;

    if(sd_init()) {
        printf("Could not initialize the SD card. Please make sure that you "
               "have an SD card adapter plugged in and an SD card inserted.\n");
        exit(EXIT_FAILURE);
    }

    if(fs_fat_init()) {
        printf("Could not initialize fs_fat!\n");
        exit(EXIT_FAILURE);
    }

	fs_fat_mount("/sd", &sd_dev, FS_FAT_MOUNT_READONLY);
	chdir("/sd");

	return 1;


}


void fsReadBios(char *biosFile, void *prom)
{

	FILE* bios1;
	long fsize;
	int readcount;

	printf("INFO: loading bios in %s\n",biosFile);
	bios1=fopen(biosFile,"rb");
	if (bios1==NULL)printf("ERROR: Bios load error\n");
	printf("INFO: Bios load success\n");
    fseek (bios1 , 0 , SEEK_END);
	fsize = ftell(bios1);
	rewind (bios1);

	readcount=fread(prom,1,fsize,bios1);
	fclose(bios1);

}



int fsClose()
{

    fs_fat_unmount("/sd");
    fs_fat_shutdown();
    sd_shutdown();

	return 1;

}


int fsOpenIso(char *path)
{

	fp=fopen(path,"rb");
	if(!fp) 
	{	
		printf("ERROR: can't load game file, exiting\n");
	return 0;

	}
	printf("INFO: game file loaded succesfully\n");
	return 1;
}

int fsCloseIso()
{

	fclose(fp);
	return 1;

}


int fsReadBlock(void *buffer,int sector)
{

    fseek (fp , 2048*sector , SEEK_SET);
	fread(buffer,1,2048,fp);
	rewind(fp);
//	printf("reading %d\n",sector*2048);
	return 1;

}

char *fsReadSize()
{
	char *buffer=(char *)malloc(sizeof(char)*4);
	rewind(fp);
    fseek (fp , 80 , SEEK_SET);
	fread(buffer,1,4,fp);
	rewind(fp);
	return buffer;

}

unsigned int fsReadDiscSize()
{
	unsigned int size;
	char sectorZero[2048];
	unsigned int temp;
	char *ssize;
	ssize=fsReadSize();

	memcpy(&temp,ssize,4);
	size=(temp & 0x000000FFU) << 24 | (temp & 0x0000FF00U) << 8 |
	   (temp & 0x00FF0000U) >> 8 | (temp & 0xFF000000U) >> 24;
	printf("INFO: disc size: %d sectors\n",size);
	return size;

}


