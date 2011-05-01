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

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SIGNAL_H
#include <sys/signal.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifdef HAVE_LIBIDN_H
#  ifdef HAVE_LIBOGC
#    include <libidn.h>
#  endif
#endif

#include "tld_server.h"

#ifndef DEFAULT_PORT
#define DEFAULT_PORT "whois"
#endif

int check_code ( char *tail );
int crsCheck ( char *wserv );
int is_ipaddr (char *query);
char * get_tail (char *query);
char * parseQuery ( char *qry, char *wserv);
int is_longip (char *query);
void long2ip (char ** ip);

char *extension = NULL;

void
alarm_handler(int signum)
{
	char *message;

	message = _("Timeout exceeded.\n");
	write(STDERR_FILENO, message, strlen(message));
	exit(0);
}

static void
process_query(const char *server, const char *port, const char *query,
	      int timeout, int recurse, int verbose)
{
	int sd = -1, ret = 0;
	FILE *reader = NULL;
	char buf[LINE_MAX], ubuf[LINE_MAX];
	char *next_server = NULL;

#ifdef HAVE_GETADDRINFO
	struct addrinfo hints, *res = NULL;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if(timeout > 0) {
		signal(SIGALRM, alarm_handler);
		alarm(timeout);
	}

	if(getaddrinfo(server, port, &hints, &res) != 0) {
		fprintf(stderr, _("%s while getting connection info for %s:%s\n"),
			gai_strerror(errno), server, port);
		exit(1);
	}

	if(res == NULL) {
		fprintf(stderr, _("no results while getting connection info for %s:%s\n"), server, port);
		exit(1);
	}

	while(res != NULL) {
		sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		ret = -1;

		if(sd == -1) {
			res = res->ai_next;
			continue;
		}

		ret = connect(sd, res->ai_addr, res->ai_addrlen);
		if(ret != -1) {
			break;
		}
		res = res->ai_next;
		close(sd);
	}
#else
	struct sockaddr_in server_addr;
	struct hostent *host = NULL;
	struct servent *serv = NULL;

	serv = getservbyname(port, "tcp");
	if(serv == NULL) {
		serv = getservbyname("nicname", "tcp");
	}
	if(serv == NULL) {
		serv = getservbyname("whois", "tcp");
	}
	if(serv == NULL) {
		fprintf(stderr, _("%s while getting service info for %s\n"),
			strerror(errno), port);
		exit(1);
	}

	host = gethostbyname(server);
	if(host == NULL) {
		printf(_("%s while getting address for \"%s\"\n"),
			strerror(h_errno), server);
		exit(1);
	}

	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sd == -1) {
		printf(_("%s initializing protocol stacks?!?\n"),
			strerror(errno));
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = host->h_addrtype;
	server_addr.sin_port = serv->s_port;
	memcpy(&server_addr.sin_addr, host->h_addr_list[0],
	       host->h_length);

	ret = connect(sd, (struct sockaddr*) &server_addr,
		      sizeof(struct sockaddr_in));
#endif

	if(ret == -1) {
		printf(_("%s connecting to %s\n"), strerror(errno), server);
		exit(1);
	}

	if (verbose) {
		fprintf (stderr, _("===> Connect to %s success\n"), server);
		fprintf (stderr, _("===> Query %s to server\n\n"), parseQuery ((char *) query, (char *) server));
	}

	printf("[%s]\n", server);

	snprintf (buf, sizeof (buf), "%s", parseQuery ((char *) query, (char *) server));
	send(sd, buf, strlen(buf), 0);

	fflush(stdout);

	reader = fdopen(sd, "r");
	if(reader == NULL) {
		printf(_("%s while reading from socket\n"), strerror(errno));
		exit(1);
	}

	memset(buf, 0, sizeof(buf));
	while(fgets(buf, sizeof(buf) - 1, reader) != NULL) {
		int i;

		/* Give the user the literal string, including the newline. */
		printf("%s", buf);

		/* Create an upper-cased copy of the response line. */
		memset(ubuf, '\0', sizeof(ubuf));
		for(i = 0; i < strlen(buf); i++) {
			ubuf[i] = toupper(buf[i]);
		}

		/* If the line includes the magic string, pull out the
		 * name of the server we should talk to next. */
		if(recurse && (strstr(ubuf, "WHOIS SERVER:") != NULL)) {
			char *p = NULL;
			next_server = buf;
			while((next_server[0] != '\0') &&
			      (next_server[0] != ':')) {
				next_server++;
			}
			while((next_server[0] != '\0') &&
			      ((next_server[0] == ':') ||
			       isspace(next_server[0]))) {
				next_server++;
			}
			p = buf + strlen(buf);
			while((p > buf) &&
			      ((isspace(p[-1])) ||
			       (p[-1] == '\r') ||
			       (p[-1] == '\n'))) {
				p[-1] = '\0';
				p--;
			}
			next_server = strdup(next_server);
		}
	}
	fclose(reader);
	printf("\n");

	if(timeout > 0) {
		alarm(0);
	}

	if((recurse > 0) && (next_server != NULL)) {
		process_query(next_server, port, query,
			      timeout, recurse - 1, verbose);
	}
	free (next_server);
}

