
#include "freedocore.h"
#include "video.h"
#include "frame.h"
#include "kos.h"

#include <plx/texture.h>
#include <plx/context.h>
#include <plx/prim.h>


int screen_width=0;
int screen_height=0;
int screen_bpp=0;
int frame_counter=0;
kos_img_t	img;
unsigned short * temp_tex;

unsigned char FIXED_CLUTR[32];
uint8 dmabuffers[2][512 * 1024] __attribute__((aligned(32)));
//SDL_Surface *screen;
//SDL_Surface *image; /*for OPENGL video*/
VDLFrame *frame;
plx_texture_t *texture;

extern _ext_Interface  fd_interface;

pvr_init_params_t params = {
	/* Enable opaque and translucent polygons with size 16 */
	{ PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },
	
	/* Vertex buffer size 512K */
	512*1024,
	0
};

static inline void plx_vert_ifpm3(int flags, float x, float y, float z, uint32 color, float u, float v) {	
	plx_mat_tfip_3d(x, y, z);
	plx_vert_ifp(flags, x, y, z, color, u, v);
	//printf("%f %f %f\n",x,y,z);
}

int initVideo(int w,int h, int bpp)
{

	frame=(VDLFrame *)malloc(sizeof(VDLFrame));

	vid_set_mode(DM_640x480_VGA, PM_RGB565);
	if (pvr_init(&params) < 0)
		return -1;

//    pvr_set_vertbuf(PVR_LIST_OP_POLY, dmabuffers[0],512 * 1024);
//    pvr_set_vertbuf(PVR_LIST_TR_POLY, dmabuffers[1],512 * 1024);

	/* Init matrices */
	plx_mat3d_init();                    /* Clear internal to an identity matrix */
//	plx_mat3d_mode(PLX_MAT_PROJECTION);  /** Projection (frustum, screenview) matrix */
	plx_mat3d_identity();                /** Load an identity matrix */
//	plx_mat3d_perspective(45.0f, 640.0f / 480.0f, 100.0f, 0.1f);  // (float angle, float aspect, float znear, float zfar);
//	plx_mat3d_mode(PLX_MAT_MODELVIEW);   /** Modelview (rotate, scale) matrix */



	pvr_set_bg_color(1.0,1.0,1.0);

	plx_mat_identity();	    


	printf("INFO: video mode set\n");
	texture= plx_txr_canvas(512, 512,PVR_TXRFMT_RGB565);

	plx_cxt_init();
	plx_cxt_texture(texture);
	plx_cxt_culling(PLX_CULL_NONE);

//	temp_tex = (unsigned short *)malloc(512 * 512 * 2);
	temp_tex = memalign( 32, 512*512*2 );
	img.w=512;
	img.h=512;
	img.fmt=KOS_IMG_FMT_RGB565;
	img.byte_count=512*512*16;

	int j;
	for(j = 0; j < 32; j++)
	{
		FIXED_CLUTR[j] = (unsigned char)(((j & 0x1f) << 3) | ((j >> 2) & 7));
	}

	return 0;

}

void toggleFullscreen()
{

//	SDL_WM_ToggleFullScreen(screen);

}




void videoFlip()
{
BitmapCrop bmpcrop;
enum ScalingAlgorithm sca=None;
int rw,rh;

uint32 color;






		fd_interface(FDP_DO_EXECFRAME,frame);

		short *destPtr = (short*)temp_tex;
		int line;

		for (line = 0; line < 240;line++)
		{
		VDLLine* linePtr = &frame->lines[line];
		short* srcPtr = (short*)linePtr;

		int allowFixedClut = (linePtr->xOUTCONTROLL & 0x2000000) > 0;
		int pix;
			for (pix = 0; pix < 512; pix++)
			{
				if(pix<320){
					short bPart = 0;
					short gPart = 0;
					short rPart = 0;
					if (*srcPtr == 0)
					{
						bPart = (short)(linePtr->xBACKGROUND & 0x1F);
						gPart = (short)((linePtr->xBACKGROUND >> 5) & 0x1F);
						rPart = (short)((linePtr->xBACKGROUND >> 10) & 0x1F);

					}
					else if (allowFixedClut && (*srcPtr & 0x8000) > 0)
					{
						bPart = (short)FIXED_CLUTR[(*srcPtr) & 0x1F];
						gPart = (short)FIXED_CLUTR[((*srcPtr) >> 5) & 0x1F];
						rPart = (short)FIXED_CLUTR[(*srcPtr) >> 10 & 0x1F];

					}
					else
					{
						bPart = (short)(linePtr->xCLUTB[(*srcPtr) & 0x1F]);
						gPart = (short)linePtr->xCLUTG[((*srcPtr) >> 5) & 0x1F];
						rPart = (short)linePtr->xCLUTR[(*srcPtr) >> 10 & 0x1F];

					}
					//rrrrrGGG|GGGbbbbb
					*destPtr++=(short)(((rPart << 0x8)&0xF800) | (((gPart << 0x3))&0x7E0) | (bPart >>0x3));
//					*destPtr++=(short)(*srcPtr);
			
					srcPtr++;
				}else{
				//	srcPtr++;
					destPtr++;
				}
			}
		}
	
//		pvr_txr_load_kimg(&img.data, texture->ptr,PVR_TXRLOAD_16BPP);
	    pvr_txr_load_ex(temp_tex, texture->ptr, 512, 512, PVR_TXRLOAD_16BPP);
//	    pvr_txr_load(temp_tex, texture->ptr,512*512*2); //sin el twiddle
//		pvr_txr_load_dma(temp_tex, texture->ptr, 512*512*2, 0,0,0);


//		fd_interface(FDP_DO_EXECFRAME,texture->ptr);
		printf("INFO: rendering frame %d\n",frame_counter);
		frame_counter++;
		pvr_scene_begin();
		pvr_list_begin(PVR_LIST_OP_POLY);

		plx_cxt_send(PVR_LIST_OP_POLY);

		plx_vert_ifpm3(PLX_VERT, 0, 480.0f, 1, 0xffffffff, 0,240.0f/512.0f);
		plx_vert_ifpm3(PLX_VERT, 0, 0, 1, 0xffffffff, 0, 0);
		plx_vert_ifpm3(PLX_VERT, 640.0f, 480.0f, 1, 0xffffffff, 320.0f/512.0f, 240.0f/512.0f);
		plx_vert_ifpm3(PLX_VERT_EOS, 640.0f, 0, 1, 0xffffffff, 320.0f/512.0f, 0);


		pvr_list_finish();
		pvr_scene_finish();//flipea la escena

	//hack para que funcione el lxdream, con wait_ready debería ser suficiente en el hardware real
//		while(pvr_check_ready()!=0){

//		}
		pvr_wait_ready();



}

