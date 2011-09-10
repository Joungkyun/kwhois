/*******************************************************************************
 A replacement for fwhois/whois.

 Copyright (C) 2000,2001 Red Hat, Inc.
 Written by Nalin Dahyabhai <nalin@redhat.com>
 
 This is free software; you can redistribute it and/or modify it
 under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 ******************************************************************************/
#ident "$Id$"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* support i18n */
#include "i18n.h"

/* ansi color */
#define COLOR 34

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>

#include <errno.h>

#include <limits.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <netdb.h>
#include <ctype.h>

#ifdef HAVE_LIBOGC
#	include <olibc/libidn.h>
#endif

#include "tld_server.h"

#ifndef DEFAULT_PORT
#define DEFAULT_PORT "whois"
#endif

#ifndef null
#	define null NULL
#endif
typedef unsigned long ULong;

typedef struct {
	char	* server;
	char	* port;
	char	* query;
	int		timeout;
	int		recurse;
	int		verbose;
} Pquery;

int  check_code (char * tail);
int  crsCheck (char * wserv);
int  is_ipaddr (char * query);
char * get_tail (char * query);
char * parseQuery (char * qry, char * wserv);
int  is_longip (char * query);
void long2ip (char ** ip);

char * extension = null;

void alarm_handler (int signum) { // {{{
	char	* message;

	message = _("Timeout exceeded.\n");
	write (STDERR_FILENO, message, strlen (message));
	exit (0);
} // }}}

