# $Id: Makefile,v 1.1 2004-02-04 08:29:21 oops Exp $
CC	= gcc
LANGDIR	= /usr/local/share/locale
CFLAGS	= -g -O2
LDFLAGS	=
SOURCE	= race.c
OBJECT	= i18n.o misc.o race.o main.o
OBOPT	= -Wall -DHAVE_CONFIG_H -I. -I..

all: $(OBJECT)
	$(CC) $(LDFLAGS) -DLANGDIR=\"$(LANGDIR)\" $(OBJECT) -o raceconv

i18n.o: ../i18n.c ../i18n.h ../config.h
	$(CC) $(OBOPT) $(CFLAGS) -DLANGDIR=\"$(LANGDIR)\" -c ../i18n.c

main.o: ../config.h race.h main.c
	$(CC) $(OBOPT) $(CFLAGS) -c main.c

race.o: ../config.h misc.h race.h race.c
	$(CC) $(OBOPT) $(CFLAGS) -c race.c

misc.o: ../config.h misc.h misc.c
	$(CC) $(OBOPT) $(CFLAGS) -c misc.c

clean:
	rm -f *.o *~ raceconv
