dnl Copyright (C) 2011 JoungKyun.Kim <http://oops.org>
dnl
dnl This file is part of libkrisp
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU Lesser General Public License as published
dnl by the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU Lesser General Public License
dnl along with this program.  If not, see <http://www.gnu.org/licenses/>.
dnl
dnl $Id$

AC_DEFUN([AX_OLIBC],
[
	AC_ARG_WITH(olibc,
		[  --with-olibc=PATH       prefix of olibc [[default: /usr]]])

	if test "x$with_olibc" != "xno"; then
		if test "x$with_olibc" != "x"; then
			if test "x$with_olibc" = "xyes"; then
				olibc_prefix="/usr"
			else
				olibc_prefix="$with_olibc"
			fi
		else
			olibc_prefix="/usr"
		fi

		olibc_incdir="$olibc_prefix/include"
		olibc_cppflags="-I$olibc_incdir"
		if test -f "$olibc_prefix/lib64/libolibc.so"; then
			olibc_libdir="$olibc_prefix/lib64"
		elif test -f "$olibc_prefix/lib64/libolibc.a"; then
			olibc_libdir="$olibc_prefix/lib64"
		else
			olibc_libdir="$olibc_prefix/lib"
		fi

		olibc_ldflags="-L$olibc_libdir"

		#
		# olibc library check
		#
		olibc_OLD_FLAGS="$LDFLAGS"
		LDFLAGS="$olibc_ldflags"

		AC_CHECK_LIB(
			ogc, convert_punycode, [
				AC_DEFINE(
					[HAVE_LIBOGC], 1,
					[Define to 1 if you have the `ogc' library (-logc).]
				)
				LIBS="-logc $LIBS"
				AC_CHECK_LIB(
					ogc, join, [
						AC_DEFINE(
							[HAVE_LIBOC_VER], 1,
							[Define to 0 if you have over olibc 1.0.0]
						)
					],[
						AC_DEFINE(
							[HAVE_LIBOC_VER], 0,
							[Define to 1 if you have under olibc 1.0.0]
						)
					]
				)
			],[
				AC_MSG_ERROR([Error.. you must need over olibc 1.0.0!])
			]
		)

		LDFLAGS="$olibc_OLD_LDFLAGS"

		ret_ldflags=0
		for check_ldflags in $LDFLAGS
		do
			test -z "$olibc_prefix" && break
	
			if test "$check_ldflags" = "$olibc_ldflags"; then
				ret_ldflags=1
				break
			fi
		done

		test $ret_ldflags -eq 0 && LDFLAGS="$olibc_ldflags $LDFLAGS"

		#
		# olibc header check
		#
		ret_cppflags=0
		for check_cppflags in $CPPFLAGS
		do
			test -z "$olibc_prefix" && break

			if test "$check_cppflags" = "$olibc_cppflags"; then
				ret_cppflags=1
				break
			fi
		done
		test $ret_cppflags -eq 0 && CPPFLAGS="$olibc_cppflags $CPPFLAGS"
		AC_CHECK_HEADERS(olibc/libidn.h)
		if test "$ac_cv_header_olibc_libidn_h" != "yes"; then
			AC_MSG_ERROR([You must need olibc/libidn.h header file!])
		fi
	fi
])
