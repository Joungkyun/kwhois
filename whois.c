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
#ident "$Id: whois.c,v 1.1 2004-02-04 06:29:31 oops Exp $"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
#ifdef HAVE_ARPA_NAMESER_H
#include <arpa/nameser.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif

#ifndef DEFAULT_SERVER
#define DEFAULT_SERVER "whois.crsnic.net"
#endif
#ifndef DEFAULT_PORT
#define DEFAULT_PORT "whois"
#endif

#define KR_SERVER "whois.krnic.net"
#define JP_SERVER "whois.nic.ad.jp"
#define TO_SERVER "whois.tonic.to"
#define CC_SERVER "whois.nic.cc"
#define TV_SERVER "whois.tv"

#define DEFAULTS_SERVER "whois.crsnic.net whois.krnic.net whois.nic.ad.jp whois.tonic.to whois.nic.cc whois.tv"

int get_lang();
int langs;

void
alarm_handler(int signum)
{
	char *message;

	if ( langs == 1 ) { message = "요청 시간 초과.\n"; }
	else { message = "Timeout exceeded.\n"; }
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
		if ( langs == 1 ) {
			fprintf(stderr, "%s:%s 에 접속하는 동안 발생한 에러 : %s\n",
				server, port, gai_strerror(errno));
		} else {
			fprintf(stderr, "%s while getting connection info for %s:%s\n",
				gai_strerror(errno), server, port);
		}
		exit(1);
	}

	if(res == NULL) {
		if ( langs == 1 ) {
			fprintf(stderr, "%s:%s 에 접속을 하는 동안 아무런 결과를 얻지 못함\n",
				server, port);
		} else {
			fprintf(stderr, "no results while getting connection info for "
				"%s:%s\n", server, port);
		}
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
		if ( langs == 1 ) {
			fprintf(stderr, "%s 서비스에 발생한 에러 : %s\n",
				port, strerror(errno));
		} else {
			fprintf(stderr, "%s while getting service info for %s\n",
				strerror(errno), port);
		}
		exit(1);
	}

	host = gethostbyname(server);
	if(host == NULL) {
		if ( langs == 1 ) {
			printf("\"%s\" 에 접속하는 동안 발생한 에러 : %s\n",
				server, strerror(h_errno));
		} else {
			printf("%s while getting address for \"%s\"\n",
				strerror(h_errno), server);
		}
		exit(1);
	}

	sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sd == -1) {
		if ( langs == 1 ) {
			printf("%s initializing protocol stacks?!?\n",
				strerror(errno));
		} else {
			printf("%s initializing protocol stacks?!?\n",
				strerror(errno));
		}
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
		if ( langs == 1 ) {
			printf("%s 의 접속 에러 : %s\n", server, strerror(errno));
		} else {
			printf("%s connecting to %s\n", strerror(errno), server);
		}
		exit(1);
	}

	printf("[%s]\n", server);
	snprintf(buf, sizeof(buf), "%s\r\n", query);
	if(verbose) {
		printf("=>'%s'\n", query);
	}
	send(sd, buf, strlen(buf), 0);

	fflush(stdout);

	reader = fdopen(sd, "r");
	if(reader == NULL) {
		if ( langs == 1 ) {
			printf("소켓전송 에러 : %s\n", strerror(errno));
		} else {
			printf("%s while reading from socket\n",
				strerror(errno));
		}
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
}

int
main(int argc, char **argv)
{
	char *server = NULL;
	char *port = DEFAULT_PORT;
	char *query = NULL;
	int i, recurse = -1, help = 0, parse = 1, verbose = 0, timeout = -1;

	/* get language type
	 * return 1 : korean
	 * return 0 : english
	 */
	langs = get_lang();

	/* If we got no arguments, we're just going to print out a short
	 * usage message and quit. */
	if(argc <= 1) {
		help = 1;
	}

	while(parse && ((i = getopt(argc, argv, "h:vrnp:t:-")) != -1)) {
		switch(i) {
			case 'h':
				/* The -h option for traditional whois specifies
				 * the server to query. */
				server = strdup(optarg);
				break;
			case 'p':
				/* Use an alternate port.  This can be a name
				 * or a number. */
				port = strdup(optarg);
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
		if ( langs == 1 ) {
			printf("사용법: %s [옵션...] 질의[@서버[:포트]]\n"
			       "유효한 옵션들:\n"
			       "       -h server  whois 서버 이름\n"
			       "       -p port    서버 포트\n"
			       "       -t timeout 질의 시간 제한\n"
			       "       -r         force recursion\n"
			       "       -n         disable recursion\n"
			       "       -v         verbose mode\n"
			       "       --         treat remaining arguments as part of "
			       "the query\n", strchr(argv[0], '/') ?
			       strrchr(argv[0], '/') + 1 : argv[0]);
			printf("기본 서버 : %s\n", DEFAULTS_SERVER);
		} else {
			printf("Usage: %s [OPTION...] query[@server[:port]]\n"
			       "valid options:\n"
			       "       -h server  server name\n"
			       "       -p port    server port\n"
			       "       -t timeout query time limit\n"
			       "       -r         force recursion\n"
			       "       -n         disable recursion\n"
			       "       -v         verbose mode\n"
			       "       --         treat remaining arguments as part of "
			       "the query\n", strchr(argv[0], '/') ?
			       strrchr(argv[0], '/') + 1 : argv[0]);
			printf("default server is %s\n", DEFAULTS_SERVER);
		}
		exit(0);
	}

	/* If we didn't get an explicit server name, check if the query string
	 * includes a '@', and use the string to its right if we have one.
	 * This handles fwhois/finger syntax. */
	if(server == NULL) {
		if(strrchr(query, '@') != NULL) {
			server = strrchr(query, '@');
			server[0] = '\0';
			server++;
		} else {
			char *tail;
			tail = strdup(rindex(query, '.'));

			/* Nothing there either.  Use the NICNAMESERVER,
			 * WHOISSERVER, or DEFAULT_SERVER, in that order. */
			if((server == NULL) && getenv("NICNAMESERVER")) {
				server = getenv("NICNAMESERVER");
			} else {
				if((server == NULL) && getenv("WHOISSERVER")) {
					server = getenv("WHOISSERVER");
				} else if (!tail) {
					server = DEFAULT_SERVER;
				} else {
					if (!strcmp(tail, ".kr")) {
						server = KR_SERVER;
					} else if (!strcmp(tail, ".to")) {
						server = TO_SERVER;
					} else if (!strcmp(tail, ".cc")) {
						server = CC_SERVER;
					} else if (!strcmp(tail, ".jp")) {
						server = JP_SERVER;
					} else if (!strcmp(tail, ".tv")) {
						server = TV_SERVER;
					} else if (!strcmp(tail, ".biz")) {
						server = "whois.networksolutions.com";
					} else if (!strcmp(tail, ".info")) {
						server = "whois.networksolutions.com";
					} else {
						server = DEFAULT_SERVER;
					}
				}
			}
		}
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
		if(strcmp(DEFAULT_SERVER, server) == 0) {
			recurse = 1;
		} else {
			recurse = 0;
		}
	}

	/* Hand it off to the query function. */
	process_query(server, port, query, timeout, recurse, verbose);

	return 0;
}

int get_lang()
{
       if ( !strncmp( (char *) getenv("LANG"), "ko", 2) ) {
               return 1;
       } else {
               return 0;
       }
}
