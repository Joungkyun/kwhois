/* $Id: misc.c,v 1.1 2004-02-04 08:29:21 oops Exp $ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void strtolower (char *str) {
	int i = 0;
	int len = strlen (str);

	for ( i=0; i<len; i++ )
		memset (str + i, tolower (str[i]), 1);
}

unsigned char *hex2bin(unsigned char c) {
	static char h2b[5];
		    
	memset (h2b, '\0', sizeof (h2b));

	if((c >= 0x61 && c <= 0x7a) || (c >= 0x41 && c <= 0x5a)) {
		switch (c) { 
			case 'a' : strcpy (h2b, "1010"); break;
			case 'b' : strcpy (h2b, "1011"); break;
			case 'c' : strcpy (h2b, "1100"); break;
			case 'd' : strcpy (h2b, "1101"); break;
			case 'e' : strcpy (h2b, "1110"); break;
			case 'f' : strcpy (h2b, "1111"); break;
			case 'A' : strcpy (h2b, "1010"); break;
			case 'B' : strcpy (h2b, "1011"); break;
			case 'C' : strcpy (h2b, "1100"); break;
			case 'D' : strcpy (h2b, "1101"); break;
			case 'E' : strcpy (h2b, "1110"); break;
			case 'F' : strcpy (h2b, "1111"); break;
		}
	} else {
		switch (c) {
			case '0' : strcpy (h2b, "0000"); break;
			case '1' : strcpy (h2b, "0001"); break;
			case '2' : strcpy (h2b, "0010"); break;
			case '3' : strcpy (h2b, "0011"); break;
			case '4' : strcpy (h2b, "0100"); break;
			case '5' : strcpy (h2b, "0101"); break;
			case '6' : strcpy (h2b, "0110"); break;
			case '7' : strcpy (h2b, "0111"); break;
			case '8' : strcpy (h2b, "1000"); break;
			case '9' : strcpy (h2b, "1001"); break;
		}
	}

	return h2b;
}

unsigned int bin2dec(unsigned char *src) {
	int i, ret = 0;
	unsigned char var[2];

	for(i=0 ; i<8 ; i++) {
		sprintf(var, "%c", src[i]);
		ret += atoi(var) << (7 - i);
	}

	return ret;
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
