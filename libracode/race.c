/* $Id: race.c,v 1.1 2004-02-04 08:29:21 oops Exp $ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "i18n.h"

#ifdef HAVE_ICONV_H
#include <iconv.h>

#include "race.h"
#include "misc.h"
#include "i18n.h"

char ansist[] = "[1;34m";
char ansien[] = "[7;0m";
int  unbuflen = 0;
char *extension[] = { "com", "net", "org" };

char * decode_race (char *domain, int debug) {
	char *tail = NULL, *gettail = NULL;
	char name[BUFSIZ];
	char dedomain[1024], backupstr[1024], uncompress[1024];
	int i = 0, len = 0;
	static char redomain[1024];

	len = strlen (domain);

	/* convert to low case */
	strtolower (domain);

	/* if don't start 'bq--', name is not race code */
	if ( strncmp (domain, RacePrefix, 4) ) {
		fprintf (stderr, _("ERROR: %s is not Race Code\n"), domain);
		exit (1);
	}

	memset (name, '\0', sizeof (name));
	strcpy (name, domain + 4);

	for ( i=len - 1; i>0; i-- ) {
		if ( name[i] == '.' ) name[i] = '\0';
		if ( name[i] & 0x80 ) {
			fprintf (stderr, _("ERROR: %s is not Race Code\n"), domain);
			exit (1);
		}
	}   

	/* get extension */
	gettail = rindex (domain, '.');
	tail = ( gettail != NULL ) ? strdup (gettail + 1) : NULL;

	if ( tail == NULL ) {
		fprintf (stderr, _("ERROR: Domain don't have extension\n"));
		exit (1);
	}

	if ( debug ) {
		fprintf (stderr, "\n");
		fprintf (stderr, "%s[ INPUT Date Check ]%s\n", ansist, ansien);
		fprintf (stderr, "%s-------------------------------------------%s\n", ansist, ansien);
		fprintf (stderr, _("Original Domain :     %s \n"), domain);
		fprintf (stderr, _("Convert  Target :     %s \n"), name);
		fprintf (stderr, _("Extension       :     %s \n"), tail);
		fprintf (stderr, "\n");
	}

	/* if local domain or don't com/net/org or singlebyte domain, don't convert */
	if ( strlen (tail) < 3 || ! permit_extension (tail) ) return domain;

	/* decoding base 32 */
	memset (backupstr, '\0', sizeof (backupstr));
	strcpy (backupstr, race_base32_decode (name));

	/* uncompress unicode hex */
	memset (uncompress, '\0', sizeof (uncompress));
	race_uncompress (uncompress, backupstr);

	/* convert utf-16be to euc-kr */
	string_convert (dedomain, uncompress, "UTF-16BE", "EUC-KR", debug);
	sprintf (redomain, "%s.%s", dedomain, tail);

	return redomain;
}

char * encode_race (char *domain, int debug) {
	char *tail = NULL, *gettail = NULL;
	char name[BUFSIZ];
	char utf16str[1024], backupstr[1024], compress[1024];
	int i = 0, yes = 0, len = 0, comlen = 0;
	static char endomain[1024];

	len = strlen (domain);

	/* convert to low case */
	strtolower (domain);

	memset (name, '\0', sizeof (name));
	strcpy (name, domain);

	for ( i=len - 1; i>0; i-- ) {
		if ( name[i] == '.' ) name[i] = '\0';
		if ( name[i] & 0x80 ) {
			yes = 1;
			break;
		}
	}

	/* get extension */
	gettail = rindex (domain, '.');
	tail = ( gettail != NULL ) ? strdup (gettail + 1) : NULL;

	if ( tail == NULL ) {
		fprintf (stderr, _("ERROR: Domain don't have extension\n"));
		exit (1);
	}

	if ( debug ) {
		fprintf (stderr, "\n");
		fprintf (stderr, "%s[ INPUT Date Check ]%s\n", ansist, ansien);
		fprintf (stderr, "%s-------------------------------------------%s\n", ansist, ansien);
		fprintf (stderr, _("Original Domain  :     %s \n"), domain);
		fprintf (stderr, _("Convert  Target  :     %s \n"), name);
		fprintf (stderr, _("Extension        :     %s \n"), tail);
		fprintf (stderr, "\n");
	}

	/* if local domain or don't com/net/org or singlebyte domain, don't convert */
	if ( ! yes || strlen (tail) < 3 || ! permit_extension (tail) ) return domain;

	memset (utf16str, '\0', sizeof (utf16str));
	memset (backupstr, '\0', sizeof (backupstr));

	strcpy (backupstr, name);
	len = utf16_length (backupstr);

	/* convert euc-kr to utf-16be */
	string_convert (utf16str, backupstr, "EUC-KR", "UTF-16BE", debug);

	/* compress unicode */
	memset (compress, '\0', sizeof (compress));
	strcpy (compress, race_compress (utf16str, len * 2));

	comlen = strlen (compress);

	if ( ! comlen ) {
		fprintf (stderr, _("ERROR: Race code compress failed\n"));
		exit (1);
	} else if ( comlen > 72 ) {
		fprintf (stderr, _("ERROR: Compress string is bigger than 72 charactors\n"));
		exit (1);
	}

	memset (endomain, '\0', sizeof (endomain));
	sprintf (endomain, "%s%s.%s", RacePrefix, race_base32_encode (compress), tail);

	return endomain;
}

