/* $Id$ */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_LIBOGC

#include <olibc/libidn.h>

#define __CINCLUDE__
#include "i18n.h"

char *progs = "raceconv";

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

	if (argc - optind < 1 ) usage();
#if HAVE_LIBOC_VER == 0
	printf ("%s\n", (char *) convert_racecode ( argv[optind], res, debug ));
#endif

	return 0;
}

void usage (void) {
	fprintf (stderr, "%s %s\n", progs, PACKAGE_VERSION);
	fprintf (stderr, _("Usage: %s [OPTIONS...] convert_domain\n"), progs);
	fprintf (stderr, _("valid options:\n"));
	fprintf (stderr, _("    -h    help message (this message)\n"));
	fprintf (stderr, _("    -a    convert EUC-KR to Race code\n"));
	fprintf (stderr, _("    -u    convert Race code to EUC-KR\n"));
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
