
#include "freedocore.h"
#include "video.h"
#include "frame.h"


int screen_width=0;
int screen_height=0;
int screen_bpp=0;
GLuint texture;
SDL_Surface *screen;
SDL_Surface *image; /*for OPENGL video*/
struct VDLFrame *frame;
extern _ext_Interface  fd_interface;

int initVideo(int w,int h, int bpp)
{

	frame=(VDLFrame *)malloc(sizeof(VDLFrame));

#ifdef USEGL


	videoInitGL(w,h,bpp);
	return 0;

#else


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
#endif

}

void toggleFullscreen()
{

	SDL_WM_ToggleFullScreen(screen);

}

SDL_Surface *videoInitGL(int width,int height,int bpp)
{



	Uint32 saved_flags;
	Uint8  saved_alpha;

	screen_width=width;
	screen_height=height;
	screen_bpp=bpp;

	if((SDL_Init( SDL_INIT_VIDEO )) < 0 )
	{
		printf("ERROR: can't start SDL VIDEO\n");
		SDL_Quit();
		return 0;
	}
	

	SDL_ShowCursor(0);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 1);

	printf("INFO: setting video mode:%dx%dx%d\n",width,height,bpp);
	if((screen = SDL_SetVideoMode(width,height,bpp,SDL_DOUBLEBUF | SDL_OPENGL)) < 0)
	{
		printf("ERROR: can't set video mode\n");
		SDL_Quit();
		return 0;
	}
	
	SDL_WM_SetCaption("3d'oh!",0);

	printf("INFO: video mode set\n");
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable( GL_TEXTURE_2D );

	glViewport( 0, 0, width, height);
	
	//glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

//	glMatrixMode(GL_PROJECTION);				
//	glPushMatrix();							
//	glLoadIdentity();						
	glOrtho(0, (GLdouble)screen_width, (GLdouble)screen_height, 0, -100000, 100000);				
	glMatrixMode(GL_MODELVIEW);					
//	glPushMatrix();							
//	glLoadIdentity();


	image = SDL_CreateRGBSurface(
			SDL_SWSURFACE,
			320, 240,
			32,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN /* OpenGL RGBA masks */
			0x000000FF, 
			0x0000FF00, 
			0x00FF0000, 
			0xFF000000
#else
			0xFF000000,
			0x00FF0000, 
			0x0000FF00, 
			0x000000FF
#endif
		       );
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	return image;
	
}


void videoFlipGL()
{

	SDL_GL_SwapBuffers();/*flipea la escena*/

}



void videoFlip()
{
struct BitmapCrop bmpcrop;
ScalingAlgorithm sca=None;
int rw,rh;

		fd_interface(FDP_DO_EXECFRAME,frame);
#ifdef USEGL
    	if(SDL_MUSTLOCK(image))SDL_LockSurface( image );
		Get_Frame_Bitmap((VDLFrame *)frame,image->pixels,image->w,&bmpcrop,320,240,0,0,0,sca,&rw,&rh);
		if(SDL_MUSTLOCK(image))SDL_UnlockSurface( image );


		videoDrawGL(loadTexture(image));
		videoFlipGL();
#else
    	if(SDL_MUSTLOCK(screen))SDL_LockSurface( screen );
		Get_Frame_Bitmap((VDLFrame *)frame,screen->pixels,screen->w,&bmpcrop,320,240,0,1,0,sca,&rw,&rh);
		if(SDL_MUSTLOCK(screen))SDL_UnlockSurface( screen );

		SDL_Flip(screen);
#endif

}

GLuint loadTexture(SDL_Surface *surface)
{


	Uint32 saved_flags;
	Uint8  saved_alpha;


	/* Create an OpenGL texture for the image */


	glTexImage2D(GL_TEXTURE_2D,
		     0,
		     GL_RGB,
		     surface->w, surface->h,
		     0,
		     GL_BGR,
		     GL_UNSIGNED_BYTE,
		     surface->pixels);
//	SDL_FreeSurface(image); /* No longer needed */

	return texture;


}

void videoDrawGL(GLuint texture)
{


	glPushMatrix();	

//	glEnable( GL_TEXTURE_2D );		

//	glTranslatef(screen_width/2 ,screen_height/2,0);
	glTranslatef(0,0,0);
//	glTranslatef(-screen_width/2,-screen_height/2,0);
		
//	glBindTexture(GL_TEXTURE_2D,texture);

	glBegin(GL_QUADS);

		glTexCoord2f(0,0);
		glVertex2f(0,0);    

		glTexCoord2f(1,0);
		glVertex2f(screen_width,0);

		glTexCoord2f(1,1);
		glVertex2f(screen_width,screen_height);
    
		glTexCoord2f(0,1);
		glVertex2f(0,screen_height);
	glEnd();


	glPopMatrix();

}
