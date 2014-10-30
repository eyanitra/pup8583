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

uch PUP_isPackedFieldExist(const uch *packedIso, int bitField);
uch PUP_getPackedFieldOK(const uch *packedIso, const uch *codec, int bitField, uch *outPut);
int PUP_getIndexAfter(const uch *bitmap, int bitmapLen, int index);

uch PUP_isFieldExist(PUP_HDL handle, int bitField);
int PUP_getFieldByteLen(PUP_HDL handle, int bitField);
const uch* PUP_ptrField(PUP_HDL handle, int bitField);
const uch* PUP_ptrFieldData(PUP_HDL handle, int bitField);
const uch* PUP_ptrOriginalPacket(PUP_HDL *handle);
int PUP_getOriginalPacketLength(PUP_HDL *handle);


void PUP_endUnpack(PUP_HDL *handle);



#endif
