/*
 *  Copyright (C) 2015 JoungKyun.Kim <http://oops.org/>
 *
 *  This file is part of libipcalc.
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
 *
 *  This program is forked from Nalin Dahyabhai's whois on 2001
 *    Nalin Dahyabhai <nalin@redhat.com>
 */
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
#	include <olibc/libpcre.h>
#	include <olibc/libstring.h>
#endif

#include "tld_server.h"

#ifndef DEFAULT_PORT
#define DEFAULT_PORT "whois"
#endif

#ifndef null
#	define null NULL
#endif

// already define from olibc
//typedef unsigned long ULong;

typedef struct {
	char	* server;
	char	* port;
	char	* query;
	int		timeout;
	int		recurse;
	int		verbose;
} Pquery;

int  crsCheck (char * wserv);
int  is_ipaddr (char * query);
char * get_tail (char * query);
char * parseQuery (char * qry, char * wserv);
int  is_longip (char * query);
void long2ip (char ** ip);
void str_tolower (char *buf);
void str_toupper (char *buf);
char * check_2depth (char * ext);

char * ex = null;

// {{{ void alarm_handler (int signum)
void alarm_handler (int signum) {
	char	* message;

	message = _("Timeout exceeded.\n");
	write (STDERR_FILENO, message, strlen (message));
	exit (0);
} // }}}

// {{{ static void get_next_server (char * buf, char ** server)
static void get_next_server (char * buf, char ** server) {
	char	* p = null;
	char	* next = buf;

	if ( *server != null )
		free (*server);

	while ( (next[0] != '\0') && (next[0] != ':') ) {
		next++;
	}
	while ( (next[0] != '\0') &&
			((next[0] == ':') ||
			 isspace (next[0])) ) {
		next++;
	}
	p = buf + strlen (buf);
	while ( (p > buf) &&
			((isspace (p[-1])) ||
			 (p[-1] == '\r') ||
			 (p[-1] == '\n')) ) {
		p[-1] = '\0';
		p--;
	}

	if ( (p = strstr (next, "whois://")) != null )
		next = p + 8;
	else if ( (p = strstr (next, "http://")) != null )
		next = p + 7;

	if ( strchr (next, '.') == null ) {
		char	* q;
		q = malloc (sizeof (char) * strlen (next) + 20);
		memset (q, strlen (next) + 20, 0);

		if ( strlen (next) > 1 ) {
			sprintf (q, "%c%c.whois-servers.net", tolower (next[0]), tolower (next[1]));
			*server = strdup (q);
		} else
			*server = null;
		free (q);
	} else
		*server = strdup (next);
}
// }}}

