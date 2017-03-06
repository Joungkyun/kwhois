/*
 *  Copyright (C) 2017 JoungKyun.Kim <http://oops.org>
 *
 *  This file is part of kwhois.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef HAVE_LIBOGC
#include <olibc/libidn.h>

/* support i18n */
#include "../i18n.h"

char *progs = "punyconv";

void usage (void);

int main (int argc, char *argv[]) {
	int opt;
#if HAVE_LIBOC_VER == 0
	int res = 0, debug = 0;
#endif

#ifdef ENABLE_NLS
	i18n_print();
#endif

#if HAVE_LIBOC_VER == 0
	while ( (opt = getopt (argc,argv, "auvh?")) != -1 )
#else
	while ( (opt = getopt (argc,argv, "h?")) != -1 )
#endif
	{	
		switch (opt) {
#if HAVE_LIBOC_VER == 0
			case 'a' :
				res = 0;
				break;
			case 'u' :
				res = 1;
				break;
			case 'v' :
				debug = 1;
				break;
#endif
			default :
				usage();
		}
	}

	if ( argc - optind < 1 )
		usage();

#if HAVE_LIBOC_VER == 1
	printf ("%s\n", (char *) convert_punycode (argv[optind], NULL)) ;
#else
	printf ("%s\n", (char *) convert_punycode (argv[optind], res, debug)) ;
#endif

	return 0;
}

void usage (void) {
	fprintf (stderr, "%s %s\n", progs, PACKAGE_VERSION);
	fprintf (stderr, _("Usage: %s [OPTIONS...] convert_domain\n"), progs);
	fprintf (stderr, _("valid options:\n"));
	fprintf (stderr, _("    -h    help message (this message)\n"));
#if HAVE_LIBOC_VER == 0
	fprintf (stderr, _("    -a    convert EUC-KR to Punycode\n"));
	fprintf (stderr, _("    -u    convert punycode to EUC-KR\n"));
	fprintf (stderr, _("    -v    verbose mode\n"));
#endif

	exit (1);
}

#else
int main (void) {
	fprintf (stderr, "Can't support olibc library\n");

	return 1;
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
