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
//#include <SDL/SDL.h>
#include "freedocore.h"
#include "common.h"
#include "timer.h"


#ifdef DREAMCAST
#include "kos.h"
extern uint8 romdisk[];
KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);
KOS_INIT_ROMDISK(romdisk);
#endif


char* pNVRam;
extern _ext_Interface  io_interface;
_ext_Interface  fd_interface;
//SDL_Event event;
int onsector=0;
char biosFile[100];
char imageFile[100];

int __temporalfixes;
int HightResMode;
int __tex__scaler;

int count_samples=0;

int initEmu(int xres,int yres, int bpp, int armclock);

uint32 ReverseBytes(uint32 value)
{
			return (value & 0x000000FFU) << 24 | (value & 0x0000FF00U) << 8 |
				   (value & 0x00FF0000U) >> 8 | (value & 0xFF000000U) >> 24;
}


void readNvRam(void *pnvram)
{

	FILE* bios1;
	long fsize;
	char *buffer;
	NvRamStr *nvramStruct;

//	nvramStruct=(NvRamStr *)malloc(sizeof(NvRamStr));
	nvramStruct=(NvRamStr *)pnvram;
			////////////////
			// Fill out the volume header.
			nvramStruct->recordType = 0x01;
			int x;
			for (x = 0; x < 5; x++) nvramStruct->syncBytes[x] = (char)'Z';
			nvramStruct->recordVersion = 0x02;
			nvramStruct->flags = 0x00;
			for (x = 0; x < 32; x++) nvramStruct->comment[x] = 0;

			nvramStruct->label[0] = (char)'n';
			nvramStruct->label[1] = (char)'v';
			nvramStruct->label[2] = (char)'r';
			nvramStruct->label[3] = (char)'a';
			nvramStruct->label[4] = (char)'m';
			for (x = 5; x < 32; x++) nvramStruct->label[x] = 0;

			nvramStruct->id         = ReverseBytes(0xFFFFFFFF);
			nvramStruct->blockSize  = ReverseBytes(0x00000001); // Yep, one byte per block.
			nvramStruct->blockCount = ReverseBytes(0x00008000); // 32K worth of NVRAM data.

			nvramStruct->rootDirId        = ReverseBytes(0xFFFFFFFE);
			nvramStruct->rootDirBlocks    = ReverseBytes(0x00000000);
			nvramStruct->rootDirBlockSize = ReverseBytes(0x00000001);
			nvramStruct->lastRootDirCopy  = ReverseBytes(0x00000000);

			nvramStruct->rootDirCopies[0] = ReverseBytes(0x00000084);
			for (x = 1; x < 8; x++) nvramStruct->rootDirCopies[x] = 0;

			////////////////
			// After this point, I could not find the proper structure for the data.
			int w = sizeof(NvRamStr) / 4;

	/*		uint32_t nvramData 
			nvramData[w++] = ReverseBytes(0x855A02B6);
			nvramData[w++] = ReverseBytes(0x00000098);
			nvramData[w++] = ReverseBytes(0x00000098);
			nvramData[w++] = ReverseBytes(0x00000014);
			nvramData[w++] = ReverseBytes(0x00000014);
			nvramData[w++] = ReverseBytes(0x7AA565BD);
			nvramData[w++] = ReverseBytes(0x00000084);
			nvramData[w++] = ReverseBytes(0x00000084);
			nvramData[w++] = ReverseBytes(0x00007668); // This is blocks remaining.
			nvramData[w++] = ReverseBytes(0x00000014);
*/


//	buffer = (char*)malloc(32*1024);


//	pnvram=(void *)nvramStruct;

}




void loadRom1(void *prom)
{
	fsReadBios(biosFile, prom);

}


void *swapFrame(void *curr_frame)
{
//		printf("swap frame\n");
		return curr_frame;
	
}