static void
process_query (Pquery * v) { // {{{
	int		sd = -1,
			ret = 0;
	FILE	* reader = null;
	char	buf[LINE_MAX],
			ubuf[LINE_MAX];
	char	* next_server = null;

#ifdef HAVE_GETADDRINFO
	struct	addrinfo hints,
			* res = null;

	memset (&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( v->timeout > 0 ) {
		signal (SIGALRM, alarm_handler);
		alarm (v->timeout);
	}

	if ( getaddrinfo (v->server, v->port, &hints, &res) != 0 ) {
		fprintf (stderr, _("%s while getting connection info for %s:%s\n"),
				gai_strerror (errno), v->server, v->port);
		exit (1);
	}

	if ( res == null ) {
		fprintf (stderr, _("no results while getting connection info for %s:%s\n"), v->server, v->port);
		exit (1);
	}

	while ( res != null ) {
		sd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
		ret = -1;

		if ( sd == -1 ) {
			res = res->ai_next;
			continue;
		}

		ret = connect (sd, res->ai_addr, res->ai_addrlen);
		if ( ret != -1 )
			break;

		res = res->ai_next;
		close (sd);
	}
#else
	struct	sockaddr_in server_addr;
	struct	hostent * host = null;
	struct	servent * serv = null;

	serv = getservbyname (v->port, "tcp");
	if ( serv == null )
		serv = getservbyname ("nicname", "tcp");

	if ( serv == null )
		serv = getservbyname ("whois", "tcp");

	if ( serv == null ) {
		fprintf (stderr, _("%s while getting service info for %s\n"),
				strerror (errno), v->port);
		exit (1);
	}

	host = gethostbyname (v->server);
	if ( host == null ) {
		printf (_("%s while getting address for \"%s\"\n"),
				strerror (h_errno), v->server);
		exit (1);
	}

	sd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if ( sd == -1 ) {
		printf (_("%s initializing protocol stacks?!?\n"),
				strerror (errno));
		exit (1);
	}

	memset (&server_addr, 0, sizeof (server_addr));
	server_addr.sin_family = host->h_addrtype;
	server_addr.sin_port = serv->s_port;
	memcpy (&server_addr.sin_addr, host->h_addr_list[0], host->h_length);

	ret = connect (
		sd,
		(struct sockaddr *) &server_addr,
		sizeof (struct sockaddr_in)
	);
#endif

	if ( ret == -1 ) {
		printf (_("%s connecting to %s\n"), strerror (errno), v->server);
		exit (1);
	}

	if ( v->verbose ) {
		char buf[1024] = { 0, };

		strncpy (buf, parseQuery (v->query, v->server), 1024);
		buf[strlen (buf) - 2] = 0;

		fprintf (stderr, _("===> Connect to %s success\n"), v->server);
		fprintf (stderr, _("===> Query %s to server\n\n"), buf);
	}

	printf ("[%s]\n", v->server);

	snprintf (buf, sizeof (buf), "%s", parseQuery (v->query, v->server));
	send (sd, buf, strlen (buf), 0);

	fflush (stdout);

	reader = fdopen (sd, "r");
	if ( reader == null ) {
		printf (_("%s while reading from socket\n"), strerror (errno));
		exit (1);
	}

	memset (buf, 0, sizeof (buf));
	while ( fgets (buf, sizeof (buf) - 1, reader) != null ) {
		int i;

		/* Give the user the literal string, including the newline. */
		printf ("%s", buf);

		/* Create an upper-cased copy of the response line. */
		memset (ubuf, '\0', sizeof (ubuf));
		for ( i=0; i < strlen (buf); i++ ) {
			ubuf[i] = toupper (buf[i]);
		}

		/* If the line includes the magic string, pull out the
		 * name of the server we should talk to next. */
		if ( v->recurse && (strstr (ubuf, "WHOIS SERVER:") != null)) {
			char	* p = null;
			next_server = buf;
			while ( (next_server[0] != '\0') && (next_server[0] != ':') ) {
				next_server++;
			}
			while ( (next_server[0] != '\0') &&
			      ((next_server[0] == ':') ||
			       isspace (next_server[0])) ) {
				next_server++;
			}
			p = buf + strlen (buf);
			while ( (p > buf) &&
			      ((isspace (p[-1])) ||
			       (p[-1] == '\r') ||
			       (p[-1] == '\n')) ) {
				p[-1] = '\0';
				p--;
			}
			next_server = strdup (next_server);
		}
	}
	fclose (reader);
	printf ("\n");

	if ( v->timeout > 0 )
		alarm (0);

	if ( (v->recurse > 0) && (next_server != null) ) {
		v->server = next_server;
		v->recurse -= 1;
		process_query (v);
	}
	free (next_server);
} // }}}

int main (int argc, char ** argv) { // {{{
	char	* name = null;
	int		i,
			help = 0,
			parse = 1;
	Pquery	qr;

	qr.server  = null;
	qr.port    = DEFAULT_PORT;
	qr.query   = null;
	qr.timeout = -1;
	qr.recurse = -1;
	qr.verbose = 0;

	/* support i18n */
#ifdef ENABLE_NLS
	i18n_print ();
#endif

	/* If we got no arguments, we're just going to print out a short
	 * usage message and quit. */
	if ( argc <= 1 )
		help = 1;

	while ( parse && ((i = getopt (argc, argv, "h:vrnp:Pt:-")) != -1) ) {
		switch (i) {
			case 'h':
				/* The -h option for traditional whois specifies
				 * the server to query. */
				qr.server = optarg;
				break;
			case 'p':
				/* Use an alternate port.  This can be a name
				 * or a number. */
				qr.port = optarg;
				break;
			case 'v':
				/* Be verbose.  Currently this means that we
				 * print the query for the user when we send
				 * it. */
				qr.verbose = 1;
				break;
			case 'r':
				/* Force chasing on. */
				qr.recurse = 1;
				break;
			case 'n':
				/* Force chasing off. */
				qr.recurse = 0;
				break;
			case 't':
				/* Use a timeout when querying.  The timeout
				 * applies to all phases of the query,
				 * including name resolution. */
				qr.timeout = atoi (optarg);
			case '-':
				parse = 0;
				break;
			default:
				parse = 0;
				help = 1;
				break;
		}
	}
	/* Anything else is part of the query string.  We try to be clever here,
	 * in that if we see multiple arguments, we string them together into a
	 * single query string.  Because we only have argv[] to play with, we
	 * have to guess that a single space will work, otherwise the user has
	 * to quote it all. */
	for ( i=optind; i<argc; i++ ) {
		if ( qr.query ) {
			char	* p,
					* q;
			p = strdup (argv[i]);
			q = qr.query;
			qr.query = malloc (strlen (p) + 1 + strlen (q) + 1);
			strcpy (qr.query, q);
			strcat (qr.query, " ");
			strcat (qr.query, p);
			free (p);
			free (q);
		} else {
			qr.query = strdup (argv[i]);
		}
	}

	/* If the help flag was given, or we didn't get anything that looked
	 * like a query string, print a short help message and quit. */
	if ( help || (qr.query == null) || (strlen ( qr.query) == 0) ) {
		fprintf (stderr, _("Usage: %s [OPTION...] query[@server[:port]]\n"),
				strchr (argv[0], '/') ? strrchr (argv[0], '/') + 1 : argv[0]);
		fprintf (stderr, _("valid options:\n"));
		fprintf (stderr, _("       -h server  server name\n"));
		fprintf (stderr, _("       -p port    server port\n"));
		fprintf (stderr, _("       -t timeout query time limit\n"));
		fprintf (stderr, _("       -r         force recursion\n"));
		fprintf (stderr, _("       -n         disable recursion\n"));
		fprintf (stderr, _("       -v         verbose mode\n"));
		fprintf (stderr, _("       --         treat remaining arguments as part of the query\n"));
		fprintf (stderr, _("default server is %s\n"), DEFAULT_SERVER);
		fprintf (stderr, "%s %s\n", PACKAGE_NAME, PACKAGE_VERSION);
		exit (1);
	}

	/* If we didn't get an explicit server name, check if the query string
	 * includes a '@', and use the string to its right if we have one.
	 * This handles fwhois/finger syntax. */
	if ( qr.server == null ) {
		if ( strrchr (qr.query, '@') != null) {
			qr.server = strrchr (qr.query, '@');
			qr.server[0] = '\0';
			qr.server++;

			/* get contry code */
			extension = strdup (get_tail (qr.query));
		} else {
			/* get contry code */
			extension = strdup (get_tail (qr.query));

			if ( ! strcmp (extension, "IP ADDRESS") )
				qr.server = LO_SERVER;

			/* Nothing there either.  Use the NICNAMESERVER,
			 * WHOISSERVER, or DEFAULT_SERVER, in that order. */
			if ( (qr.server == null) && getenv ("NICNAMESERVER") ) {
				qr.server = getenv ("NICNAMESERVER");
			} else {
				if ( (qr.server == null) && getenv ("WHOISSERVER") ) {
					qr.server = getenv ("WHOISSERVER");
				} else if ( !extension ) {
					qr.server = DEFAULT_SERVER;
				} else {
					if ( strlen (extension) == 2 ) {
						char tmphost[50];
						sprintf (tmphost, "%c%c.%s", extension[0], extension[1], LO_SERVER);
						qr.server = tmphost;
					} else if (!strcasecmp (extension, "biz")) {
						qr.server = BIZ_SERVER;
					} else if (!strcasecmp (extension, "info")) {
						qr.server = INFO_SERVER;
					} else if (!strcasecmp (extension, "name")) {
						qr.server = NAME_SERVER;
					} else if (!strcasecmp (extension, "org")) {
						qr.server = ORG_SERVER;
					} else if ( ! strcmp (extension, "IP ADDRESS") ) {
						qr.server = LO_SERVER;
					} else {
						qr.server = DEFAULT_SERVER;
					}
				}
			}
		}
	} else {
		/* get contry code */
		extension = strdup (get_tail (qr.query));
	}

	/* If the server name includes a colon, snip the name there and
	 * assume everything to the right is a port number. */
	if ( strchr ( qr.server, ':') != null ) {
		qr.port = strchr (qr.server, ':');
		qr.port[0] = '\0';
		qr.port++;
	}

	/* If we got neither the -r nor the -n arguments, set the default
	 * based on which server we're querying. */
	if ( qr.recurse == -1 )
		qr.recurse = (! strcasecmp (DEFAULT_SERVER, qr.server)) ? 1 : 0;

#ifdef HAVE_LIBOGC
	/* use racecode ??? */
	/*
	if ( ! check_code (extension) ) {
		strcpy (name, (char *) convert_punycode (qr.query, 0, qr.verbose));
	} else {
		strcpy (name, (char *) convert_racecode (qr.query, 0, qr.verbose));
	}
	*/
	name = strdup (convert_punycode (qr.query, 0, qr.verbose));

	if ( is_longip (name) )
		long2ip (&name);

	if ( qr.verbose ) {
		fprintf (stderr, _("\n------------------- Debug Message --------------------\n\n"));
		fprintf (stderr, _("[1;%dmHOST          :[7;0m %s\n"), COLOR, qr.query);
		fprintf (stderr, _("[1;%dmCONV HOST     :[7;0m %s\n"), COLOR, name);
		fprintf (stderr, _("[1;%dmTAIL          :[7;0m %s\n"), COLOR, extension);
		fprintf (stderr, _("[1;%dmSERVER        :[7;0m %s\n"), COLOR, qr.server);
		fprintf (stderr, _("[1;%dmPORT          :[7;0m %s\n"), COLOR, qr.port);
		fprintf (stderr, _("\n------------------- Debug Message --------------------\n\n"));
	}

	/* Hand it off to the query function. */
	process_query (&qr);
	free (name);
#else
	if ( is_longip (qr.query) )
		long2ip (&(qr.query));

	process_query (&qr);
#endif
	free (qr.query);
	free (extension);

	return 0;
} // }}}

int check_code (char * tail) { // {{{
	if ( ! strcasecmp ( tail, "com" ) || ! strcasecmp ( tail, "net" ) ) {
		return 1;
	} else if ( ! strcasecmp ( tail, "org" ) || ! strcasecmp ( tail, "info" ) ||
			    ! strcasecmp ( tail, "biz" ) || ! strcasecmp ( tail, "name" ) ) {
		return 2;
	}

	return 0;
} // }}}

int crsCheck (char * wserv) { // {{{
	if ( ! strcmp ( wserv, DEFAULT_SERVER ) )
		return 1;

	return 0;
} // }}}

char * parseQuery (char * qry, char * wserv) { // {{{
	static char	query[1024];
	char		tmp[1024];

	memset (query, 0, 1024);
	memset (tmp, 0, 1024);

	strncpy (tmp, qry, ( strlen (qry) > 1023 ) ? 1023 : strlen (qry));

	if ( ! strcmp ("jp", extension) ) {
		sprintf ( query, "%s/e\r\n", tmp);
	} else
		sprintf ( query, "%s%s\r\n", crsCheck (wserv) ? "=" : "", tmp);

	return query;
} // }}}

int is_ipaddr (char * query) { // {{{
	int		pos;
	char	point;

	pos = strlen (query) - 1;
	point = query[pos];

	if ( point > 47 && point < 58 )
		return 1;

	return 0;
} // }}}

char * get_tail (char * query) { // {{{
	char	* gettail = null;

	if ( is_ipaddr (query) ) {
		return "IP ADDRESS";
	} else {
		gettail = rindex (query, '.');
		if ( gettail == null )
			return "";

		return gettail + 1;
	}
} // }}}

int is_longip (char * query) { // {{{
	while ( *query != 0 ) {
		if ( *query < 48 || *query > 57 )
			return 0;
		query++;
	}

	return 1;
} // }}}

char * _long2ip (char * ip) { // {{{
	struct	in_addr addr;
	ULong	longip;

	longip = strtoul (ip, null, 10);

	addr.s_addr = htonl (longip);
	return inet_ntoa (addr);
} // }}}

void long2ip (char ** q) { // {{{
	char	* p;

	if ( ! *q )
		return;

	p = strdup (*q);
	free (*q);

	*q = strdup (_long2ip (p));
	free (p);
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
