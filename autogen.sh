# $Id: autogen.sh,v 1.5 2004-08-10 09:38:06 oops Exp $
autoheader --force && autoconf --force
rm -rf autom4te* *~
