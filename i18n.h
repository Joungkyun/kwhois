#ifndef I18N_H
#define I18N_H

#include <config.h>

#define ENABLE_NLS

#ifndef HAVE_LOCALE_H
#undef ENABLE_NLS
#endif

#ifndef HAVE_LIBINTL_H
#undef ENABLE_NLS
#endif

#ifdef ENABLE_NLS
	#include <libintl.h>
	#include <locale.h>

	#define _(String)   gettext (String)

	void i18n_print (void);
#else
	/* This code follows GPL 2 License : Start */
	#define textdomain(domain) ((const char *) (domain))
	#define bindtextdomain(domain, dir) ((const char *) (dir))
	#define bind_textdomain_codeset(domain, charset) ((const char *) (charset))
	#define setlocale(lc, lctype) ((const char *) (lctype))
	#define _(String) (String)
	/* This code follows GPL 2 License : END */
#endif

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