int main(int argc, char **argv) {
	char *server = NULL;
	char *port = DEFAULT_PORT;
	char *query = NULL;
	char *name = NULL;
	int i, recurse = -1, help = 0, parse = 1;
	int verbose = 0, timeout = -1;

	/* support i18n */
#ifdef ENABLE_NLS
	i18n_print();
#endif

	/* If we got no arguments, we're just going to print out a short
	 * usage message and quit. */
	if(argc <= 1) {
		help = 1;
	}

	while(parse && ((i = getopt(argc, argv, "h:vrnp:Pt:-")) != -1)) {
		switch(i) {
			case 'h':
				/* The -h option for traditional whois specifies
				 * the server to query. */
				server = optarg;
				break;
			case 'p':
				/* Use an alternate port.  This can be a name
				 * or a number. */
				port = optarg;
				break;
			case 'v':
				/* Be verbose.  Currently this means that we
				 * print the query for the user when we send
				 * it. */
				verbose = 1;
				break;
			case 'r':
				/* Force chasing on. */
				recurse = 1;
				break;
			case 'n':
				/* Force chasing off. */
				recurse = 0;
				break;
			case 't':
				/* Use a timeout when querying.  The timeout
				 * applies to all phases of the query,
				 * including name resolution. */
				timeout = atoi(optarg);
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
	for(i = optind; i < argc; i++) {
		if(query) {
			char *p, *q;
			p = strdup(argv[i]);
			q = query;
			query = malloc(strlen(p) + 1 + strlen(q) + 1);
			strcpy(query, q);
			strcat(query, " ");
			strcat(query, p);
			free(p);
			free(q);
		} else {
			query = strdup(argv[i]);
		}
	}

	/* If the help flag was given, or we didn't get anything that looked
	 * like a query string, print a short help message and quit. */
	if(help || (query == NULL) || (strlen(query) == 0)) {
		fprintf (stderr, _("Usage: %s [OPTION...] query[@server[:port]]\n"),
				strchr(argv[0], '/') ? strrchr(argv[0], '/') + 1 : argv[0]);
		fprintf (stderr, _("valid options:\n"));
		fprintf (stderr, _("       -h server  server name\n"));
		fprintf (stderr, _("       -p port    server port\n"));
		fprintf (stderr, _("       -t timeout query time limit\n"));
		fprintf (stderr, _("       -r         force recursion\n"));
		fprintf (stderr, _("       -n         disable recursion\n"));
		fprintf (stderr, _("       -v         verbose mode\n"));
		fprintf (stderr, _("       --         treat remaining arguments as part of the query\n"));
		fprintf (stderr, _("default server is %s\n"), DEFAULT_SERVER);
		fprintf (stderr, "%s %s\n", NAME, PVERSION);
		exit(1);
	}

	/* If we didn't get an explicit server name, check if the query string
	 * includes a '@', and use the string to its right if we have one.
	 * This handles fwhois/finger syntax. */
	if(server == NULL) {
		if(strrchr(query, '@') != NULL) {
			server = strrchr(query, '@');
			server[0] = '\0';
			server++;

			/* get contry code */
			extension = strdup (get_tail (query));
		} else {
			/* get contry code */
			extension = strdup (get_tail (query));

			if ( ! strcmp (extension, "IP ADDRESS") )
				server = LO_SERVER;

			/* Nothing there either.  Use the NICNAMESERVER,
			 * WHOISSERVER, or DEFAULT_SERVER, in that order. */
			if((server == NULL) && getenv("NICNAMESERVER")) {
				server = getenv("NICNAMESERVER");
			} else {
				if((server == NULL) && getenv("WHOISSERVER")) {
					server = getenv("WHOISSERVER");
				} else if ( !extension ) {
					server = DEFAULT_SERVER;
				} else {
					if ( strlen(extension) == 2 ) {
						char tmphost[50];
						sprintf(tmphost, "%c%c.%s", extension[0], extension[1], LO_SERVER);
						server = tmphost;
					} else if (!strcasecmp(extension, "biz")) {
						server = BIZ_SERVER;
					} else if (!strcasecmp(extension, "info")) {
						server = INFO_SERVER;
					} else if (!strcasecmp(extension, "name")) {
						server = NAME_SERVER;
					} else if (!strcasecmp(extension, "org")) {
						server = ORG_SERVER;
					} else if ( ! strcmp (extension, "IP ADDRESS") ) {
						server = LO_SERVER;
					} else {
						server = DEFAULT_SERVER;
					}
				}
			}
		}
	} else {
		/* get contry code */
		extension = strdup (get_tail (query));
	}

	/* If the server name includes a colon, snip the name there and
	 * assume everything to the right is a port number. */
	if(strchr(server, ':') != NULL) {
		port = strchr(server, ':');
		port[0] = '\0';
		port++;
	}

	/* If we got neither the -r nor the -n arguments, set the default
	 * based on which server we're querying. */
	if(recurse == -1) {
		if(strcasecmp(DEFAULT_SERVER, server) == 0) {
			recurse = 1;
		} else {
			recurse = 0;
		}
	}

#ifdef HAVE_LIBOGC
	/* use racecode ??? */
	/*
	if ( ! check_code (extension) ) {
		strcpy (name, (char *) convert_punycode (query, 0, verbose));
	} else {
		strcpy (name, (char *) convert_racecode (query, 0, verbose));
	}
	*/
	name = strdup (convert_punycode (query, 0, verbose));

	if ( is_longip (name) )
		long2ip (&name);

	if (verbose) {
		fprintf (stderr, _("\n------------------- Debug Message --------------------\n\n"));
		fprintf (stderr, _("[1;%dmHOST          :[7;0m %s\n"), COLOR, query);
		fprintf (stderr, _("[1;%dmCONV HOST     :[7;0m %s\n"), COLOR, name);
		fprintf (stderr, _("[1;%dmTAIL          :[7;0m %s\n"), COLOR, extension);
		fprintf (stderr, _("[1;%dmSERVER        :[7;0m %s\n"), COLOR, server);
		fprintf (stderr, _("[1;%dmPORT          :[7;0m %s\n"), COLOR, port);
		fprintf (stderr, _("\n------------------- Debug Message --------------------\n\n"));
	}

	/* Hand it off to the query function. */
	process_query(server, port, name, timeout, recurse, verbose);
	free (name);
#else
	if ( is_longip (query) )
		long2ip (&query);

	process_query(server, port, query, timeout, recurse, verbose);
#endif
	free (query);
	free (extension);

	return 0;
}

int check_code ( char *tail ) {
	if ( ! strcasecmp ( tail, "com" ) || ! strcasecmp ( tail, "net" ) ) {
		return 1;
	} else if ( ! strcasecmp ( tail, "org" ) || ! strcasecmp ( tail, "info" ) ||
			    ! strcasecmp ( tail, "biz" ) || ! strcasecmp ( tail, "name" ) ) {
		return 2;
	}

	return 0;
}

int crsCheck ( char *wserv ) {
	if ( ! strcmp ( wserv, DEFAULT_SERVER ) )
		return 1;

	return 0;
}

char * parseQuery ( char *qry, char *wserv ) {
	static char query[1024];
	char tmp[1024];

	memset (query, 0, 1024);
	memset (tmp, 0, 1024);

	strncpy (tmp, qry, ( strlen (qry) > 1023 ) ? 1023 : strlen (qry));

	if ( ! strcmp ("jp", extension) ) {
		sprintf ( query, "%s/e\r\n", tmp);
	} else
		sprintf ( query, "%s%s\r\n", crsCheck (wserv) ? "=" : "", tmp);

	return query;
}

int is_ipaddr (char *query) {
	int pos;
	char point;

	pos = strlen (query) - 1;
	point = query[pos];

	if ( point > 47 && point < 58 )
		return 1;

	return 0;
}

char * get_tail (char *query) {
	char	* gettail = NULL;

	if ( is_ipaddr (query) ) {
		return "IP ADDRESS";
	} else {
		gettail = rindex(query, '.');
		if ( gettail == NULL )
			return "";

		return gettail + 1;
	}
}

int is_longip (char *query) {
	while ( *query != 0 ) {
		if ( *query < 48 || *query > 57 )
			return 0;
		query++;
	}

	return 1;
}

char * _long2ip (char *ip) {
	struct in_addr addr;
	unsigned long longip;

	longip = strtoul (ip, NULL, 10);

	addr.s_addr = htonl (longip);
	return inet_ntoa (addr);
}

void long2ip (char **q) {
	char *p;

	if ( ! *q )
		return;

	p = strdup (*q);
	free (*q);

	*q = strdup (_long2ip (p));
	free (p);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
