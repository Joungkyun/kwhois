#$Id: Makefile,v 1.1.1.1 2004-02-04 06:05:14 oops Exp $
CC	=gcc
CFLAGS	=-O
PROG	=kwhois
SOURCE	=kwhois.c
STRIP	=strip

bsd:
	$(CC) $(CFLAGS) -o $(PROG) $(SOURCE)
	$(STRIP) $(PROG)

install:
	install -m755 kwhois /usr/bin/kwhois; \
	cd /usr/bin/; \
	ln -sf kwhois whois

clean:
	rm -f core *~ $(PROG)
