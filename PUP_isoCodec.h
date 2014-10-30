#ifndef CODEC_HANDLER_ISO
#define CODEC_HANDLER_ISO

#include "DSC_converter.h"

enum pup_dt{
	pup_fix = 0,
	pup_lbin = 1,
	pup_lascii = 2,
	pup_lbcd = 3,
	pup_l2bin = 4,
	pup_l2ascii = 5,
	pup_l2bcd = 6,
	pup_l3bin = 7,
	pup_l3ascii = 8,
	pup_l3bcd = 9,
	pup_encoded = 10,
	pup_pan = 11,
	pup_error,
};

typedef enum pup_dt PUP_FTYPE;

const uch *PUP_ptrCodecField(const uch *codec, int index);
PUP_FTYPE  PUP_getFieldType(const uch *codec, int index);
int        PUP_getFixFieldByteLen(const uch *codec, int index);
int        PUP_getExpectedValueOffset(const uch *flCodec, const uch *flData);
int 	   PUP_getExpectedByteLen(const uch *flCodec, const uch *flData);
const uch *PUP_ptrNextCodec(const uch *codec);
int		   PUP_getCodecIndex(const uch *codec);


#endif
