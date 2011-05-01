# $Id$
AC_PREREQ(2.57)

AC_INIT(kwhois, 3.5, http://devel.oops.org)
AC_CONFIG_SRCDIR([i18n.h])
AC_CONFIG_HEADER([config.h])
AC_PREFIX_DEFAULT([/usr/local])

AC_ARG_WITH(rrd,
[  --with-msgfmt=PATH      path of msgfmt binary [[default]]])

AC_ARG_WITH(olibc,
[  --with-olibc=PATH       path of olibc library. [[default]]])

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

AC_PATH_PROG(MSGFMT, msgfmt,, $PATH:$with_msgfmt:$with_msgfmt/bin)
if test "x$ac_cv_path_MSGFMT" = "x"; then
  AC_MSG_ERROR(Can't not found msgfmt)
fi


# Checks for libraries.
AC_CHECK_LIB(resolv,res_mkquery)

# olibc library check
if test "x$with_olibc" != "x"; then
  if test -f "$with_olibc" ; then
    chk_ext=`echo $with_olibc | ${SED} 's!.*\.\([^.]\+\)$!\1!g'`
    if test "$chk_ext" = "so" -a "$chk_ext" = "a"; then
      with_olibc=`echo $with_olibc | ${SED} 's!/[^/]*$!!g'`
      olibclib="-L$with_olibc"
      dnl CHECK_RPATH(olibclib, $with_olibc)

      olibcchk=`echo $with_olibc | ${GERP} '/lib$'`
      if test -n "olibcdchk"; then
        with_olibc=`echo $with_olibc | ${SED} 's!/lib$!!g'`
      fi
    fi
  elif test -d "$with_olibc" ; then
    if test -f $with_olibc/lib/libogc.a -o -f $with_olibc/lib/libogc.so; then
      olibclib="-L$with_olibc/lib"
      dnl CHECK_RPATH(olibclib, $with_olibc/lib)
    fi
  else
    olibclib=
  fi

  LDFLAGS="$olibclib $LDFLAGS"
fi
AC_CHECK_LIB(ogc, convert_punycode,, [AC_MSG_ERROR(Need over Oops C Library 0.1.0)])

# olibc header check
if test "$ac_cv_lib_ogc_convert_punycode" = "yes" ; then
  for i in $with_olibc /usr /usr/local
  do
    if test -f "$i/include/libidn.h"; then
      inschk=`echo $CPPFLAGS | ${GREP} "I$i/include "`
      if test -z "$inschk"; then
        CPPFLAGS="-I$i/include $CPPFLAGS"
      fi
      break
    elif test -f "$i/include/olibc/libidn.h"; then
      inschk=`echo $CPPFLAGS | ${GREP} "I$i/include/olibc "`
      if test -z "$inschk"; then
        CPPFLAGS="-I$i/include/olibc $CPPFLAGS"
      fi
      break
    fi
  done
fi

# Checks for header files.
AC_CHECK_HEADERS(stdio.h string.h ctype.h limits.h sys/types.h sys/socket.h \
		 stdlib.h unistd.h netinet/in.h arpa/inet.h arpa/nameser.h netdb.h errno.h \
		 sys/signal.h getopt.h libintl.h locale.h dlfcn.h error.h inttypes.h \
		 memory.h stdint.h sys/stat.h iconv.h langinfo.h libidn.h $geo_header)

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
