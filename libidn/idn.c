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
 * $Id: idn.c,v 1.1 2004-02-04 08:29:21 oops Exp $
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "stringprep.h"
#include "punycode.h"
#include "idna.h"

/* support i18n */
#include "../i18n.h"

char *progs = "punyconv";
char *version = "0.2.2";

void usage (void);

int
main (int argc, char *argv[])
{
  char readbuf[BUFSIZ];
  char *p, *r;
  uint32_t *q;
  int rc, opt, optlenth, res = 0, debug = 0;

#ifdef ENABLE_NLS
  i18n_print();
#endif

  memset (readbuf, '\0', sizeof(readbuf));

  while ((opt = getopt(argc,argv, "auvh?")) != -1) {
	  if (optarg != NULL)
		  optlenth = strlen(optarg);

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

  if (argc - optind < 1 ) {
	  usage();
  }

  strcpy (readbuf, argv[optind]);

      if (readbuf[strlen (readbuf) - 1] == '\n')
	readbuf[strlen (readbuf) - 1] = '\0';

      if (res == 0)
	{
	  p = stringprep_locale_to_utf8 (readbuf);
	  if (!p)
	    {
	      fprintf (stderr, _("%s: could not convert from %s to UTF-8.\n"),
		       readbuf, stringprep_locale_charset ());
	      return 1;
	    }

	  q = stringprep_utf8_to_ucs4 (p, -1, NULL);
	  if (!q)
	    {
	      free (p);
	      fprintf (stderr, _("%s: could not convert from UCS-4 to UTF-8.\n"),
		       readbuf);
	      return 1;
	    }

	  if ( debug == 1 )
	    {
	      size_t i;
	      for (i = 0; q[i]; i++)
		fprintf (stderr, "input[%d] = U+%04x\n", i, q[i] & 0xFFFF);
	    }

	  rc = idna_to_ascii_4z (q, &r, 0);
	  free (q);
	  if (rc != IDNA_SUCCESS)
	    {
	      fprintf (stderr, _("%s: idna_to_ascii_from_locale() failed with error %d.\n"), readbuf, rc);
	      return 1;
	    }
	  fprintf (stdout, "%s\n", r);

	  free (r);
	}

      if (res == 1)
	{
	  p = stringprep_locale_to_utf8 (readbuf);
	  if (!p)
	    {
	      fprintf (stderr, _("%s: could not convert from %s to UTF-8.\n"),
		       readbuf, stringprep_locale_charset ());
	      return 1;
	    }

	  q = stringprep_utf8_to_ucs4 (p, -1, NULL);
	  if (!q)
	    {
	      free (p);
	      fprintf (stderr, _("%s: could not convert from UCS-4 to UTF-8.\n"),
		       readbuf);
	      return 1;
	    }

	  rc = idna_to_unicode_8z4z (p, &q, 0|0);
	  free (p);
	  if (rc != IDNA_SUCCESS)
	    {
	      fprintf (stderr, _("%s: idna_to_unicode_locale_from_locale() failed with error %d.\n"), readbuf, rc);
	      return 1;
	    }

	  if (debug == 1)
	    {
	      size_t i;
	      for (i = 0; q[i]; i++)
		fprintf (stderr, "output[%d] = U+%04x\n", i, q[i] & 0xFFFF);
	    }

	  p = stringprep_ucs4_to_utf8 (q, -1, NULL, NULL);
	  if (!p)
	    {
	      free (q);
	      fprintf (stderr, _("%s: could not convert from UCS-4 to UTF-8.\n"),
		       readbuf);
	      return 1;
	    }

	  r = stringprep_utf8_to_locale (p);
	  free (p);
	  if (!r)
	    {
	      fprintf (stderr, _("%s: could not convert from UTF-8 to %s.\n"),
		       readbuf, stringprep_locale_charset ());
	      return 1;
	    }

	  fprintf (stdout, "%s\n", r);

	  free (r);
	}


  return 0;
}

void usage (void) {
  fprintf (stderr, "%s %s\n", progs, version);
  fprintf (stderr, _("Usage: %s [OPTIONS...] convert_domain\n"), progs);
  fprintf (stderr, _("valid options:\n"));
  fprintf (stderr, _("    -h    help message (this message)\n"));
  fprintf (stderr, _("    -a    convert EUC-KR to Punycode\n"));
  fprintf (stderr, _("    -u    convert punycode to Euc-kr\n"));
  fprintf (stderr, _("    -v    verbose mode\n"));

  exit (1);
}
