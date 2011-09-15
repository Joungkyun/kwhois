/* idn.c	Command line interface to the library
 * Copyright (C) 2003  Simon Josefsson
 *
 * This file is part of GNU Libidn.
 *
 * GNU Libidn is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNU Libidn is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Libidn; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
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
	int opt, res = 0, debug = 0;

#ifdef ENABLE_NLS
	i18n_print();
#endif

	while ((opt = getopt(argc,argv, "auvh?")) != -1) {
		switch (opt) {
			case 'a' :
				res = 0;
				break;
			case 'u' :
				res = 1;
				break;
			case 'v' :
				debug = 1;
				break;
			default :
				usage();
		}
	}

	if ( argc - optind < 1 )
		usage();

	printf ("%s\n", (char *) convert_punycode ( argv[optind], res, debug )) ;

	return 0;
}

void usage (void) {
	fprintf (stderr, "%s %s\n", progs, PACKAGE_VERSION);
	fprintf (stderr, _("Usage: %s [OPTIONS...] convert_domain\n"), progs);
	fprintf (stderr, _("valid options:\n"));
	fprintf (stderr, _("    -h    help message (this message)\n"));
	fprintf (stderr, _("    -a    convert EUC-KR to Punycode\n"));
	fprintf (stderr, _("    -u    convert punycode to EUC-KR\n"));
	fprintf (stderr, _("    -v    verbose mode\n"));

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
