/* $Id: race.h,v 1.1 2004-02-04 08:29:21 oops Exp $ */
#ifndef __RACE_H
#define __RACE_H

#ifndef __CINCLUDE__
	#if HAVE_CONFIG_H
	#include <config.h>
	#endif
	/* Define as const if the declaration of iconv() needs const. */
	#define ICONV_CONST
	#define RacePrefix	"bq--"
	#define COLOR		34
#endif

char * encode_race (char *domain, int debug);
char * decode_race (char *domain, int debug);
int permit_extension (char *tail);
void string_convert (char *dest, char *src, char *from, char *to, int debug);
char * race_compress (char *src, int len);
void race_uncompress (char *ret, char *src);
int utf16_length (char *src);
int race_check_same (const char *src, int len);
int race_check_simple (const char *src, int len);
char * race_base32_encode (char *src);
char * race_base32_decode (char *src);
char en_base32 (char *src);
char * de_base32 (char src);
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
