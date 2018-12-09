#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "input.h"
#include "video.h"
#include "config.h"


SDL_Event eventjoy;
inputState internal_input_state;
int usekb=0;
SDL_Joystick *joystick[6];
int isexit=0;
int fullscreen=0;

inputMapping joystick0;


int inputMapButton(char *button)
{

char buttonr = "";

memcpy(&buttonr, button, sizeof(char);

	printf("%s\n",button);
	if(strcmp(buttonr,"JOY_BUTTON0")==0) return 0;
	if(strcmp(buttonr,"JOY_BUTTON1")==0) return 1;
	if(strcmp(buttonr,"JOY_BUTTON2")==0) return 2;
	if(strcmp(buttonr,"JOY_BUTTON3")==0) return 3;
	if(strcmp(buttonr,"JOY_BUTTON4")==0) return 4;
	if(strcmp(buttonr,"JOY_BUTTON5")==0) return 5;
	if(strcmp(buttonr,"JOY_BUTTON6")==0) return 6;
	if(strcmp(buttonr,"JOY_BUTTON7")==0) return 7;
	if(strcmp(buttonr,"JOY_BUTTON8")==0) return 8;
	if(strcmp(buttonr,"JOY_BUTTON9")==0) return 9;
	if(strcmp(buttonr,"JOY_BUTTON10")==0) return 10;

	return 0;
			

}


void inputReadConfig()
{

	configOpen("config.ini");

	joystick0.buttonup = inputMapButton(configReadString("joystick0","buttonup"));
	joystick0.buttondown = inputMapButton(configReadString("joystick0","buttondown"));
	joystick0.buttonleft = inputMapButton(configReadString("joystick0","buttonleft"));
	joystick0.buttonright = inputMapButton(configReadString("joystick0","buttonright"));
	joystick0.buttona = inputMapButton(configReadString("joystick0","buttona"));
	joystick0.buttonb = inputMapButton(configReadString("joystick0","buttonb"));
	joystick0.buttonc = inputMapButton(configReadString("joystick0","buttonc"));
	joystick0.buttonl = inputMapButton(configReadString("joystick0","buttonl"));
	joystick0.buttonr = inputMapButton(configReadString("joystick0","buttonr"));
	joystick0.buttonx = inputMapButton(configReadString("joystick0","buttonx"));
	joystick0.buttonp = inputMapButton(configReadString("joystick0","buttonp"));

	configClose();



}

int inputQuit()
{
	return isexit;
}

int inputFullscreen()
{
	return fullscreen;
}

int inputInit()
{

	int i=0;
	printf("INFO: reading input config\n");
	inputReadConfig();

	if((SDL_InitSubSystem( SDL_INIT_JOYSTICK )) < 0 )
	{
		printf("ERROR: can't init joystick subsystem\n");
		return 0;
	}

	printf("INFO: input found %d joysticks\n",inputEnum());
	if(inputEnum()==0){
		usekb=1;
	}

	for(i=0;i<inputEnum();i++)
	{
		joystick[i]=inputOpen(i);
	}
	return SDL_InitSubSystem(SDL_INIT_JOYSTICK);

}

SDL_Joystick *inputOpen(int joyid)
{

	SDL_Joystick *joystick;
	SDL_JoystickEventState(SDL_ENABLE);

	joystick = SDL_JoystickOpen(joyid);
	
	return joystick;

}

int inputEnum()
{
	
	if(usekb)return 1;
	else return SDL_NumJoysticks();
}


void inputPoll(SDL_Joystick *joy)
{

	//internal_input_state.buttons=0;
	while(SDL_PollEvent(&eventjoy))
	{
	    switch(eventjoy.type)
	    {

		case SDL_JOYAXISMOTION:
	        	if( eventjoy.jaxis.axis == 0){
		//		internal_input_state.axis1x=eventjoy.jaxis.value;
				if(eventjoy.jaxis.value < -3200){
					internal_input_state.buttons&=~INPUTBUTTONRIGHT;
					internal_input_state.buttons|=INPUTBUTTONLEFT;
				}else if(eventjoy.jaxis.value > 3200){
					internal_input_state.buttons&=~INPUTBUTTONLEFT;
					internal_input_state.buttons|=INPUTBUTTONRIGHT;
				}else{
					internal_input_state.buttons&=~INPUTBUTTONLEFT;
					internal_input_state.buttons&=~INPUTBUTTONRIGHT;
				}

			}
			if( eventjoy.jaxis.axis == 1){
		//		internal_input_state.axis1y=eventjoy.jaxis.value;
				if(eventjoy.jaxis.value < -3200){
					internal_input_state.buttons&=~INPUTBUTTONDOWN;
					internal_input_state.buttons|=INPUTBUTTONUP;
				}else if(eventjoy.jaxis.value > 3200){
					internal_input_state.buttons&=~INPUTBUTTONUP;
					internal_input_state.buttons|=INPUTBUTTONDOWN;
				}else{
					internal_input_state.buttons&=~INPUTBUTTONUP;
					internal_input_state.buttons&=~INPUTBUTTONDOWN;
				}
			}
		break;

		case SDL_JOYBUTTONDOWN:

			if(eventjoy.jbutton.button==joystick0.buttona)
			{
				internal_input_state.buttons|=INPUTBUTTONA;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonb)
			{
				internal_input_state.buttons|=INPUTBUTTONB;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonc)
			{
				internal_input_state.buttons|=INPUTBUTTONC;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonl)
			{
				internal_input_state.buttons|=INPUTBUTTONL;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonr)
			{
				internal_input_state.buttons|=INPUTBUTTONR;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonx)
			{
				internal_input_state.buttons|=INPUTBUTTONX;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonp)
			{
				internal_input_state.buttons|=INPUTBUTTONP;
			}

		break;

		case SDL_JOYBUTTONUP:

			if(eventjoy.jbutton.button==joystick0.buttona)
			{
				internal_input_state.buttons&=~INPUTBUTTONA;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonb)
			{
				internal_input_state.buttons&=~INPUTBUTTONB;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonc)
			{
				internal_input_state.buttons&=~INPUTBUTTONC;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonl)
			{
				internal_input_state.buttons&=~INPUTBUTTONL;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonr)
			{
				internal_input_state.buttons&=~INPUTBUTTONR;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonx)
			{
				internal_input_state.buttons&=~INPUTBUTTONX;
			}
			else if(eventjoy.jbutton.button==joystick0.buttonp)
			{
				internal_input_state.buttons&=~INPUTBUTTONP;
			}

		break;

		case SDL_QUIT:
			isexit=1;break;

		case SDL_KEYDOWN:
	                switch( eventjoy.key.keysym.sym ){
				case SDLK_LEFT:
					internal_input_state.buttons&=~INPUTBUTTONRIGHT;
					internal_input_state.buttons|=INPUTBUTTONLEFT;
						break;
				case SDLK_RIGHT:
					internal_input_state.buttons|=INPUTBUTTONRIGHT;
					internal_input_state.buttons&=~INPUTBUTTONLEFT;
						break;
				case SDLK_UP:
					internal_input_state.buttons|=INPUTBUTTONUP;
					internal_input_state.buttons&=~INPUTBUTTONDOWN;
						break;
				case SDLK_DOWN:
					internal_input_state.buttons&=~INPUTBUTTONUP;
					internal_input_state.buttons|=INPUTBUTTONDOWN;
						break;
				case SDLK_z:
			        	internal_input_state.buttons|=INPUTBUTTONC;
						break;
				case SDLK_x:
		        		internal_input_state.buttons|=INPUTBUTTONB;
						break;
				case SDLK_c:
		        		internal_input_state.buttons|=INPUTBUTTONA;
						break;
				case SDLK_v:
		        		internal_input_state.buttons|=INPUTBUTTONR;
						break;
				case SDLK_a:
		        		internal_input_state.buttons|=INPUTBUTTONX;
						break;
				case SDLK_s:
		        		internal_input_state.buttons|=INPUTBUTTONP;
						break;
				case SDLK_f:
		        		internal_input_state.buttons|=INPUTBUTTONL;
						break;
				default:
					break;
			}break;

		case SDL_KEYUP:
	                switch( eventjoy.key.keysym.sym ){
				case SDLK_LEFT:
					internal_input_state.buttons&=~INPUTBUTTONRIGHT;
					internal_input_state.buttons&=~INPUTBUTTONLEFT;
					break;
				case SDLK_RIGHT:
					internal_input_state.buttons&=~INPUTBUTTONRIGHT;
					internal_input_state.buttons&=~INPUTBUTTONLEFT;
					break;
				case SDLK_UP:
					internal_input_state.buttons&=~INPUTBUTTONUP;
					internal_input_state.buttons&=~INPUTBUTTONDOWN;
					break;
				case SDLK_DOWN:
					internal_input_state.buttons&=~INPUTBUTTONUP;
					internal_input_state.buttons&=~INPUTBUTTONDOWN;
					break;
				case SDLK_z:
			        	internal_input_state.buttons&=~INPUTBUTTONC;
						break;
				case SDLK_x:
		        		internal_input_state.buttons&=~INPUTBUTTONB;
						break;
				case SDLK_c:
		        		internal_input_state.buttons&=~INPUTBUTTONA;
						break;
				case SDLK_v:
		        		internal_input_state.buttons&=~INPUTBUTTONR;
						break;
				case SDLK_a:
		        		internal_input_state.buttons&=~INPUTBUTTONX;
						break;
				case SDLK_s:
		        		internal_input_state.buttons&=~INPUTBUTTONP;
						break;
				case SDLK_f:
		        		internal_input_state.buttons&=~INPUTBUTTONL;
						break;
				case SDLK_F11:
		        		fullscreen^=1;
						if(fullscreen)toggleFullscreen();
				case SDLK_ESCAPE:
		        		isexit=1;
						break;
				default:
					break;
			}break;

	    }
	}

}



int inputLength()
{

	return 16;

}

int CheckDownButton(int deviceNumber,int button)
{

	if(internal_input_state.buttons&button)return 1;
	else return 0;
	

}

char CalculateDeviceLowByte(int deviceNumber, int deviceCount)
{
	char returnValue = 0;

	if (deviceNumber >= deviceCount)
		return returnValue;

			returnValue |= 0x01 & 0; // unknown
			returnValue |= 0x02 & 0; // unknown
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONL) ? (char)0x04 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONR) ? (char)0x08 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONX) ? (char)0x10 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONP) ? (char)0x20 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONC) ? (char)0x40 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONB) ? (char)0x80 : (char)0;
//			printf("returnvalue %d\n",returnValue);
			return returnValue;
}

