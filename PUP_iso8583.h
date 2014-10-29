#ifndef PACK_UNPACK_ISO
#define PACK_UNPACK_ISO

#include "PUP_isoCodec.h"

struct opx{
	void *src;
};

typedef struct opx PUP_HDL;

typedef unsigned char unhandled_field;

unhandled_field PUP_getUnpack(const uch *codec, const uch *packedFromHeader, int packedLen, PUP_HDL *hdl);
unhandled_field PUP_getUnpackNoHeader(const uch *codec, const uch *packedFromMTI, int packedLen, PUP_HDL *hdl);

unsigned char PUP_isFieldExist(PUP_HDL handle, int bitField);
unsigned char PUP_isPackedFieldExist(const uch *packedIso, int bitField);
int PUP_getFieldByteLen(PUP_HDL handle, int bitField);
unsigned char PUP_getFieldOK(PUP_HDL handle, int bitField, uch *outPut);
unsigned char PUP_getPackedFieldOK(const uch *packedIso, const uch *codec, int bitField, uch *outPut);

void PUP_endUnpack(PUP_HDL *handel);

int PUP_getIndexAfter(const uch *bitmap, int bitmapLen, int index);

#endif
