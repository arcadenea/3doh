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

#include "freedoconfig.h"
#include "bitop.h"
#include "arm.h"


uint32 BitReaderBigRead(BitReaderBig *sbit)
{
const static uint8 mas[]={0,1,3,7,15,31,63,127,255};
uint32 retval=0;
int32 bitcnt=sbit->bitset;
 	if(!sbit->buf)return retval;
 	if((8-sbit->bitpoint) > sbit->bitset)
 	{
 		retval=memread8(sbit->buf+(sbit->point^3));

 		retval>>=8-sbit->bitpoint-sbit->bitset;
 		retval&=mas[sbit->bitset];
 		sbit->bitpoint+=sbit->bitset;
 		return retval;
 	}
 	if(sbit->bitpoint)
 	{
 		retval=memread8(sbit->buf+(sbit->point^3))&mas[8-sbit->bitpoint];
		sbit->point++;
 		bitcnt-=8-sbit->bitpoint;
 	}
 	while(bitcnt>=8)
 	{
		retval<<=8;
 		retval|=memread8(sbit->buf+(sbit->point^3));

        sbit->point++;
 		bitcnt-=8;
 	}
 	if(bitcnt)
 	{
		retval<<=bitcnt;
 		retval|=memread8(sbit->buf+(sbit->point^3))>>(8-bitcnt); 		
	
 	}
 	sbit->bitpoint=bitcnt;

 	return retval; 	 	
}

uint32 BitReaderBigRead8(BitReaderBig *sbit, uint8 bits)
{
	BitReaderBigSetBitRate(sbit, bits);
	return BitReaderBigRead(sbit);
}


void BitReaderBigInit(BitReaderBig *sbit)
{
    sbit->buf=0;
    sbit->bitset=1;
    sbit->point=0;
	sbit->bitpoint=0;
	sbit->pram=getpram();
}
	
	
void BitReaderBigInitBuff(BitReaderBig *sbit, uint32 buff)
{
	sbit->buf=buff;
	sbit->point=0;
	sbit->bitpoint=0;
	sbit->bitset=1;
	sbit->pram=getpram();
}

	
void BitReaderBigAttachBuffer(BitReaderBig *sbit, uint32 buff)
{
    sbit->buf=buff;
	sbit->point=0;
	sbit->bitpoint=0;
}

	
void BitReaderBigSetBitRate(BitReaderBig *sbit, uint8 bits)
{
	sbit->bitset=bits;
    if(sbit->bitset>32) sbit->bitset=32;
    if(!sbit->bitset) sbit->bitset=1;
}
    
	
void BitReaderBigSetPosition(BitReaderBig *sbit, uint32 bytepos, uint8 bitpos)
{
	sbit->point=bytepos;
	sbit->bitpoint=bitpos;
}

	
void BitReaderBigSetPos(BitReaderBig *sbit, uint32 bitpos)
{
	BitReaderBigSetPosition(sbit, bitpos>>3, bitpos&7);
}


uint32 BitReaderBigGetBytePose(BitReaderBig *sbit)
{
return sbit->point;
}
    


void BitReaderBigSkip(BitReaderBig *sbit, uint32 bits)
{
	bits+=sbit->bitpoint;
	sbit->point+=(bits>>3);
	sbit->bitpoint=bits&7;
}