int permit_extension (char *tail) {
	int i = 0;

	for ( i=0; i<3; i++) {
		if ( ! strncmp (extension[i], tail, 3) ) {
			return 1;
		}
	}

	return 0;
}

void string_convert (char *dest, char *src, char *from, char *to, int debug) {
	iconv_t cd;
	char * inbuf_p = src;
	char * outbuf_p = dest;
	char buf[9];

	size_t il;
	size_t ol;
	int err, have_error = 0, len;

    if ( strcmp (from, "UTF-16BE") ) len = strlen (src);
	else len = unbuflen * 2;

	il = len;
	ol = len * 3;

	if (src == NULL) {
		fprintf (stderr, _("ERROR: input data is nothing\n"));
		exit (1);
	}

	if ( ! strcmp (to, from) ) {
		fprintf (stderr, _("ERROR: original charactor set sames convert charactor set\n"));
		exit (1);
	}

	cd = iconv_open (to, from);

	if ( !cd ) {
		fprintf (stderr, _("ERROR: Can't not open iconv point\n"));
		exit (1);
	}

	err = iconv ( cd, &inbuf_p, &il, &outbuf_p, &ol );

	if (err == (size_t) - 1) {
		switch (errno) {
			case EINVAL:
				// Incomplete text, do not report an error
				break;
			case E2BIG:
				// There is not sufficient room at *outbuf
				have_error = 1;
				break;
			case EILSEQ:
				// An invalid multibyte sequence has been encountered in the input
				have_error = 1;
				break;
			default:
				have_error = 1;
			break;
		}
	}
	iconv_close (cd);

	if ( debug ) {
		int i, utflen;

		fprintf (stderr, _("%sstring_convert() debug%s\n"), ansist, ansien);
		fprintf (stderr, "%s-------------------------------------------%s\n", ansist, ansien);
		fprintf (stderr, _("Convert Method   :     %s from %s\n"), to, from);
		fprintf (stderr, _("Convert String   :     %s\nHex Data         :     "), src);
		if ( strcmp ( from, "UTF-16BE") ) utflen = utf16_length (src);
		else utflen = unbuflen;
		for (i = 0; i < utflen; i++) {
			sprintf (buf, "%02X", dest[i] & 0x000000ff);
			memset (buf + 2, '\0', 1);
				fprintf (stderr, "%s", buf);

			if ( (i % 2) == 1 ) fprintf (stderr, " ");
		}
		fprintf (stderr, "\n");
		fprintf (stderr, "Convert Length   :     %d\n", len);
		fprintf (stderr, "INPUT Remaining  :     %d\n", il);
		fprintf (stderr, "OUTPUT Remaining :     %d\n", ol);
		fprintf (stderr, "\n");
	}

	if (have_error == 1) {
		fprintf (stderr, _("ERROR: encoding error with iconv\n"));
		exit (1);
	}
}

void race_uncompress (char *ret, char *src) {
	int i = 0, len = 0, buflen = 0, retlen = 0;
	char tmp[9], buf[1024];

	memset (ret, '\0', sizeof (ret));
	memset (buf, '\0', sizeof (buf));

	len = strlen (src);
	for ( i=0; i<len; i+=8 ) {
		strncpy (tmp, src + i, 8);
		if ( strlen(tmp) < 8 ) continue;

		memset (buf + buflen, bin2dec (tmp), 1);
		buflen += 1;
	}

	if ( buf[0] == 0xffffffd8 ) {
		memcpy (ret, buf + 1, buflen);
	} else {
		i = 1;
		len = buflen;
		for ( i=1; i<len-1; i++ ) {
			if ( buf[i] == 0xffffffff ) {
				if ( buf[i+1] == 0xffffff99 ) {
					if ( (i+1) == len ) break;
					memset (ret + retlen, buf[0] & 0x000000ff, 1);
					memset (ret + retlen + 1, 0xff, 1);
				} else {
					memset (ret + retlen + 1, 0x00, 1);
					memset (ret + retlen, buf[i+1] & 0x000000ff, 1);
				}
				i++;
			} else {
				memset (ret + retlen, buf[0] & 0x000000ff, 1);
				memset (ret + retlen, buf[i] & 0x000000ff, 1);
			}
			retlen += 2;
		}
	}
	unbuflen = buflen - 1;
}

