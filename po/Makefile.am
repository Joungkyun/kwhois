## Process this file with automake to produce Makefile.in
# Copyright (C) 2017 JoungKyun.Kim <http://oops.org>
#
# This file is part of kwhois
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

CLEANFILES = POTFILES *.mo
EXTRA_DIST = default.pot ko.po

all:
	for i in `ls *.po | sed 's/\.po//'`; do \
		$(MSGFMT) $$i.po -o $$i.mo; \
	done

install:
	for i in `ls *.po | sed 's/\.po//'` ; do \
		$(mkdir_p) $(DESTDIR)$(localedir)/$$i/LC_MESSAGES/ ; \
		$(INSTALL) -m644 $$i.mo $(DESTDIR)$(localedir)/$$i/LC_MESSAGES/$(PACKAGE).mo ; \
	done

pot:
	find ../ -name "*.c" > POTFILES
	$(XGETTEXT) -d default -k_ --from-code=utf-8 -s `cat POTFILES`
	mv default.po default.pot

merge:
	for i in `ls *.po`; do \
		$(MSGMERGE) -U $$i default.pot; \
		rm -f $$i~; \
	done
