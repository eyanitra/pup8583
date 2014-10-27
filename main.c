#include "PUP_iso8583.h"
#include "DSC_converter.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define Z_MALLOX(a) 	malloc(a)

const char *typeTranslator(uch *codec, int i)
{
	PUP_FTYPE type;
	static char buff[30];
	memset(buff, 0, sizeof (buff));
	
	type = PUP_fieldType(codec, i);
	switch(type)
	{
	case pup_fix:
		sprintf(buff,"fixed %d", PUP_fixFieldLen(codec, i));
		return buff;
	case pup_l2ascii:
		sprintf(buff,"llvar ascii %s", PUP_fixFieldLen(codec, i)?"error":" ");
		return buff;
	case pup_l2bcd:
		sprintf(buff,"llvar bcd %s", PUP_fixFieldLen(codec, i)?"error":" ");
		return buff;
	case pup_l2bin:
		sprintf(buff,"llvar bin %s", PUP_fixFieldLen(codec, i)?"error":" ");
		return buff;
	case pup_l3ascii:
		sprintf(buff,"lllvar ascii %s", PUP_fixFieldLen(codec, i)?"error":" ");
		return buff;
	case pup_l3bcd:
		sprintf(buff,"lllvar bcd %s", PUP_fixFieldLen(codec, i)?"error":" ");
		return buff;
	case pup_l3bin:
		sprintf(buff,"lllvar bin %s", PUP_fixFieldLen(codec, i)?"error":" ");
		return buff;
	default :
		return "error!";
	}
}

extern void print8L(char *buffer, int bufferLength);

int main(int arg, char** argc)
{
	// all fix except 48 type 9 -- lll_bcd
	unsigned char sCon []= "020109""030103""040106""070105""0B0103""0C0103""0D0102""0F0102"\
						   "120102""25010C""270102""290108""2A010F""3024""400108";
	int sLen, i;
	uch *codec;
	
	sLen = strlen((char*)sCon);
	sLen >>= 1;
	codec = Z_MALLOX(sLen);
	if(codec)
		dscHexStrToBinary(sCon,codec,sLen);
		
	for(i = 2; i < 65; ++i)
		printf("bit field %d: %s\n",i,typeTranslator(codec,i));

	printf("test\n");
	print8L((char*)codec, sLen);
	return 0;
}