#include "i18n.h"

#ifdef ENABLE_NLS

void i18n_print(void)
{
	setlocale (LC_CTYPE, "");
	setlocale (LC_MESSAGES, "");
	bindtextdomain("kwhois", LANGDIR);
	textdomain("kwhois");
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
