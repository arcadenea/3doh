

#include <SDL/SDL.h>
#include "config.h"


static Uint8 *audio_buffer;
static Uint32 audio_len=0;
static Uint8 *audio_pos;
SDL_AudioSpec wanted;
int audio_pointer=0;
int audio_read=0;

int buffer_size=4096*4;
//int buffer_size=512;

void soundRun()
{



}


void fill_audio(void *udata, Uint8 *stream, int len)
{


    /* Only play if we have data left */
	if (( audio_len == 0 )){
//		audio_read=0;
		printf("INFO: no audio data for playing!\n");
		return;
	}


	/* Mix as much data as possible */
	len = ( len > audio_len ? audio_len : len );
//	printf("audiopointer %d audioread %d audio_len %d len %d\n",audio_pointer,audio_read,audio_len,len);
//	SDL_MixAudio(stream, audio_buffer, len, SDL_MIX_MAXVOLUME);

    /*protect against underruns*/
	if((audio_read+len>=audio_pointer)&&(audio_read<audio_pointer))
	{
		SDL_PauseAudio(1);
//		audio_pointer=0;
//		audio_read=0;
//		printf("INFO: audio underrun!\n");
		return;
	
	}
	memcpy(stream,audio_buffer+audio_read,len);

	audio_read+=len;

	if(audio_read>=buffer_size)
	{
		audio_read=0;
	}

	//audio_buffer-=len;

}


int soundInit()
{


#ifndef DREAMCAST
	if((SDL_InitSubSystem( SDL_INIT_AUDIO )) < 0 )
	{
		printf("ERROR: can't init sound\n");
		return 0;
	}
	printf("INFO: sound init success\n");

	/* Read sound config */
//	configOpen("config.ini");
//	buffer_size=atoi(configReadString("sound","soundbuffer"));
//	configClose();

	/* Set the audio format */
	wanted.freq = 44100;
	wanted.format = AUDIO_S16LSB;
	wanted.channels = 2;    /* 1 = mono, 2 = stereo */
	wanted.samples = 2048;  /* Good low-latency value for callback */
	wanted.callback = fill_audio;
	wanted.userdata = NULL;

	/* Open the audio device, forcing the desired format */
	if ( SDL_OpenAudio(&wanted, NULL) < 0 )
	printf("Couldn't open audio: %s\n", SDL_GetError());

	audio_buffer=(Uint8 *)malloc(sizeof(Uint8)*buffer_size);
	memset(audio_buffer,0,buffer_size);

	return 1;
#else
	return 1;
#endif

}




void soundFillBuffer(unsigned int dspLoop)
{



		unsigned int Rloop=dspLoop & 0x0000FFFF;
		unsigned int Lloop=(dspLoop & 0xFFFF0000)>>16;
//		printf("dspLoop %x %x %x\n",dspLoop,Lloop,Rloop);
	//	if(audio_len<131040){
		if(audio_pointer<=buffer_size){

			memcpy(audio_buffer+audio_pointer,&dspLoop,4);
			audio_pointer+=4;
//			audio_len+=4;
		

		}else{
			audio_pointer=0;
//			audio_read=0;


		}

		if(audio_pointer>buffer_size*1/5)
			SDL_PauseAudio(0);

//			printf("audio_pointer %d\n",audio_pointer);
		if(audio_pointer>=audio_read)
			audio_len=(audio_pointer-audio_read);
//			audio_len=0;
		else
			audio_len=((buffer_size-audio_read));

//		if(audio_len<500)audio_len=((buffer_size-audio_read*2)/2);

	

//	printf("audiopointer %d audioread %d\n",audio_pointer*32,audio_read);

}


void soundClose()
{

//    SDL_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

}