char * race_compress (char *src, int len) {
	int i = 0; // j = 0;
	int simple = 0, outlen = 0;
	static char outputstr[1024];
	char buf[BUFSIZ];

	memset (buf, '\0', sizeof (buf));
	memset (outputstr, '\0', sizeof (outputstr));

	/* for debugging code
	for ( i=0; i<len; i++ ) {
		//fprintf (stderr, "%d, %02x\n", i, src[i] << 24);
		fprintf (stderr, "%d, %02x\n", i, src[i] & 0x000000ff);
		sprintf (buf + j, "%02x", src[i] & 0x000000ff);
		j += 2;
	}   
	printf ("%s\n", buf);
	*/

	if ( ! race_check_same (src, len) ) {
		simple = race_check_simple (src, len);

		if ( simple != 0 )
			sprintf (outputstr, "%02x", simple & 0x000000ff);
		else {
			sprintf (outputstr, "%02x", 0xd8);
			outlen = strlen (outputstr);
			for ( i=0; i<len; i++ ) {
				sprintf (outputstr + outlen, "%02x", src[i] & 0x000000ff); 
				outlen = strlen (outputstr);
			}

			return outputstr;
		}
	} else {
		sprintf (outputstr, "%02x", src[0] & 0x000000ff);
	}
	outlen = strlen (outputstr);

	if ( src[0] >= 0xffffffd8 && src[0] <= 0xffffffdc )
		return "";

	for ( i=0; i<len; i+=2 ) {
		if ( src[i] == 0xffffff00 && src[i+1] == 0xffffff99 )
			return "";

		if ( src[i] == src[0] && src[i+1] != 0xffffffff) {
			sprintf (outputstr + outlen, "%02x", src[i+1] & 0x000000ff);
			outlen = strlen (outputstr);
		} else if ( src[i] == src[0] && src[i+1] == 0xffffffff) {
			sprintf (outputstr + outlen, "%02x%02x", src[i+1] & 0x000000ff, 0x99);
			outlen = strlen (outputstr);
		} else {
			sprintf (outputstr + outlen, "%02x%02x", 0xff, src[i+1] & 0x000000ff);
			outlen = strlen (outputstr);
		}
	}

	return outputstr;
}

int utf16_length (char *src) {
	int i = 0;
	static int ulen = 0;

	for ( i=0; i<strlen(src); i++ ) {
		if ( src[i] & 0x80 ) {
			i++;
		}

		ulen += 1;
	}

	return ulen;
}

int race_check_same (const char *src, int len) {
	int i = 0;
	static int same = 1;

	for ( i=0; i<len-1; i+=2 ) {
		if ((src[i] & 0x000000ff) != (src[i+2] & 0x000000ff)) {
			same = 0;
			break;
		}
	}

	return same;
}

int race_check_simple (const char *src, int len) {
	int i = 0;
	static int secondval = 0;

	for ( i=0; i<len-1; i+=2) {
		if ( secondval == 0 && src[i] != 0x0000)
			secondval = src[i];
		else if ( src[i] != secondval && src[i] != 0x0000 )
			return 0;
	}

	return secondval;
}

char * race_base32_encode (char *src) {
	int i = 0, j = 0, slen = 0, buflen = 0;
	int buf1len = 0, retlen = 0;
	char buf[1024], buf1[6];
	static char ret[1024];

	memset (ret, '\0', sizeof (ret));
	memset (buf, '\0', sizeof (buf));
	memset (buf1, '\0', sizeof (buf1));

	slen = strlen (src);
	for ( i=0; i<slen; i++ ) {
		strcpy ( buf + buflen, hex2bin (src[i]));
		buflen = strlen (buf);
	}

	for ( i=0; i<buflen; i+=5 ) {
		strncpy ( buf1, buf + i, 5);

		buf1len = strlen (buf1);
		if ( buf1len < 5 ) {
			slen = 5 - buf1len;

			for ( j=0; j<slen; j++ ) {
				strncpy (buf1 + buf1len, "0", 1);
				buf1len = strlen (buf1);
			}
		}

		memset (ret + retlen, en_base32 (buf1), 1);
		retlen = strlen (ret);
	}

	return ret;
}

