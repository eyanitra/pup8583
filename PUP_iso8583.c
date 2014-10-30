#include "PUP_iso8583.h"
#include "DSC_converter.h"

#ifndef _EFT30_
#	include <string.h>
#	include <stdlib.h>
#	define Z_MALLOX(a) 	malloc(a)
#	define Z_FREE(x)		free(x)
#else
#	include <SDK30.h>
#	define Z_MALLOX(a) 	umalloc(a)
#	define Z_FREE(x)		ufree(x)
#endif


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

static const uch * ptrVarField(HDL *hdl, const uch *data, int index)
{
	int i, sk;
	const uch *codec = hdl->codec;
	
	for(i = 2; i < index; i = PUP_getIndexAfter(hdl->bitmap, (hdl->extended)?16:8,i)){
		if(i == 0)
			return 0;
		while( i > PUP_getCodecIndex(codec))
			codec = PUP_ptrNextCodec(codec);
		if(i == PUP_getCodecIndex(codec)){
			sk = PUP_getExpectedValueOffset(codec, data);
			sk += PUP_getExpectedByteLen(codec, data);
			data += sk;
		}
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
			if(PUP_ptrCodecField(codec, i) == 0){
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

uch PUP_isFieldExist(PUP_HDL handle, int bitField)
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
		return PUP_getFixFieldByteLen(hdl->codec, bitField);
		
	fData = ptrVarField(hdl, hdl->pData, bitField);
	fCode = PUP_ptrCodecField(hdl->codec, bitField);
	return PUP_getExpectedByteLen(fCode,fData);
}

const uch* PUP_ptrFieldData(PUP_HDL handle, int bitField)
{
	int sk;
	const uch *fData, *fCode;
	
	HDL *hdl = (HDL *)handle.src;
	if(!PUP_isFieldExist(handle, bitField))
		return 0;
		
	fData = ptrVarField(hdl, hdl->pData, bitField);
	fCode = PUP_ptrCodecField(hdl->codec, bitField);
	sk = PUP_getExpectedValueOffset(fCode, fData);
	
	return fData + sk;
}

const uch* PUP_ptrField(PUP_HDL handle, int bitField)
{
	const uch *fData;
	HDL *hdl = (HDL *)handle.src;
	if(!PUP_isFieldExist(handle, bitField))
		return 0;
		
	fData = ptrVarField(hdl, hdl->pData, bitField);
	return fData;
}


void PUP_endUnpack(PUP_HDL *handel)
{
	HDL *hdl = handel->src;
	Z_FREE(hdl);
	handel->src = 0;
}
