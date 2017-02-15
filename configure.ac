# Process this file with autoconf to produce a configure script.
# Configure template for kwhois
#
# $Id$

AC_PREREQ(2.59)
AC_INIT([kwhois], [4.7], [https://github.com/Joungkyun/kwhois/issues])

AC_CONFIG_AUX_DIR([tools])
AM_INIT_AUTOMAKE([-Wall -Werror -Wno-override foreign no-dependencies])
AM_MAINTAINER_MODE

AC_CONFIG_SRCDIR([i18n.h])
AC_CONFIG_HEADER([config.h])
AC_PREFIX_DEFAULT([/usr/local])

AC_DEFUN([AC_DATAROOTDIR_CHECKED])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_PROG_MAKE_SET
AM_PO_SUBDIRS

# Checks for libraries.
AC_CHECK_LIB(resolv,res_mkquery)
AX_OLIBC
AM_ICONV

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(
	sys/signal.h sys/socket.h getopt.h limits.h netinet/in.h \
	arpa/inet.h arpa/nameser.h netdb.h ctype.h langinfo.h \
	locale.h libintl.h errno.h error.h iconv.h
)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_STAT
AC_CHECK_FUNCS(getaddrinfo)
AC_CHECK_FUNCS(getopt_long,, [AC_LIBOBJ(getopt) AC_LIBOBJ(getopt1)])

AC_CONFIG_FILES([Makefile
                 po/Makefile
				 docs/Makefile
                 libidn/Makefile])

AC_OUTPUT
