/* kwhois.c by JoungKyun Kim & ByungKi Lee in BbuWoo Packagement.
* Networking code taken and modified from net.c in the finger source code
*/

/*
* Copyright (c) 1989 The Regents of the University of California.
* All rights reserved.
*
* This code is derived from software contributed to Berkeley by
* Tony Nardo of the Johns Hopkins University/Applied Physics Labi
* and Redistributioned by JoungKyun Kim & ByungKi Lee in BbuWoo Packagement..
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. All advertising materials mentioning features or use of this software
*    must display the following acknowledgement:
*      This product includes software developed by the University of
*      California, Berkeley and its contributors.
* 4. Neither the name of the University nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
*
* THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
* $Id: kwhois.c,v 1.2 2004-02-04 06:12:07 oops Exp $
*/

#ifndef lint
char copy_en[] =
"Redistributions JoungKyun Kim & ByungKi Lee in B.P from fwhois.\n";
char copy_ko[] =
"Redistributions ������ & �̺��� in B.P from fwhois.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)kwhois.c   1.00 (DQ) 7/26/93";
#endif /* not lint */

#include <sys/param.h>
#include <sys/file.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <ctype.h>

main(argc, argv)
char **argv;
int argc;
{
   char *stuff, *eleet;
   char *tail, *fname;
   
   if (argc == 2) {
      eleet = argv[1];
      if (strstr(argv[1], ".")) {
        tail = strdup(rindex(argv[1], '.'));

        /* If no host specified, use whois.internic.net */
        if(!index(argv[1],'@')) {
           if (tail) {
              if(!strcmp(tail, ".kr")) {
                 stuff = strcat(eleet,"@whois.nic.or.kr");
              }
              else if(!strcmp(tail, ".to")) {
                 stuff = strcat(eleet,"@whois.tonic.to");
              }
              else if(!strcmp(tail, ".cc")) {
                 stuff = strcat(eleet,"@whois.nic.cc");
              }
              else if(!strcmp(tail, ".jp")) {
                 stuff = strcat(eleet,"@whois.nic.ad.jp");
              }
              else {
                 stuff = strcat(eleet,"@whois.internic.net");
              }
           }
           eleet = stuff;
        }
      }	
      else {
	stuff = strcat(eleet,"@whois.internic.net");
      }
   }
   /* If the user entered an invalid argument, here they go */
   if (argc != 2) {
      fname = strdup(argv[0]);
      
      if (!strcmp((char *)getenv("LANG"), "ko")) {
         printf("���� : kwhois [domain name]\n\n");
         printf("�Ʒ��� �����Ǵ� whois server���� server�� �Ʒ��� ��������\n");
         printf("�˻��� �ϼž� �մϴ�.\n");
         printf("Supported Default whois Server :\n");
         printf("Krnic InterNic Tonic eNIC JpNIC\n\n");
         printf("���� : kwhois [domain name]@[whois server]\n");
	 printf("%s\n", copy_ko);
         exit(1);
      }
      else {
         printf("USAGE : kwhois [domainanme]\n\n");
         printf("If you wanna search some Whois Directory Server\n");
         printf("except Krnic, Internic, eNIC and ToNIC, JPnic\n");
         printf("U must use follow usage.\n\n");
         printf("Expected USAGE : kwhois [domainname]@[whois server]\n");
	 printf("%s\n", copy_en);
         exit(1);
      }
   }
   netfinger(eleet);
}

netfinger(name)
   char *name;
{
   register FILE *fp;
   register int c, lastc;
   struct in_addr defaddr;
   struct hostent *hp, def;
   struct servent *sp;
   struct sockaddr_in sin;
   int s;
   char *alist[1], *host, *rindex();
   u_long inet_addr();

   if (!(host = rindex(name, '@')))
      return;
   (int)*host++ = NULL;
   if (!(hp = gethostbyname(host))) {
      defaddr.s_addr = inet_addr(host);
      if (defaddr.s_addr == -1) {
         (void)fprintf(stderr,
             "kwhois: unknown host: %s\n",host);
         return;
      }
      def.h_name = host;
      def.h_addr_list = alist;
      def.h_addr = (char *)&defaddr;
      def.h_length = sizeof(struct in_addr);
      def.h_addrtype = AF_INET;
      def.h_aliases = 0;
      hp = &def;
   }
   if (!(sp = getservbyname("whois", "tcp"))) {
      (void)fprintf(stderr, "kwhois: tcp/whois: unknown service\n");
      return;
   }
   sin.sin_family = hp->h_addrtype;
   bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
   sin.sin_port = sp->s_port;
   if ((s = socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0) {
      perror("kwhois: socket");
      return;
   }

   /* have network connection; identify the host connected with */
   (void)printf("[%s]\n", hp->h_name);
   if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
      perror("kwhois: connect");
      (void)close(s);
      return;
   }

   /* send the name followed by <CR><LF> */
   (void)write(s, name, strlen(name));
   (void)write(s, "\r\n", 2);

   if (fp = fdopen(s, "r"))
      while ((c = getc(fp)) != EOF) {
         if (c == 0x0d) {
            c = '\n';
            lastc = '\r';
         } else {
            if (!isprint(c) && !isspace(c) && !((c & 0xe0) > 0x90))
               c |= 0x40;
            if (lastc != '\r' || c != '\n')
               lastc = c;
            else {
               lastc = '\n';
               continue;
            }
         }
         putchar(c);
      }
   if (lastc != '\n')
      putchar('\n');
   (void)fclose(fp);
}
