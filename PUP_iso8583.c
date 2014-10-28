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

static const uch *ptrCodecField(const uch *codec, int index)
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

static uch PUP_isBitmapExist(const uch *bitmap, int index)
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

PUP_FTYPE PUP_fieldType(const uch *codec, int index)
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

unsigned char PUP_isFieldExist(PUP_HDL *handle, int bitField)
{
	HDL *hdl = (HDL *)handle->src;
	
	if((hdl == 0)||(bitField < 0)||(bitField > 128))
		return 0;
		
	if((bitField > 64)&& (hdl->extended == 0))
		return 0;
	
	return PUP_isBitmapExist(hdl->bitmap,bitField);
}

//////// code this /////
static int getSkipByte(PUP_FTYPE ty, const uch *field){ return 0;};
static int getByteLen(PUP_FTYPE ty, const uch *field){ return 0;};
static const uch * ptrFieldDataValue(PUP_FTYPE ty, const uch *field){return 0;};

int PUP_getFieldByteLen(PUP_HDL *handle, int bitField)
{
	int i, sk;
	const uch *run;
	
	HDL *hdl = (HDL *)handle->src;
	if(!PUP_isFieldExist(handle, bitField))
		return 0;
	if(PUP_fieldType(hdl->codec, bitField) == pup_fix)
		return PUP_fixFieldLen(hdl->codec, bitField);
	// otherwise enumerate
	for(i = 2, run = hdl->pData; i < bitField ; ++i){
		if(PUP_isBitmapExist(hdl->bitmap,i))
		sk = getSkipByte(PUP_fieldType(hdl->codec,i), run);
		run += sk;
	}
	return getByteLen(PUP_fieldType(hdl->codec,i), run);
}

unsigned char PUP_getFieldOK(PUP_HDL *handle, int bitField, uch *outPut)
{
	int i, sk, len;
	const uch *run;
	
	HDL *hdl = (HDL *)handle->src;
	if(!PUP_isFieldExist(handle, bitField))
		return 0;
	if(PUP_fieldType(hdl->codec, bitField) == pup_fix)
		return PUP_fixFieldLen(hdl->codec, bitField);
	// otherwise enumerate
	for(i = 2, run = hdl->pData; i < bitField ; ++i){
		if(PUP_isBitmapExist(hdl->bitmap,i))
		sk = getSkipByte(PUP_fieldType(hdl->codec,i), run);
		run += sk;
	}
	len = getByteLen(PUP_fieldType(hdl->codec,i), run);
	run = ptrFieldDataValue(PUP_fieldType(hdl->codec,i), run);
	memcpy(outPut, run, len);
	return 1;
}

