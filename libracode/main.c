/* $Id: main.c,v 1.1 2004-02-04 08:29:21 oops Exp $ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define __CINCLUDE__
#include "race.h"
#include "misc.h"

#include "i18n.h"

char *progs = "raceconv for libracode";
char *version = "0.0.1";

void usage (void);

int main (int argc, char *argv[]) {
	char readbuf[BUFSIZ];
	int opt, optlength, res = 0, debug = 0;

#ifdef ENABLE_NLS
	i18n_print();
#endif

	memset (readbuf, '\0', sizeof(readbuf));

	while ((opt = getopt(argc,argv, "auvh?")) != -1) {
		if (optarg != NULL)
			optlength = strlen(optarg);

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
	strcpy (readbuf, argv[optind]);

	//if ( ! res ) encode_race (readbuf);
	if ( ! res ) printf ("%s\n", encode_race (readbuf, debug));
	else printf ("%s\n", decode_race (readbuf, debug));

	return 0;
}

void usage (void) {
	fprintf (stderr, "%s %s\n", progs, version);
	fprintf (stderr, _("Usage: %s [OPTIONS...] convert_domain\n"), progs);
	fprintf (stderr, _("valid options:\n"));
	fprintf (stderr, _("    -h    help message (this message)\n"));
	fprintf (stderr, _("    -a    convert EUC-KR to Race code\n"));
	fprintf (stderr, _("    -u    convert Race code to Euc-kr\n"));
	fprintf (stderr, _("    -v    verbose mode\n"));
	exit (1);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
