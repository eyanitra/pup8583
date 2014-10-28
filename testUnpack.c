#include "PUP_iso8583.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

const char *typeTranslator(const uch *codec, int i)
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

void testUp_01(const uch *codec)
{
	int i;
	for(i = 2; i < 65; ++i)
		printf("bit field %d: %s\n",i,typeTranslator(codec,i));
}