void * emuinterface(int procedure, void *datum)
{
	typedef void *(*func_type)(void);
	void *therom;
	switch(procedure)
	{
	case EXT_READ_ROMS:
		loadRom1(datum);
		break;
	case EXT_READ2048:
	fsReadBlock(datum,onsector);
		break;
	case EXT_GET_DISC_SIZE:
		return (void *) (intptr_t) fsReadDiscSize();
		break;
	case EXT_ON_SECTOR:
		onsector=*((int*)&datum);
		break;
	case EXT_READ_NVRAM:
		readNvRam(datum);
		break;
	case EXT_WRITE_NVRAM:
		break;
	case EXT_PUSH_SAMPLE:
		soundFillBuffer(*((unsigned int*)&datum));
		count_samples++;
		break;
	case EXT_SWAPFRAME:
		return swapFrame(datum);
		break;
	case EXT_GETP_PBUSDATA:
		return (void *)inputRead();
		break;
	case EXT_GET_PBUSLEN:
		return (void *) (intptr_t) inputLength();
		break;
	case EXT_FRAMETRIGGER_MT:
		break;
	default:
	//	return _freedo_Interface(procedure,datum);
		break;
	};

return (void *)readNvRam;
}


void readConfiguration()
{

	configOpen("config.ini");
//	configReadInt("general","biosfile");
//	configReadInt("general","screenwidth");
//	configReadInt("general","screenheight");
	configClose();

}


int main(int argc, char *argv[])
{


	printf("3d'oh! MAIN\n");

	int biosset=0;
	int imageset=0;
	fsInit();
#ifdef DREAMCAST


	chdir("/rd");
#endif
	readConfiguration();	

#ifndef DREAMCAST
	if(argc>1){


		int i;
		for(i=0;i<argc;i++)
		{
//			printf("%s\n",argv[i]);   
			if(strcmp(argv[i],"-b")==0){
			/*set bios filename*/     
			sprintf(biosFile,"bios/%s",argv[i+1]);
			biosset=1;
			}
			if(strcmp(argv[i],"-i")==0){
			/*set image (iso) filename*/  
			sprintf(imageFile,"games/%s",argv[i+1]);
			imageset=1;
			}
		}
	}

	if((biosset)&&(imageset))
	{
		if(!initEmu(320,240,16,12500000)) return 0;


		/*free resources*/
		fd_interface(FDP_DESTROY,(void *)0);
		soundClose();
		fsClose();
//		SDL_Quit();
	}else{

		printf("3d'oh! - a 3do emulator\nUsage 3doemu -b biosfile -i isofile\n");

	}
#else
		sprintf(biosFile,"bios/bios.bin");
		sprintf(imageFile,"games/game.iso");
		initEmu(320,240,16,12500000);
		fd_interface(FDP_DESTROY,(void *)0);
		soundClose();
//		SDL_Quit();

#endif


	return 0;
}




///int main(int argc, char *argv[])
int initEmu(int xres,int yres, int bpp, int armclock)
{

	printf("INFO: starting 3d'oh! emulator\n");

	int arm_clock=armclock;
	int tex_quality=0;
	int quit=0;
	int waitfps;
	

//	memset(frame,0,sizeof(VDLFrame));
//	printf("frame %p\n",frame);
	io_interface=&emuinterface;
	fd_interface=&_freedo_Interface;
	videoInit(xres,yres,bpp);
//	toggleFullscreen(screen);
	soundInit();
	inputInit();

	if(!fsOpenIso(imageFile)) return 0;


//init freedo core

	void *prom;
	void *profile;



	fd_interface(FDP_SET_ARMCLOCK,(void *)(intptr_t)arm_clock);
	fd_interface(FDP_SET_TEXQUALITY,(void *)(intptr_t)tex_quality);
//	io_interface(FDP_INIT,0);
//	profile=io_interface(FDP_GETP_PROFILE,0);

	_3do_Init();

	int frame_end,framerate;
	int time_start=0;
	int frame_start=0;
	int frames=0;
	int debug=0;
	time_start = timerGettime();
	while(!quit){


		videoFlip();
		soundRun();

		quit = inputQuit();

		/* Framerate control */
		waitfps=timerGettime()-frame_end;
		if(waitfps<17)
		{
	
			SE_timer_waitframerate(17-waitfps);

		}

		frame_end=timerGettime();
		frames++;

		/* Calculate Frames Per Second */
		if((frame_end-time_start)>=1000) 
		{

			//printf("framerate:%d fps samples:%d\n",frames,count_samples);
			frames=0;
			count_samples=0;
			time_start = timerGettime();
			

		}

		

	}


	/* Close everything and return */
	videoClose();
	soundClose();
	inputClose();

#ifndef DREAMCAST

	SDL_Quit();

#endif

	return 1;
}
