#!/bin/sh
#
# $Id$
# require autoconf > 2.59 & automake > 1.8

#aclocal --force >& /dev/null && autoconf --force && autoheader --force && libtoolize --force
#rm -rf autom4te* *~ aclocal.m4 config.guess config.sub ltmain.sh
#touch install-sh
#
#for i in config.guess config.sub ltmain.sh
#do
#   cp -af /usr/share/libtool/${i} ./
#done

export LIBTOOL=/usr/bin/libtool
export AUTOMAKE=/usr/bin/automake
export ACLOCAL=/usr/bin/aclocal
rm -f configure autotool/{config.guess,config.sub,ltmain.sh}
autoreconf --install
rm -rf autom4te* *~

exit 0
