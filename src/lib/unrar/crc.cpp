#include "rar.hpp"

uint CRCTab[256];

void InitCRC()
{
	for (int I=0;I<256;I++)
	{
		uint C=I;
		for (int J=0;J<8;J++)
			C=(C & 1) ? (C>>1)^0xEDB88320L : (C>>1);
		CRCTab[I]=C;
	}
}


uint CRC(uint StartCRC,const void *Addr,size_t Size)
{
	// Always initialized ahead of time, and this func call makes it a non-leaf func.
	if (false)
		if (CRCTab[1]==0)
			InitCRC();
	byte *Data=(byte *)Addr;
#if defined(LITTLE_ENDIAN) && defined(PRESENT_INT32) && defined(ALLOW_NOT_ALIGNED_INT)

#ifdef _MSC_VER
	// avoid a warning about 'Data' pointer truncation in 64 bit mode
	#pragma warning( disable : 4311 )
#endif

    while (Size>0 && ((long)Data & 7))
	{
		StartCRC=CRCTab[(byte)(StartCRC^Data[0])]^(StartCRC>>8);
		Size--;
		Data++;
	}
	while (Size>=8)
	{
		StartCRC^=*(uint32 *)Data;
		StartCRC=CRCTab[(byte)StartCRC]^(StartCRC>>8);
		StartCRC=CRCTab[(byte)StartCRC]^(StartCRC>>8);
		StartCRC=CRCTab[(byte)StartCRC]^(StartCRC>>8);
		StartCRC=CRCTab[(byte)StartCRC]^(StartCRC>>8);
		StartCRC^=*(uint32 *)(Data+4);
		StartCRC=CRCTab[(byte)StartCRC]^(StartCRC>>8);
		StartCRC=CRCTab[(byte)StartCRC]^(StartCRC>>8);
		StartCRC=CRCTab[(byte)StartCRC]^(StartCRC>>8);
		StartCRC=CRCTab[(byte)StartCRC]^(StartCRC>>8);
		Data+=8;
		Size-=8;
	}
#endif
	for (size_t I=0;I<Size;I++)
		StartCRC=CRCTab[(byte)(StartCRC^Data[I])]^(StartCRC>>8);
	return(StartCRC);
}

#ifndef SFX_MODULE
ushort OldCRC(ushort StartCRC,const void *Addr,size_t Size)
{
	byte *Data=(byte *)Addr;
	for (size_t I=0;I<Size;I++)
	{
		StartCRC=(StartCRC+Data[I])&0xffff;
		StartCRC=((StartCRC<<1)|(StartCRC>>15))&0xffff;
	}
	return(StartCRC);
}
#endif
