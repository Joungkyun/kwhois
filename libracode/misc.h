/* $Id: misc.h,v 1.1 2004-02-04 08:29:21 oops Exp $ */
#ifndef __MISC_H
#define __MISC_H

#if HAVE_CONFIG_H
#include "../config.h"
#endif

void strtolower (char *str);
unsigned char *hex2bin(unsigned char c);
unsigned int bin2dec(unsigned char *src);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
