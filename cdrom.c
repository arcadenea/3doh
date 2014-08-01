#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//#include <libisofs/libisofs.h>


//IsoDataSource *src;
FILE *fcdrom;
uint32_t lba=0;


int cdromOpenIso(char *path)
{

	fcdrom=fopen(path,"rb");
	if(!fcdrom) 
	{	
		printf("ERROR: can't load game file, exiting\n");
	return 0;

	}
	printf("INFO: game file loaded succesfully\n");
	return 1;
}

int cdromCloseIso()
{

	fclose(fcdrom);
	return 1;

}


int cdromReadBlock(void *buffer,int sector)
{

    fseek (fcdrom , 2048*sector , SEEK_SET);
	fread(buffer,1,2048,fcdrom);
	rewind(fcdrom);
//	printf("reading %d\n",sector*2048);
	return 1;

}

char *cdromReadSize()
{
	char *buffer=(char *)malloc(sizeof(char)*4);
	rewind(fcdrom);
    fseek (fcdrom , 80 , SEEK_SET);
	fread(buffer,1,4,fcdrom);
	rewind(fcdrom);
	return buffer;

}

unsigned int cdromDiscSize()
{
	unsigned int size;
	char sectorZero[2048];
	unsigned int temp;
	char *ssize;
	ssize=cdromReadSize();
/*			fixed (byte* sectorBytePointer = sectorZero)
			{
				var sectorPointer = new IntPtr((int)sectorBytePointer);
				this.ReadSector(sectorPointer, 0);
				this.sectorCount = EmulationHelper.GetSectorCount(sectorPointer);
			}*/
	memcpy(&temp,ssize,4);
	size=(temp & 0x000000FFU) << 24 | (temp & 0x0000FF00U) << 8 |
	   (temp & 0x00FF0000U) >> 8 | (temp & 0xFF000000U) >> 24;
	printf("INFO: disc size: %d sectors\n",size);
	return size;

}


