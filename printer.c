#include <stdio.h>
#include <string.h>

void print8L(char *buffer, int bufferLength)
{
	char LineCache[30];
	int i;
	char n;

	memset(LineCache,0x00,sizeof(LineCache));
	for(i = 0, n = 0; i<bufferLength; i++){
		sprintf(&LineCache[n*3], "%02X ", buffer[i]);
		if( n == 7 ){
			printf(LineCache);
			n = 0;
			memset(LineCache, 0x00, sizeof(LineCache));
		}
		else
			++n;
	}
	if (n != 0){
		strcat(LineCache, "\n");
		printf(LineCache);
	}
}

void printCode(char *buffer, int bufferLength){
	char buf[64], c;
	int i;
	FILE* hdl = fopen("CTR.txt","w");
	
	if(!hdl){
		printf("file could not opened");
		return;
	}
	
	for(i = 0, c = 0; i < bufferLength; ++i){
		sprintf(&buf[c * 4], "\\x%02X", buffer[i]);
		++c;
		if(c >= 7){
			printf(buf);
			fprintf(hdl, buf);
			memset(buf, 0, sizeof(buf));
			c = 0;
		}
	}
	if(c){
		strcat(buf, "\n");
		printf(buf);
		fprintf(hdl, buf);
	}
	fclose(hdl);
}
