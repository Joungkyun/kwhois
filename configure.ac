# $Id: configure.ac,v 1.3 2004-06-03 12:10:46 oops Exp $
AC_INIT(kwhois, 3.2, http://www.oops.org)
AC_CONFIG_SRCDIR([i18n.h])
AC_CONFIG_HEADER([config.h])
AC_PREFIX_DEFAULT([/usr/local])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_AWK
AC_PATH_PROG(GREP, grep)
if test ! -f "$ac_cv_path_GREP"; then
  AC_MSG_ERROR(Can't not found grep)
fi

AC_PATH_PROG(SED, sed)
if test ! -f "$ac_cv_path_SED"; then
  AC_MSG_ERROR(Can't not found sed)
fi

AC_PATH_PROG(LS, ls)
if test ! -f "$ac_cv_path_LS"; then
  AC_MSG_ERROR(Can't not found ls)
fi

AC_PATH_PROG(RM, rm)
if test ! -f "$ac_cv_path_RM"; then
  AC_MSG_ERROR(Can't not found rm)
fi

AC_PATH_PROG(MKDIR, mkdir)
if test ! -f "$ac_cv_path_MKDIR"; then
  AC_MSG_ERROR(Can't not found mkdir)
fi

AC_PATH_PROG(MSGFMT, msgfmt)
AC_ARG_WITH(msgfmt,
[  --with-msgfmt=PATH      path of msgfmt program])
if test "x$with_msgfmt" != "x"; then
  if test -f $with_msgfmt; then
    ac_cv_path_MSGFMT=$with_msgfmt
  fi
else
  if test "x$ac_cv_path_MSGFMT" != "x"; then
    if test -f $ac_cv_path_MSGFMT ; then
      ac_cv_path_MSGFMT=$with_msgfmt
    else
      AC_MSG_ERROR(Can't not found msgfmt)
    fi
  else
    AC_MSG_ERROR(Can't not found msgfmt)
  fi
fi

# Checks for libraries.
AC_CHECK_LIB(resolv,res_mkquery)
AC_CHECK_LIB(ogc, convert_punycode)

# Checks for header files.
AC_CHECK_HEADERS(stdio.h string.h ctype.h limits.h sys/types.h sys/socket.h \
		 stdlib.h unistd.h netinet/in.h arpa/nameser.h netdb.h errno.h \
		 sys/signal.h getopt.h libintl.h locale.h dlfcn.h error.h inttypes.h \
		 memory.h stdint.h sys/stat.h iconv.h langinfo.h olibc/libstring.h)

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_UID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS(getaddrinfo)
AC_CHECK_FUNCS(getopt_long,, [AC_LIBOBJ(getopt) AC_LIBOBJ(getopt1)])

AC_CONFIG_FILES([Makefile
                 po/Makefile
                 libidn/Makefile
                 libracode/Makefile])

AC_OUTPUT
