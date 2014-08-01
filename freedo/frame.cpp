#include "freedoconfig.h"
#include "freedocore.h"
#include "frame.h"

#include "hqx.h"

unsigned char FIXED_CLUTR[32];
unsigned char FIXED_CLUTG[32];
unsigned char FIXED_CLUTB[32];

static void* tempBitmap;
static int currentAlgorithm;

void setCurrentAlgorithm(int algorithm);

void _frame_Init()
{
	tempBitmap = NULL;
	currentAlgorithm = 0;

	for(int j = 0; j < 32; j++)
	{
		FIXED_CLUTR[j] = (unsigned char)(((j & 0x1f) << 3) | ((j >> 2) & 7));
		FIXED_CLUTG[j] = FIXED_CLUTR[j];
		FIXED_CLUTB[j] = FIXED_CLUTR[j];
	}
}

extern "C"
{
/*void Get_Frame_Bitmap(
	VDLFrame* sourceFrame,
	void* destinationBitmap,
	int destinationBitmapWidth,
	BitmapCrop* bitmapCrop,
	int copyWidth,
	int copyHeight,
	bool addBlackBorder,
	bool copyPointlessAlphaByte,
	bool allowCrop,
	ScalingAlgorithm scalingAlgorithm,
	int* resultingWidth,
	int* resultingHeight)*/

void Get_Frame_Bitmap(
	VDLFrame* sourceFrame,
	void* destinationBitmap,
	int destinationBitmapWidth,
	int copyWidth,
	int copyHeight,
	int addBlackBorder,
	int copyPointlessAlphaByte,
	int allowCrop
	)
{
/*	setCurrentAlgorithm(0);

	float maxCropPercent = allowCrop ? .25f : 0;
	int maxCropTall = (int)(copyHeight * maxCropPercent);
	int maxCropWide = (int)(copyWidth * maxCropPercent);

//	printf("mx:%f\n",maxCropPercent);

/*	bitmapCrop->top = maxCropTall;
	bitmapCrop->left = maxCropWide;
	bitmapCrop->right = maxCropWide;
	bitmapCrop->bottom = maxCropTall;*/

//	int pointlessAlphaByte = copyPointlessAlphaByte ? 1 : 0;

	// Destination will be directly changed if there is no scaling algorithm.
	// Otherwise we extract to a temporary buffer.
	char* destPtr;
//	if (currentAlgorithm == 0)
		destPtr = (char*)destinationBitmap;
//	else
//		destPtr = (char*)tempBitmap;

//	VDLFrame* framePtr = sourceFrame;
	for (int line = 0; line < copyHeight; line++)
	{
//		VDLLine* linePtr = &framePtr->lines[line];
		VDLLine* linePtr = &sourceFrame->lines[line];
		short* srcPtr = (short*)linePtr;
		bool allowFixedClut = (linePtr->xOUTCONTROLL & 0x2000000) > 0;
		for (int pix = 0; pix < copyWidth; pix++)
		{
//			char bPart = 0;
//			char gPart = 0;
//			char rPart = 0;
			if (*srcPtr == 0)
			{
//				bPart = (char)(linePtr->xBACKGROUND & 0x1F);
//				gPart = (char)((linePtr->xBACKGROUND >> 5) & 0x1F);
//				rPart = (char)((linePtr->xBACKGROUND >> 10) & 0x1F);
				*destPtr++ = (char)(linePtr->xBACKGROUND & 0x1F);
				*destPtr++ = (char)((linePtr->xBACKGROUND >> 5) & 0x1F);
				*destPtr++ = (char)((linePtr->xBACKGROUND >> 10) & 0x1F);
			}
			else if (allowFixedClut && (*srcPtr & 0x8000) > 0)
			{
//				bPart = FIXED_CLUTB[(*srcPtr) & 0x1F];
//				gPart = FIXED_CLUTG[((*srcPtr) >> 5) & 0x1F];
//				rPart = FIXED_CLUTR[(*srcPtr) >> 10 & 0x1F];
				*destPtr++ = FIXED_CLUTB[(*srcPtr) & 0x1F];
				*destPtr++ = FIXED_CLUTG[((*srcPtr) >> 5) & 0x1F];
				*destPtr++ = FIXED_CLUTR[(*srcPtr) >> 10 & 0x1F];
			}
			else
			{
//				bPart = (char)(linePtr->xCLUTB[(*srcPtr) & 0x1F]);
//				gPart = linePtr->xCLUTG[((*srcPtr) >> 5) & 0x1F];
//				rPart = linePtr->xCLUTR[(*srcPtr) >> 10 & 0x1F];
				*destPtr++ = (char)(linePtr->xCLUTB[(*srcPtr) & 0x1F]);
				*destPtr++ = linePtr->xCLUTG[((*srcPtr) >> 5) & 0x1F];
				*destPtr++ = linePtr->xCLUTR[(*srcPtr) >> 10 & 0x1F];
			}
//			*destPtr++ = bPart;
//			*destPtr++ = gPart;
//			*destPtr++ = rPart;

//			printf("R:%d G:%d B:%d\n",rPart,gPart,bPart);

			destPtr += copyPointlessAlphaByte;
			srcPtr++;

/*			if (line < bitmapCrop->top)
				if (!(rPart < 0xF && gPart < 0xF && bPart < 0xF))
					bitmapCrop->top = line;

			if (pix < bitmapCrop->left )
				if (!(rPart < 0xF && gPart < 0xF && bPart < 0xF))
					bitmapCrop->left = pix;

			if (pix > copyWidth - bitmapCrop->right - 1)
				if (!(rPart < 0xF && gPart < 0xF && bPart < 0xF))
					bitmapCrop->right = copyWidth - pix - 1;

			if (line > copyHeight - bitmapCrop->bottom - 1)
				if (!(rPart < 0xF && gPart < 0xF && bPart < 0xF))
					bitmapCrop->bottom = copyHeight - line - 1;
*/
		}
	}

/*	int cropAdjust = 1;
	switch (currentAlgorithm)
	{
	case 0:
		// Nothing left to do
		break;
	case 1:
		hq2x_32((uint32_t*)tempBitmap, (uint32_t*)destinationBitmap, copyWidth, copyHeight);
		cropAdjust = 2;
		break;
	case 2:
		hq3x_32((uint32_t*)tempBitmap, (uint32_t*)destinationBitmap, copyWidth, copyHeight);
		cropAdjust = 3;
		break;
	case 3:
		hq4x_32((uint32_t*)tempBitmap, (uint32_t*)destinationBitmap, copyWidth, copyHeight);
		cropAdjust = 4;
		break;
	}

	bitmapCrop->top *= cropAdjust;
	bitmapCrop->left *= cropAdjust;
	bitmapCrop->right *= cropAdjust;
	bitmapCrop->bottom *= cropAdjust;

	*resultingWidth = copyWidth * cropAdjust;
	*resultingHeight = copyHeight * cropAdjust;*/
}
};

void setCurrentAlgorithm(int algorithm)
{
	//////////////////
	// De-initialize current (if necessary).
/*	if (
		((currentAlgorithm == 1)
		|| (currentAlgorithm == 2)
		|| (currentAlgorithm == 3))
		&& algorithm != 1
		&& algorithm != 2
		&& algorithm != 3 )
	{
		delete tempBitmap;
		hqxDestroy();
	}

	//////////////////
	// Initialize current (if necessary).

	if (
		((algorithm == 1)
		|| (algorithm == 2)
		|| (algorithm == 3))
		&& currentAlgorithm != 1
		&& currentAlgorithm != 2
		&& currentAlgorithm != 3)
	{
		hqxInit();
		tempBitmap = new unsigned char[1280*960*4];
	}

	// Accept new current algorithm.
	currentAlgorithm = algorithm;*/
}