// {{{ static void process_query (Pquery * v)
static void
process_query (Pquery * v) {
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

#ifdef HAVE_ICONV_H
	{
		iconv_t	cd = (iconv_t)(-1);
		ICONV_CONST
		char	* from;
		char	* to,
				* to_conv;
		size_t	flen,
				tlen;
		int		ic = current_charset ();

		if (
				! strcasecmp (v->server, "whois.krnic.net") ||
				! strcasecmp (v->server, "whois.kisa.or.kr") ||
				! strcasecmp (v->server, "whois.kisa.kr") ||
				! strcasecmp (v->server, "oldwhois.kisa.or.kr")
			) {

			if ( ! strncasecmp (v->server, "oldwhois.", 9) ) {
				if ( ! ic ) {
					cd = iconv_open ("UTF-8", "EUC-KR");
					printf ("Charset: Original charset is EUC-KR and kwhois convert to UTF-8\n");
				}
			} else {
				if ( ic ) {
					cd = iconv_open ("EUC-KR", "UTF-8");
					printf ("Charset: Original charset is UTF-8 and kwhois convert to EUC-KR\n");
				}
			}
		}

#endif
	memset (buf, 0, sizeof (buf));
	while ( fgets (buf, sizeof (buf) - 1, reader) != null ) {
		/* Give the user the literal string, including the newline. */
#ifdef HAVE_ICONV_H
		if ( cd != (iconv_t)(-1) ) {
			from = buf;
			flen = strlen (buf);
			tlen = (flen * 3) + 1;
			if ( (to_conv = (char *) malloc (sizeof (char) * tlen)) == NULL ) {
				printf ("%s", buf);
				goto skip_iconv;
			}
			memset (to_conv, 0, sizeof (char) * tlen);
			to = to_conv;

			errno = 0;
			iconv (cd, &from, &flen, &to, &tlen);
			switch (errno) {
				case E2BIG :
				case EILSEQ :
				case EINVAL :
					goto skip_iconv;
					break;
			}
			printf ("%s", to_conv);
			free (to_conv);
		} else
			printf ("%s", buf);
skip_iconv:
#else
		printf ("%s", buf);
#endif

		/* Create an upper-cased copy of the response line. */
		memset (ubuf, '\0', sizeof (ubuf));
		strcpy (ubuf, buf);
		str_toupper (ubuf);

		/* If the line includes the magic string, pull out the
		 * name of the server we should talk to next. */
		if ( strstr (ubuf, "REFERRALSERVER:") != null || strstr (ubuf, "WHOIS SERVER:") != null ) {
			get_next_server (buf, &next_server);
			v->recurse = 1;
			if ( next_server == null || strlen (next_server) == 0 )
				v->recurse = 0;
		}

		if ( v->recurse && next_server == null ) {
			if ( strstr (ubuf, "COUNTRY:") != null || strstr (ubuf, "COUNTRY CODE :") != null )
				get_next_server (buf, &next_server);
		}
	}
	fclose (reader);
	printf ("\n");

#ifdef HAVE_ICONV_H
		if ( cd != (iconv_t)(-1) )
			iconv_close (cd);
	}
#endif

	if ( v->timeout > 0 )
		alarm (0);

	if ( v->recurse > 0 && next_server != null && strcasecmp (next_server, v->server) ) {
		v->server = next_server;
		//v->recurse -= 1;

		if ( is_ipaddr (v->query) && ! strcmp (v->server + 3, "whois-servers.net") ) {
			// support ipaddress query
			if (
					strncmp (v->server, "kr", 2) &&
					strncmp (v->server, "jp", 2) &&
					strncmp (v->server, "tw", 2) &&
					strncmp (v->server, "br", 2)
				) {
				return;
			}
		}

		printf ("\n-------------------------------------------\n");
		printf ("** Recurse try to ");
#ifdef HAVE_LIBOGC
		setansi (stdout, OC_RED, false);
#endif
		printf ("%s", next_server);
#ifdef HAVE_LIBOGC
		setansi (stdout, OC_ENDANSI, false);
#endif
		printf (" ...\n\n");
		sleep(1);

		if ( v->verbose ) {
			fprintf (stderr, _("[1;%dm=> Recurse Connection <=[7;0m\n"), COLOR);
			fprintf (stderr, _("------------------- Debug Message --------------------\n"));
			fprintf (stderr, _("[1;%dmHOST          :[7;0m %s\n"), COLOR, v->query);
			fprintf (stderr, _("[1;%dmSERVER        :[7;0m %s\n"), COLOR, v->server);
			fprintf (stderr, _("[1;%dmPORT          :[7;0m %s\n"), COLOR, v->port);
			fprintf (stderr, _("------------------- Debug Message --------------------\n\n"));
			sleep (1);
		}

		process_query (v);
		free (next_server);
	}
} // }}}

void set_server (Pquery * qr, char * value) {
	if ( qr->server != null )
		free (qr->server);
	qr->server = strdup (value);
}	

