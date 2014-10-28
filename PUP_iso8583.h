#ifndef 	PACK_UNPACK_ISO
#define PACK_UNPACK_ISO

struct opx{
	void *src;
};

typedef struct opx PUP_HDL;
typedef unsigned char uch;
typedef unsigned char unhandled_field;

unhandled_field PUP_getUnpack(const uch *codec, const uch *packedFromHeader, int packedLen, PUP_HDL *hdl);
unhandled_field PUP_getUnpackNoHeader(const uch *codec, const uch *packedFromMTI, int packedLen, PUP_HDL *hdl);

unsigned char PUP_isFieldExist(PUP_HDL *handle, int bitField);
unsigned char PUP_isPackedFieldExist(const uch *packedIso, int bitField);
int PUP_getFieldByteLen(PUP_HDL *handle, int bitField);
unsigned char PUP_getFieldOK(PUP_HDL *handle, int bitField, uch *outPut);
unsigned char PUP_getPackedFieldOK(const uch *packedIso, const uch *codec, int bitField, uch *outPut);

/////////////////////

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

PUP_FTYPE PUP_fieldType(const uch *codec, int bfNumber);
int PUP_fixFieldLen(const uch *codec, int bfNumber);

#endif
