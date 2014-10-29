#include "PUP_isoCodec.h"
#include "DSC_converter.h"

#include <string.h>
#include <stdlib.h>
#define Z_MALLOX(a) 	malloc(a)
#define Z_FREE(x)		free(x)


// getNextFieldOffset(const uch *codec)
// {return (int)(2 +(codec[1] & 0x3));};
// not a function since for optimation

// getFieldType(const uch *codec)
// {return (PUP_FTYPE)((codec[1] & 0xFC)>> 2);};

const uch *PUP_ptrCodecField(const uch *codec, int index)
{
	int sh = 0, cur = 0;
	do{
		codec += sh;
		cur = codec[0];
		sh = 2 + (codec[1] & 0x3); 
	}while(cur < index);
	
	if(cur == index)
		return &codec[0];
	return 0;
}

PUP_FTYPE PUP_getFieldType(const uch *codec, int index)
{
	int sh;
	
	if((index < 2)||(index > 128))
		return pup_error;
	codec = PUP_ptrCodecField(codec, index);
	if(codec){
		sh = codec[1] & 0xFC;
		return (PUP_FTYPE)(sh >>= 2);
	}
	return pup_error;
}

int PUP_getFixFieldByteLen(const uch *codec, int index)
{
	int sh, cur, len;
	
	if((index < 2)||(index > 128))
		return 0;
	
	codec = PUP_ptrCodecField(codec, index);
	if(codec){
		sh = codec[1] & 0x3;
		for(cur = 2, len = 0; sh > 0; --sh){
			len <<= 8;
			len += codec[cur++];
		}
	}
	
	return len;
}

int PUP_getExpectedValueOffset(const uch *flCodec, const uch *flData)
{
	PUP_FTYPE type;
	type = (PUP_FTYPE)((flCodec[1] & 0xFC)>> 2);
	
	switch(type)
	{
	case pup_fix:
		return 0;
	case pup_l2bcd:
	case pup_l3bcd:
		return 2;
	case pup_lbcd:
		return 1;
	default:
		break;
	}
	return 0;
};

int PUP_getExpectedByteLen(const uch *flCodec, const uch *flData)
{	
	int offset = 0, ext;
	ext = flCodec[1] & 0x3;
	
	switch((PUP_FTYPE)((flCodec[1] & 0xFC)>> 2))
	{
	case pup_fix:
		switch(ext)
		{
		case 3:
			BIN3_TO_INT_BE(offset, flCodec,2);
			break;
		case 2:
			BIN2_TO_INT_BE(offset, flCodec,2);
			break;
		case 1:
			offset = flCodec[2];
		default:
			break;
		}
	case pup_l3bcd:
		offset = dscBcdToBinary32(flData,4);
		break;
	case pup_l2bcd:
		offset = dscBcdToBinary32(&flData[1],2);
		break;
	case pup_lbcd:
		offset = dscBcdToBinary32(flData,2);
	default:
		break;
	};
	return offset;
};