// {{{ int main (int argc, char ** argv)
int main (int argc, char ** argv) {
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
				set_server (&qr, optarg);
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
	str_tolower (qr.query);

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
			char *p = strrchr (qr.query, '@');
			set_server (&qr, p + 1);
			*p = 0;

			/* get contry code */
			ex = strdup (get_tail (qr.query));
		} else {
			/* get contry code */
			ex = strdup (get_tail (qr.query));

			if ( ! strcmp (ex, "ip address") )
				set_server (&qr, LU_SERVER);

			/* Nothing there either.  Use the NICNAMESERVER,
			 * WHOISSERVER, or DEFAULT_SERVER, in that order. */
			if ( (qr.server == null) && getenv ("NICNAMESERVER") ) {
				set_server (&qr, getenv ("NICNAMESERVER"));
			} else {
				if ( (qr.server == null) && getenv ("WHOISSERVER") ) {
					set_server (&qr, getenv ("WHOISSERVER"));
				} else if ( !ex ) {
					set_server (&qr, DEFAULT_SERVER);
				} else {
					if ( ! strcmp (ex, "ip address") ) {
						set_server (&qr, LU_SERVER);
						goto confirm_qrserver;
					}

					// 2 depth domain
#ifdef HAVE_LIBOGC
					const char ** matches = NULL;
					if ( preg_match_r ("/[^.]+(\\.[^.]+\\.[^.]+)$/", qr.query, &matches) > 0 ) {
						char *p;
						p = check_2depth((char *) matches[1]);

						if ( qr.server != NULL ) {
							set_server (&qr, p);
							if ( ex != NULL )
								free (ex);
							ex = strdup (matches[1]);

							goto confirm_qrserver;
						}
					}
#endif

					if ( strlen (ex) == 2 ) {
						char tmphost[50];
						if ( !strcmp (ex, "bj") )
							sprintf (tmphost, "%s", BJ_SERVER);
						else if ( !strcmp (ex, "bz") )
							sprintf (tmphost, "%s", BZ_SERVER);
						else if ( !strcmp (ex, "ng") )
							sprintf (tmphost, "%s", NG_SERVER);
						else if ( !strcmp (ex, "su") )
							sprintf (tmphost, "%s", SU_SERVER);
						else if ( !strcmp (ex, "tc") )
							sprintf (tmphost, "%s", TC_SERVER);
#ifdef HAVE_LIBOGC
						else if ( preg_match ("/\\.(cd|dz|so)$/", qr.query) )
							sprintf (tmphost, "whois.nic.%c%c", ex[0], ex[1]);
#endif
						else
							sprintf (tmphost, "%c%c.%s", ex[0], ex[1], LO_SERVER);

						set_server (&qr, tmphost);
					} else {
						const char **p = NULL;

						for ( p = tlds; *p; p += 2 ) {
							if ( ! strcasecmp (ex, *p) ) {
								p++;
								set_server (&qr, (char *) *p);
								goto confirm_qrserver;
							}
						}

						// new gTlds
						for ( p = new_tglds; *p; p++ ) {
							if ( ! strcasecmp (ex, *p) ) {
								char tmphost[50];
								sprintf (tmphost, "whois.nic.%s", ex);
								set_server (&qr, tmphost);
								goto confirm_qrserver;
							}
						}

						set_server (&qr, DEFAULT_SERVER);
					}
				}
			}
		}
	} else {
		/* get contry code */
		ex = strdup (get_tail (qr.query));
	}

confirm_qrserver:

	/* If the server name includes a colon, snip the name there and
	 * assume everything to the right is a port number. */
	if ( strchr ( qr.server, ':') != null ) {
		qr.port = strchr (qr.server, ':');
		*qr.port = 0;
		qr.port++;
	}

	/* If we got neither the -r nor the -n arguments, set the default
	 * based on which server we're querying. */
	if ( qr.recurse == -1 )
		qr.recurse = (! strcasecmp (DEFAULT_SERVER, qr.server) || ! strcasecmp (LU_SERVER, qr.server) ) ? 1 : 0;

#ifdef HAVE_LIBOGC
#if HAVE_LIBOC_VER == 1
	name = strdup (convert_punycode (qr.query, NULL));
#else
	name = strdup (convert_punycode (qr.query, 0, qr.verbose));
#endif

	if ( is_longip (name) )
		long2ip (&name);

	if ( qr.verbose ) {
		fprintf (stderr, _("\n------------------- Debug Message --------------------\n\n"));
		fprintf (stderr, _("[1;%dmHOST          :[7;0m %s\n"), COLOR, qr.query);
		fprintf (stderr, _("[1;%dmCONV HOST     :[7;0m %s\n"), COLOR, name);
		fprintf (stderr, _("[1;%dmTAIL          :[7;0m %s\n"), COLOR, ex);
		fprintf (stderr, _("[1;%dmSERVER        :[7;0m %s\n"), COLOR, qr.server);
		fprintf (stderr, _("[1;%dmPORT          :[7;0m %s\n"), COLOR, qr.port);
		fprintf (stderr, _("\n------------------- Debug Message --------------------\n\n"));
	}

	if ( name != NULL ) {
		free (qr.query);
		qr.query = strdup (name);
		free (name);
	}

	/* Hand it off to the query function. */
	process_query (&qr);
