#include "PUP_iso8583.h"
#include "DSC_converter.h"
#include <string.h>
#include <stdlib.h>

// todo must define default codec

struct hdl_t{
	uch tpdu;
	const uch *codec;
	const uch *head;
	const uch *bitmap;
	const uch *pData;
};

typedef struct hdl_t HDL;

#define Z_MALLOX(a) 	malloc(a)

PUP_FTYPE PUP_fieldType(const uch *codec, int index)
{
	int cur, sh;
	
	if((index < 2)||(index > 128))
		return pup_error;
	sh = 0;
	cur = 0;
	do{
		codec += sh;
		cur = codec[0];
		sh = 2 + (codec[1] & 0x3);
	}while(cur < index);
	
	if(cur == index){
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
	
	sh = 0;
	cur = 0;
	len = 0;
	do{
		codec += sh;
		cur = codec[0];
		sh = 2 + (codec[1] & 0x3);
	}while(cur < index);
	
	if(cur == index){
		sh -= 2;
		for(cur = 2; sh > 0; --sh){
			len <<= 8;
			len += codec[cur++];
		}
	}
	
	return len;
}


uch PUP_isFieldDefined(const uch *codec, int index)
{
	return 0;
}

uch PUP_isBitmapExist(const uch *bitmap, int index)
{
	int ctrIndex;
	unsigned char ctrBitValue;
	if((index > 128 )||(index < 1))
		return 0;

	if(((bitmap[0] & 0x80) == 0) && index > 64)
		return 0;

	index--;
	ctrIndex = index / 8;
	ctrBitValue = 0x80 >> (index % 8);

	if((bitmap[ctrIndex] & ctrBitValue) == ctrBitValue)
		return 1;
	return 0;
}

PUP_HDL PUP_getUnpack(const uch *codec, const uch *pack, int pLen)
{
	HDL *hdl;
	PUP_HDL out;
	
	out.src = 0;
	if((!pack)||(pack[0] != 0x60)||(!codec))
		return out;

	hdl = (HDL*)Z_MALLOX(sizeof(HDL));
	hdl->head = pack;
	hdl->bitmap = &pack[7];
	hdl->codec = codec;
	if(PUP_isBitmapExist(hdl->bitmap,1))
		hdl->pData = &pack[23];
	else
		hdl->pData = &pack[15];
	hdl->tpdu = 1;
	
	out.src = hdl;
	return out;
}

