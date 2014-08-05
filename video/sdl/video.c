
#include "freedocore.h"
#include "video.h"
#include "frame.h"


int screen_width=0;
int screen_height=0;
int screen_bpp=0;
SDL_Surface *screen;
SDL_Surface *image; /*for OPENGL video*/
VDLFrame *frame;
extern _ext_Interface  fd_interface;

int videoInit(int w,int h, int bpp)
{

	frame=(VDLFrame *)malloc(sizeof(VDLFrame));


	if((SDL_Init( SDL_INIT_VIDEO )) < 0 )
	{
		printf("ERROR: can't init video\n");
		return 0;
	}

	if((screen = SDL_SetVideoMode(320,240,32,SDL_SWSURFACE|SDL_DOUBLEBUF)) < 0)
	{
		printf("ERROR: can't set video mode\n");
		return 0;
	}

	printf("INFO: video mode set\n");
	return 0;

}

void toggleFullscreen()
{

	SDL_WM_ToggleFullScreen(screen);

}




void videoFlip()
{
BitmapCrop bmpcrop;
enum ScalingAlgorithm sca=None;
int rw,rh;

		fd_interface(FDP_DO_EXECFRAME,frame);

    	if(SDL_MUSTLOCK(screen))SDL_LockSurface( screen );
//		Get_Frame_Bitmap((VDLFrame *)frame,screen->pixels,screen->w,&bmpcrop,320,240,0,1,0,sca,&rw,&rh);
		Get_Frame_Bitmap((VDLFrame *)frame,image->pixels,image->w,320,240,0,0,0);
		if(SDL_MUSTLOCK(screen))SDL_UnlockSurface( screen );

		SDL_Flip(screen);

}

int videoClose()
{

}