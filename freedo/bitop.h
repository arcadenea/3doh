/*
  www.freedo.org
The first and only working 3DO multiplayer emulator.

The FreeDO licensed under modified GNU LGPL, with following notes:

*   The owners and original authors of the FreeDO have full right to develop closed source derivative work.
*   Any non-commercial uses of the FreeDO sources or any knowledge obtained by studying or reverse engineering
    of the sources, or any other material published by FreeDO have to be accompanied with full credits.
*   Any commercial uses of FreeDO sources or any knowledge obtained by studying or reverse engineering of the sources,
    or any other material published by FreeDO is strictly forbidden without owners approval.

The above notes are taking precedence over GNU LGPL in conflicting situations.

Project authors:

Alexander Troosh
Maxim Grishin
Allen Wright
John Sammons
Felix Lazarev
*/

#ifndef BITOPCLASS_DEFINITION_HEADER
#define BITOPCLASS_DEFINITION_HEADER

#include "types.h"

extern uint8 *getpram();

typedef struct sBitReaderBig
{	
	uint32 buf;
	uint32 point;
	int32 bitpoint;
	int32 bitset;
	uint8* pram;
} BitReaderBig;

	
void BitReaderBigInit(BitReaderBig *sbit);
	
void BitReaderBigInitBuff(BitReaderBig *sbit, uint32 buff);

void BitReaderBigAttachBuffer(BitReaderBig *sbit, uint32 buff);

void BitReaderBigSetBitRate(BitReaderBig *sbit, uint8 bits);
    
void BitReaderBigSetPosition(BitReaderBig *sbit, uint32 bytepos, uint8 bitpos);
	
void BitReaderBigSetPos(BitReaderBig *sbit, uint32 bitpos);

uint32 BitReaderBigGetBytePose(BitReaderBig *sbit);
    
uint32 BitReaderBigRead(BitReaderBig *sbit);
uint32 BitReaderBigRead8(BitReaderBig *sbit, uint8 bits);

void BitReaderBigSkip(BitReaderBig *sbit, uint32 bits);
	
	





#endif
