#include "PUP_iso8583.h"
#include "DSC_converter.h"
#include <string.h>
#include <stdlib.h>

// todo must define default codec

struct hdl_t{
	uch tpdu;
	uch extended;
	const uch *codec;
	const uch *head;
	const uch *bitmap;
	const uch *pData;
};

typedef struct hdl_t HDL;

#define Z_MALLOX(a) 	malloc(a)
#define Z_FREE(x)		free(x)

// getNextFieldOffset(const uch *codec)
// {return (int)(2 +(codec[1] & 0x3));};
// not a function since for optimation

// getFieldType(const uch *codec)
// {return (PUP_FTYPE)((codec[1] & 0xFC)>> 2);};

const uch *ptrCodecField(const uch *codec, int index)
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

uch PUP_isBitmapExist(const uch *bitmap, int index)
{
	int ctrIndex;
	unsigned char ctrBitValue;

	if(index < 1)
		return 0;

	index--;
	ctrIndex = index / 8;
	ctrBitValue = 0x80 >> (index % 8);

	if((bitmap[ctrIndex] & ctrBitValue) == ctrBitValue)
		return 1;
	return 0;
}

int PUP_getIndexAfter(const uch *bitmap, int bitmapLen, int index)
{
	int aId;
	uch mask;
	if(index < 0)
		return 0;

	while(1){
		mask = 0x80 >>(index & 0x7);
		aId = index >> 3;
		if(aId >= bitmapLen)
			break;
		++index;
		if((bitmap[aId] & mask) == mask)
			return index;
	};
	
	return 0;
}

PUP_FTYPE PUP_getFieldType(const uch *codec, int index)
{
	int sh;
	
	if((index < 2)||(index > 128))
		return pup_error;
	codec = ptrCodecField(codec, index);
	if(codec){
		sh = codec[1] & 0xFC;
		return (PUP_FTYPE)(sh >>= 2);
	}
	return pup_error;
}

int PUP_fixFieldLen(const uch *codec, int index)
{
	int sh, cur, len;
	
	if((index < 2)||(index > 128))
		return 0;
	
	codec = ptrCodecField(codec, index);
	if(codec){
		sh = codec[1] & 0x3;
		for(cur = 2, len = 0; sh > 0; --sh){
			len <<= 8;
			len += codec[cur++];
		}
	}
	
	return len;
}

//////////////////////////

static int getFieldDataOffset(const uch *flCodec, const uch *flData)
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

static int getFieldByteLen(const uch *flCodec, const uch *flData)
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

static const uch * ptrVarField(HDL *hdl, const uch *data, int index)
{
	int i, sk;
	const uch *codec = hdl->codec;
	
	for(i = 2; (i < index)&&(i); i = PUP_getIndexAfter(hdl->bitmap, (hdl->extended)?16:8,i)){
		while( i > (int)(codec[0]))
			codec += (codec[1] & 0x3)+2;
		if(i < (int)(codec[0]))
			return 0;
		sk = getFieldDataOffset(codec, data) + getFieldByteLen(codec, data);
		data += sk;
	}
	return data;
}

//////////////////////////


unhandled_field PUP_getUnpack(const uch *codec, const uch *pack, int pLen, PUP_HDL *out)
{
	HDL *hdl;
	int i, max;
	
	out->src = 0;
	if((!pack)||(pack[0] != 0x60)||(!codec))
		return 1;

	hdl = (HDL*)Z_MALLOX(sizeof(HDL));
	hdl->bitmap = &pack[7];
	if(PUP_isBitmapExist(hdl->bitmap,1)){
		hdl->pData = &pack[23];
		max = 128;
		hdl->extended = 1;
	}
	else{
		hdl->pData = &pack[15];
		max = 64;
		hdl->extended = 0;
	}
		
	for(i = 2; i <= max; ++i){
		if(PUP_isBitmapExist(hdl->bitmap, i))
			if(ptrCodecField(codec, i) == 0){
				Z_FREE(hdl);
				return i;
			}
	}
	
	hdl->tpdu = 1;
	hdl->codec = codec;
	hdl->head = pack;
	out->src = hdl;
	return 0;
}

unsigned char PUP_isFieldExist(PUP_HDL handle, int bitField)
{
	HDL *hdl = (HDL *)handle.src;
	
	if((hdl == 0)||(bitField < 0)||(bitField > 128))
		return 0;
		
	if((bitField > 64)&& (hdl->extended == 0))
		return 0;
	
	return PUP_isBitmapExist(hdl->bitmap,bitField);
}

int PUP_getFieldByteLen(PUP_HDL handle, int bitField)
{
	const uch *fData, *fCode;
	
	HDL *hdl = (HDL *)handle.src;
	if(!PUP_isFieldExist(handle, bitField))
		return 0;
	if(PUP_getFieldType(hdl->codec, bitField) == pup_fix)
		return PUP_fixFieldLen(hdl->codec, bitField);
		
	fData = ptrVarField(hdl, hdl->pData, bitField);
	fCode = ptrCodecField(hdl->codec, bitField);
	return getFieldByteLen(fCode,fData);
}

unsigned char PUP_getFieldOK(PUP_HDL handle, int bitField, uch *output)
{
	int sk, len;
	const uch *fData, *fCode;
	
	HDL *hdl = (HDL *)handle.src;
	if(!PUP_isFieldExist(handle, bitField))
		return 0;
		
	fData = ptrVarField(hdl, hdl->pData, bitField);
	fCode = ptrCodecField(hdl->codec, bitField);
	len = getFieldByteLen(fCode, fData);
	sk = getFieldDataOffset(fCode, fData);
	
	memcpy(output,fData + sk, len);
	
	return 1;
}

void PUP_endUnpack(PUP_HDL *handel)
{
	HDL *hdl = handel->src;
	Z_FREE(hdl);
	handel->src = 0;
}
