# $Id: Makefile,v 1.1 2004-02-04 08:29:21 oops Exp $
CFLAGS  =
DESTDIR =

PROG    = kwhois
langdir = /usr/share/locale

INSTALL = /usr/bin/install
LS      = /bin/ls
RM      = /bin/rm -f
SED     = /bin/sed
MKDIR   = /bin/mkdir -p
MSGFMT  = /usr/bin/msgfmt

all:
	for i in `$(LS) *.po | $(SED) 's/\.po//'` ; do \
		$(MSGFMT) $$i.po -o $$i.mo ; \
	done

install:
	for i in `$(LS) *.po | $(SED) 's/\.po//'` ; do \
		$(MKDIR) $(DESTDIR)$(langdir)/$$i/LC_MESSAGES/ ; \
		$(INSTALL) -m644 $$i.mo $(DESTDIR)$(langdir)/$$i/LC_MESSAGES/$(PROG).mo ; \
	done

clean:
	-$(RM) *.mo