char CalculateDeviceHighByte(int deviceNumber, int deviceCount)
{
	char returnValue = 0;

	if (deviceNumber >= deviceCount)
		return returnValue;

			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONA)     ? (char)0x01 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONLEFT)  ? (char)0x02 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONRIGHT) ? (char)0x04 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONUP)    ? (char)0x08 : (char)0;
			returnValue |= CheckDownButton(deviceNumber, INPUTBUTTONDOWN)  ? (char)0x10 : (char)0;
			returnValue |= 0x20 & 0; // unknown
			returnValue |= 0x40 & 0; // unknown
			returnValue |= 0x80; // This last bit seems to indicate power and/or connectivity.
//			printf("returnvalue %d\n",returnValue);
			return returnValue;
}



unsigned char *inputRead()
{
	int i=0;
	int deviceCount=inputEnum()+1;
	
	for(i=0;i<inputEnum();i++)
	{
		inputPoll(joystick[i]);
	}

	unsigned char *data=(unsigned char *)malloc(sizeof(unsigned char)*16);
	data[0x0] = 0x00;
	data[0x1] = 0x48;
	data[0x2] = CalculateDeviceLowByte(1, deviceCount);
	data[0x3] = CalculateDeviceHighByte(1, deviceCount);
	data[0x4] = CalculateDeviceLowByte(3, deviceCount);
	data[0x5] = CalculateDeviceHighByte(3, deviceCount);
	data[0x6] = CalculateDeviceLowByte(2, deviceCount);
	data[0x7] = CalculateDeviceHighByte(2, deviceCount);
	data[0x8] = CalculateDeviceLowByte(5, deviceCount);
	data[0x9] = CalculateDeviceHighByte(5, deviceCount);
	data[0xA] = CalculateDeviceLowByte(4, deviceCount);
	data[0xB] = CalculateDeviceHighByte(4, deviceCount);
	data[0xC] = 0x00;
	data[0xD] = 0x80;
	data[0xE] = CalculateDeviceLowByte(6, deviceCount);
	data[0xF] = CalculateDeviceHighByte(6, deviceCount);

//	internal_input_state.buttons=0;

	return data;

}