char * race_base32_decode (char *src) {
	int i = 0, len = 0, retlen = 0;
	static char ret[1024];

	memset (ret, '\0', sizeof (ret));

	len = strlen (src);
	for ( i=0; i<len; i++ ) {
		strcpy (ret + retlen, de_base32 (src[i]));
		retlen = strlen (ret);
	}

	return ret;
}

char en_base32 (char *src) {
	if ( ! strcmp (src, "00000") ) return 'a';
	else if ( ! strcmp (src, "00001") ) return 'b';
	else if ( ! strcmp (src, "00010") ) return 'c';
	else if ( ! strcmp (src, "00011") ) return 'd';
	else if ( ! strcmp (src, "00100") ) return 'e';
	else if ( ! strcmp (src, "00101") ) return 'f';
	else if ( ! strcmp (src, "00110") ) return 'g';
	else if ( ! strcmp (src, "00111") ) return 'h';
	else if ( ! strcmp (src, "01000") ) return 'i';
	else if ( ! strcmp (src, "01001") ) return 'j';
	else if ( ! strcmp (src, "01010") ) return 'k';
	else if ( ! strcmp (src, "01011") ) return 'l';
	else if ( ! strcmp (src, "01100") ) return 'm';
	else if ( ! strcmp (src, "01101") ) return 'n';
	else if ( ! strcmp (src, "01110") ) return 'o';
	else if ( ! strcmp (src, "01111") ) return 'p';
	else if ( ! strcmp (src, "10000") ) return 'q';
	else if ( ! strcmp (src, "10001") ) return 'r';
	else if ( ! strcmp (src, "10010") ) return 's';
	else if ( ! strcmp (src, "10011") ) return 't';
	else if ( ! strcmp (src, "10100") ) return 'u';
	else if ( ! strcmp (src, "10101") ) return 'v';
	else if ( ! strcmp (src, "10110") ) return 'w';
	else if ( ! strcmp (src, "10111") ) return 'x';
	else if ( ! strcmp (src, "11000") ) return 'y';
	else if ( ! strcmp (src, "11001") ) return 'z';
	else if ( ! strcmp (src, "11010") ) return '2';
	else if ( ! strcmp (src, "11011") ) return '3';
	else if ( ! strcmp (src, "11100") ) return '4';
	else if ( ! strcmp (src, "11101") ) return '5';
	else if ( ! strcmp (src, "11110") ) return '6';
	else if ( ! strcmp (src, "11111") ) return '7';

	return 0;
}

char *de_base32 (char src) {
	if ( src == 'a' ) return "00000";
	else if ( src == 'b' ) return "00001";
	else if ( src == 'c' ) return "00010";
	else if ( src == 'd' ) return "00011";
	else if ( src == 'e' ) return "00100";
	else if ( src == 'f' ) return "00101";
	else if ( src == 'g' ) return "00110";
	else if ( src == 'h' ) return "00111";
	else if ( src == 'i' ) return "01000";
	else if ( src == 'j' ) return "01001";
	else if ( src == 'k' ) return "01010";
	else if ( src == 'l' ) return "01011";
	else if ( src == 'm' ) return "01100";
	else if ( src == 'n' ) return "01101";
	else if ( src == 'o' ) return "01110";
	else if ( src == 'p' ) return "01111";
	else if ( src == 'q' ) return "10000";
	else if ( src == 'r' ) return "10001";
	else if ( src == 's' ) return "10010";
	else if ( src == 't' ) return "10011";
	else if ( src == 'u' ) return "10100";
	else if ( src == 'v' ) return "10101";
	else if ( src == 'w' ) return "10110";
	else if ( src == 'x' ) return "10111";
	else if ( src == 'y' ) return "11000";
	else if ( src == 'z' ) return "11001";
	else if ( src == '2' ) return "11010";
	else if ( src == '3' ) return "11011";
	else if ( src == '4' ) return "11100";
	else if ( src == '5' ) return "11101";
	else if ( src == '6' ) return "11110";
	else if ( src == '7' ) return "11111";

	return "00000";
}

# else

char * encode_race (char *domain, int debug) {
	fprintf (stderr, _("ERROR: compiled without iconv\n"));
	exit (1);
}

char * decode_race (char *domain, int debug) {
	fprintf (stderr, _("ERROR: compiled without iconv\n"));
	exit (1);
}
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
