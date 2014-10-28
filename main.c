#include "PUP_iso8583.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>




extern void testUp_01();
extern void testUp_02();



int main(int arg, char** argc)
{
	printf("test\n");
	
	testUp_01();
	testUp_02();
	
	return 0;
}