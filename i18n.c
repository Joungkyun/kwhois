#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "i18n.h"

#ifdef ENABLE_NLS

void i18n_print(void) { // {{{
	setlocale (LC_CTYPE, "");
	setlocale (LC_MESSAGES, "");
	bindtextdomain ("kwhois", LANGDIR);
	textdomain ("kwhois");
} // }}}
#endif

/*
 * If LANG is ko_KR.eucKR, return 1 else return 0
 */
int current_charset (void) {
	char	* lang_env = getenv ("LANG");
	char 	* lcharset = strrchr (lang_env + 1, '.');

	if ( lcharset == NULL )
		return 0;

	if ( ! strncasecmp ("ko_KR.euc", lang_env, 9) )
		return 1;

	return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