#else
	if ( is_longip (qr.query) )
		long2ip (&(qr.query));

	process_query (&qr);
#endif
	free (qr.query);
	if ( qr.server != null )
		free (qr.server);
	free (ex);

	return 0;
} // }}}

// {{{ int crsCheck (char * wserv)
int crsCheck (char * wserv) {
	if ( ! strcmp ( wserv, DEFAULT_SERVER ) )
		return 1;

	return 0;
} // }}}

// {{{ char * parseQuery (char * qry, char * wserv)
char * parseQuery (char * qry, char * wserv) {
	static char	query[1024];
	char		tmp[1024];

	memset (query, 0, 1024);
	memset (tmp, 0, 1024);

	strncpy (tmp, qry, ( strlen (qry) > 1023 ) ? 1023 : strlen (qry));

	if ( ! strcmp ("jp", ex) ) {
		sprintf ( query, "%s/e\r\n", tmp);
	} else if ( ! strcmp ("ip address", ex) && ! strcasecmp (wserv, LU_SERVER) ) {
		sprintf ( query, "n + %s\r\n", tmp);
	} else
		sprintf ( query, "%s%s\r\n", crsCheck (wserv) ? "=" : "", tmp);

	return query;
} // }}}

// {{{ int is_ipaddr (char * query)
int is_ipaddr (char * query) {
	int		pos;
	char	point;

	pos = strlen (query) - 1;
	point = query[pos];

	if ( point > 47 && point < 58 )
		return 1;

	return 0;
} // }}}

// {{{ char * get_tail (char * query)
char * get_tail (char * query) {
	char	* gettail = null;

	if ( is_ipaddr (query) ) {
		return "ip address";
	} else {
		do {
			if ( gettail != null )
				*gettail = 0;
			gettail = rindex (query, '.');
		} while ( gettail != null && ! strcmp (gettail, ".") );

		if ( gettail == null )
			return "";

#ifdef HAVE_LIBOGC
#if HAVE_LIBOC_VER == 1
		return convert_punycode (gettail + 1, NULL);
#else
		return convert_punycode (gettail + 1, 0, 0);
#endif
#endif
		return gettail + 1;
	}
} // }}}

// {{{ int is_longip (char * query)
int is_longip (char * query) {
	while ( *query != 0 ) {
		if ( *query < 48 || *query > 57 )
			return 0;
		query++;
	}

	return 1;
} // }}}

// {{{ char * _long2ip (char * ip)
char * _long2ip (char * ip) {
	struct	in_addr addr;
	ULong	longip;

	longip = strtoul (ip, null, 10);

	addr.s_addr = htonl (longip);
	return inet_ntoa (addr);
} // }}}

// {{{ void long2ip (char ** q)
void long2ip (char ** q) {
	char	* p;

	if ( ! *q )
		return;

	p = strdup (*q);
	free (*q);

	*q = strdup (_long2ip (p));
	free (p);
} // }}}

// {{{ void str_tolower (char *buf)
void str_tolower (char *buf) {
#ifdef HAVE_LIBOGC
	strtolower (buf);
#else
	int len = strlen (buf);
	int i;

	for ( i=0; i<len; i++ )
		buf[i] = tolower (buf[i]);
#endif
} // }}}

// {{{ void str_toupper (char *buf)
void str_toupper (char *buf) {
#ifdef HAVE_LIBOGC
	strtoupper (buf);
#else
	int len = strlen (buf);
	int i;

	for ( i=0; i<len; i++ )
		buf[i] = toupper (buf[i]);
#endif
} // }}}

// {{{ char * check_2depth (char * ext)
char * check_2depth (char * ext) {
	const char **p = NULL;

	for ( p = two_depth_tlds; *p; p += 2 ) {
		if ( ! strcasecmp (ext, *p) ) {
			p++;
			return (char *) *p;
		}
	}

	return NULL;
}
// }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
