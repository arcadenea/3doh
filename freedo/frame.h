// Frame.h - For frame extraction and filtering options.

#ifndef	FRAME_3DO_HEADER
#define FRAME_3DO_HEADER

void _frame_Init();

#ifdef __cplusplus
extern "C"
{
#endif
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
	enum ScalingAlgorithm scalingAlgorithm,
	int* resultingWidth,
	int* resultingHeight);
*/
void Get_Frame_Bitmap(
	VDLFrame* sourceFrame,
	void* destinationBitmap,
	int destinationBitmapWidth,
	int copyWidth,
	int copyHeight,
	int addBlackBorder,
	int copyPointlessAlphaByte,
	int allowCrop
	);
#ifdef __cplusplus
};
#endif

#endif 
