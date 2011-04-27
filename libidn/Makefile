# $Id: Makefile,v 1.1 2004-02-04 08:29:21 oops Exp $
CC	= gcc
LANGDIR	= /usr/local/share/locale
CFLAGS	= -g -O2
SOURCE	= idn.c idna.c nfkc.c profiles.c punycode.c stringprep.c stringprep_generic.c toutf8.c
OBJECT	= i18n.o idn.o idna.o nfkc.o profiles.o punycode.o stringprep.o stringprep_generic.o toutf8.o
OBOPT	= -Wall -DHAVE_CONFIG_H -I. -I.. -fPIC -DPIC

all: $(OBJECT)
	$(CC) $(CFLAGS) -DLANGDIR=\"$(LANGDIR)\" $(OBJECT) -o punyconv

i18n.o: ../i18n.c ../i18n.h ../config.h
	$(CC) $(OBOPT) $(CFLAGS) -DLANGDIR=\"$(LANGDIR)\" -c ../i18n.c

idn.o: idn.c stringprep.h punycode.h idna.h
	$(CC) $(OBOPT) $(CFLAGS) -c idn.c

idna.o: idna.c internal.h stringprep.h punycode.h idna.h
	$(CC) $(OBOPT) $(CFLAGS) -c idna.c

nfkc.o: nfkc.c internal.h stringprep.h punycode.h idna.h gunidecomp.h gunicomp.h
	$(CC) $(OBOPT) $(CFLAGS) -c nfkc.c

profiles.o: profiles.c internal.h stringprep.h punycode.h idna.h
	$(CC) $(OBOPT) $(CFLAGS) -c profiles.c

punycode.o: punycode.c internal.h stringprep.h punycode.h idna.h
	$(CC) $(OBOPT) $(CFLAGS) -c punycode.c

stringprep.o: stringprep.c internal.h stringprep.h punycode.h idna.h
	$(CC) $(OBOPT) $(CFLAGS) -c stringprep.c

stringprep_generic.o: stringprep_generic.c stringprep.h
	$(CC) $(OBOPT) $(CFLAGS) -c stringprep_generic.c

toutf8.o: toutf8.c internal.h stringprep.h punycode.h idna.h
	$(CC) $(OBOPT) $(CFLAGS) -c toutf8.c

clean:
	rm -f *.o *~ punyconv
