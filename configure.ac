############################################## -*- mode:autoconf; -*- ####
##									##
##  Project:	DrawView						##
##  SCCS:	<%Z% %M% %I%>					##
##  Edit:	13-Oct-06						##
##									##
##########################################################################
##									##
##  Autoconf configuration template for the package.			##
##									##
##  Process this file with autoconf to produce the 'configure' script.	##
##									##
##########################################################################
##									##
##  Copyright (c) 2006 Jonathan Marten <jjm@keelhaul.demon.co.uk>	##
##  Home & download:  http://www.keelhaul.demon.co.uk/acorn/drawview/	##
##									##
##  This program is free software; you can redistribute it and/or	##
##  modify it under the terms of the GNU General Public License as	##
##  published by the Free Software Foundation; either version 2 of	##
##  the License, or (at your option) any later version.			##
##									##
##  It is distributed in the hope that it will be useful, but		##
##  WITHOUT ANY WARRANTY; without even the implied warranty of		##
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	##
##  GNU General Public License for more details.			##
##									##
##  You should have received a copy of the GNU General Public		##
##  License along with this program; see the file COPYING for further	##
##  details.  If not, write to the Free Software Foundation, Inc.,	##
##  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.		##
##									##
##########################################################################

AC_PREREQ(2.59)

AC_INIT([DrawView],[1.02],[jjm@keelhaul.me.uk])
AC_DEFINE([PACKAGE_URL],["http://www.keelhaul.me.uk/acorn/drawview"],[The package's home page URL])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Additional arguments accepted
AC_ARG_WITH([qt4],[  --with-qt4=DIR          specify Qt 4 base location])
AC_ARG_WITH([qt4-libs],[  --with-qt4-libs=DIR     specify Qt 4 library location])
AC_ARG_WITH([qt4-includes],[  --with-qt4-includes=DIR specify Qt 4 include location])

AC_ARG_WITH([cmdutils],[  --with-cmdutils=DIR     specify command utilities source location])
AC_ARG_WITH([riscos],[  --with-riscos=DIR       specify RiscOS library source location])
AC_ARG_WITH([kde],[  --with-kde=DIR          specify KDE integration install location])
AC_ARG_WITH([desktop],[  --with-desktop=DIR      specify freedesktop integration install location])

# Checks for programs
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_CHECK_PROG([RMDIR],[rmdir],[rmdir])

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h fcntl.h strings.h math.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_VOLATILE

AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([double])

# Checks for library functions
AC_FUNC_ERROR_AT_LINE
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memmove memset strchr strerror])

## Checks for libraries
AC_CHECK_LIB([dl],[dlsym])
AC_CHECK_LIB([m],[ldexp],,AC_MSG_ERROR([Library function 'ldexp' is required]))

# Checks for linker syntax
AC_MSG_CHECKING([how to pass runpath option to the linker])
case "${CXX}"
in
g++)	rpathopt="-Wl,-R"		;;
*)	rpathopt="-R"			;;
esac
AC_MSG_RESULT([${rpathopt}])

# Global compiler flags
CPPFLAGS="-W -Wall -Wredundant-decls -Wunused-macros"
CXXFLAGS="-Wno-deprecated -fno-exceptions"

# Checks for Qt
AC_PATH_PROG([qt4binprog],[qt3to4],[],[${with_qt4:+${with_qt4}/bin:}$PATH:/usr/qt/4/bin:/usr/lib/qt4/bin:${srcdir}/qt4/bin:${srcdir}/../qt4/bin:${prefix}/bin])

AC_MSG_CHECKING([where Qt 4 is installed])
if test "${qt4binprog}" = ""
then
	AC_MSG_ERROR([Qt 4 not found - specify with '--with-qt4'])
fi
qt4dir1=`echo "${qt4binprog}" | sed -e 's,/qt3to4$,,'`
qt4dir=`cd ${qt4dir1} && cd .. && pwd`
if test "${qt4dir}" = ""
then
	AC_MSG_ERROR([Qt 4 directory '${qt4dir1}' not accessible - check permissions])
fi
AC_MSG_RESULT([${qt4dir}])
AC_SUBST([qt4dir])

AC_CHECK_FILE([${qt4dir}/bin],,AC_MSG_ERROR([Qt 4 binary directory not found - check installation]))
AC_PATH_PROG([MOC],[moc],AC_MSG_ERROR([Qt 4 'moc' program not found - check installation]),[${qt4dir}/bin])
AC_PATH_PROG([UIC],[uic],AC_MSG_ERROR([Qt 4 'uic' program not found - check installation]),[${qt4dir}/bin])

AC_PATH_PROG([qt4lib],[libQtCore.so],[],[${with-qt4-libs:+${with_qt4_libs}:}${with_qt4:+${with_qt4}/lib:}/usr/qt/4/lib:/usr/lib/qt4:${srcdir}/qt4/lib:${srcdir}/../qt4/lib:${prefix}/lib])
if test "${qt4lib}" = ""
then
	AC_MSG_ERROR([Qt 4 library directory not found - specify with '--with-qt4-libs'])
fi

AC_MSG_CHECKING([for Qt 4 library directory])
qt4lib1=`echo "${qt4lib}" | sed -e 's,/[[^/]]*$,,'`
qt4lib=`cd ${qt4lib1} && pwd`
if test "${qt4lib}" = ""
then
	AC_MSG_ERROR([Qt 4 library directory '${qt4lib1}' not accessible - check permissions])
fi
AC_MSG_RESULT([${qt4lib}])
AC_SUBST([qt4lib])


AC_PATH_PROG([qt4inc],[Qt/QtCore],[],[${with-qt4-includes:+${with_qt4_includes}:}${with_qt4:+${with_qt4}/include:}/usr/qt/4/include:/usr/include/qt4:/usr/include:${srcdir}/qt4/include:${srcdir}/../qt4/include:${prefix}/include])
if test "${qt4inc}" = ""
then
	AC_MSG_ERROR([Qt 4 include directory not found - specify with '--with-qt4-includes'])
fi

AC_MSG_CHECKING([for Qt 4 include directory])
qt4inc1=`echo "${qt4inc}" | sed -e 's,/[[^/]]*$,,'`
qt4inc=`cd ${qt4inc1} && pwd`
if test "${qt4inc}" = ""
then
	AC_MSG_ERROR([Qt 4 include directory '${qt4inc1}' not accessible - check permissions])
fi
AC_MSG_RESULT([${qt4inc}])
AC_SUBST([qt4inc])

#AC_CHECK_FILE([${qt4inc}/QtCore],,AC_MSG_ERROR([Qt 4 include directory not found - check installation]))
AC_CHECK_FILE([${qt4inc}/QtCore],,AC_MSG_ERROR([Qt 4 include file <QtCore> not found - check installation]))
AC_CHECK_FILE([${qt4inc}/QtGui],,AC_MSG_ERROR([Qt 4 include file <QtGui> not found - check installation]))

AC_PATH_PROG([pkgconfigprog],[pkg-config],[])
if test "${pkgconfigprog}" = ""
then
	AC_MSG_ERROR([Program 'pkg-config' not found])
fi

AC_MSG_CHECKING([required Qt 4 compiler flags])
temp=`PKG_CONFIG_PATH=${qt4dir}/lib; export PKG_CONFIG_PATH; ${pkgconfigprog} --cflags QtGui`
qt4cxxflags=
for arg in ${temp} ""
do
	if test "X${arg}" != "X"
	then
		case "X${arg}"
		in
X-I*)			qt4cxxflags="${qt4cxxflags} ${arg} ${arg}/Qt"	;;
*)			qt4cxxflags="${qt4cxxflags} ${arg}"		;;
		esac
	fi
done
AC_MSG_RESULT([${qt4cxxflags}])
AC_SUBST([qt4cxxflags])
CXXFLAGS="${CXXFLAGS} -DQT4 ${qt4cxxflags}"

AC_MSG_CHECKING([required Qt 4 linker flags])
temp=`PKG_CONFIG_PATH=${qt4dir}/lib; export PKG_CONFIG_PATH; ${pkgconfigprog} --libs QtGui`
qt4ldflags=
for arg in ${temp} ""
do
	if test "X${arg}" != "X"
	then
		case "X${arg}"
		in
X-L*)			dir=`echo "${arg}" | sed -e 's,^-L,,'`
			if test -d "${dir}/../src/corelib"
			then
				continue
			fi
			qt4ldflags="${qt4ldflags} -L${dir}"
			test "X${rpathopt}" != "X" && qt4ldflags="${qt4ldflags} ${rpathopt}${dir}"
			;;

*)			qt4ldflags="${qt4ldflags} ${arg}"
			;;
		esac
	fi
done
AC_MSG_RESULT([${qt4ldflags}])
AC_SUBST([qt4ldflags])

# Checks for external sources
AC_MSG_CHECKING([location of command utility sources])
if test "X${with_cmdutils}" = "X"
then
	dir1="${srcdir}/utils"
	dir2="/ws/scm-tools/src/utils"
else
	dir1="${with_cmdutils}"
	dir2="${srcdir}/utils"
fi
AC_MSG_RESULT([trying ${dir1}:${dir2}])
AC_CHECK_FILE([${dir1}/cmdutils.c],[dir="${dir1}"],
	AC_CHECK_FILE([${dir2}/cmdutils.c],[dir="${dir2}"],
		AC_MSG_ERROR([Command utility sources not found - specify with '--with-cmdutils'])
))
AC_MSG_CHECKING([for command utility sources])
utilsrc=`cd "${dir}" && pwd`
AC_MSG_RESULT([${utilsrc}])
AC_SUBST([utilsrc])

AC_MSG_CHECKING([location of RiscOS library sources])
if test "X${with_riscoslib}" = "X"
then
	dir1="${srcdir}/riscos"
	dir2="/ws/acorn/lib"
else
	dir1="${with_riscoslib}"
	dir2="${srcdir}/riscos"
fi

AC_MSG_RESULT([trying ${dir1}:${dir2}])
AC_CHECK_FILE([${dir1}/draw/decompress.c],[dir="${dir1}"],
	AC_CHECK_FILE([${dir2}/draw/decompress.c],[dir="${dir2}"],
		AC_MSG_ERROR([RiscOS library sources not found - specify with '--with-riscos'])
))
AC_MSG_CHECKING([for RiscOS library sources])
riscosrc=`cd "${dir}" && pwd`
AC_MSG_RESULT([${riscosrc}])
AC_SUBST([riscosrc])

# Checks for host type
AC_MSG_CHECKING([compile definitions for host type])
case "`uname -s`"
in
"Linux")	hostdefs="-DLINUX"		;;
"SunOS")	hostdefs-"-DSOLARIS"		;;
*)		hostdefs=			;;
esac
AC_MSG_RESULT([${hostdefs}])
AC_SUBST([hostdefs])
CPPFLAGS="${CPPFLAGS} ${hostdefs}"

# Checks for KDE integration install
AC_MSG_CHECKING([whether to install KDE integration])
if test "${with_kde}" = "no"
then
	AC_MSG_RESULT([no])
	kdeicons=
	kdemime=
	kdeapps=
else
	AC_MSG_RESULT([maybe])
	if test "${with_kde}" = "yes"
	then
		kdeinst=
	else
		kdeinst="${with_kde}"
	fi
	AC_PATH_PROG([kdeconfig],[kde-config],[],[${kdeinst:+${kdeinst}/bin:}$PATH:/usr/local/kde/bin:/usr/kde/bin])
	if test "$kdeconfig" = ""
	then
		if test "${with_kde}" != ""
		then
			AC_MSG_ERROR([KDE 'kde-config' program not found for '--with-kde'])
		fi
		kdeicons=
		kdemime=
	else
		AC_MSG_CHECKING([location for KDE icons])
		kdeicons=`${kdeconfig} --expandvars --install icon`
		AC_MSG_RESULT([${kdeicons}])
		AC_MSG_CHECKING([location for KDE mimetypes])
		kdemime=`${kdeconfig} --expandvars --install mime`
		AC_MSG_RESULT([${kdemime}])
		AC_MSG_CHECKING([location for KDE applications])
		kdeapps=`${kdeconfig} --expandvars --install xdgdata-apps`
		AC_MSG_RESULT([${kdeapps}])
	fi

	AC_MSG_CHECKING([whether to install desktop integration in addition to KDE])
	if test "${with_desktop}" = ""
	then
		AC_MSG_RESULT([no])
		with_desktop="no"
	else
		AC_MSG_RESULT([maybe])
	fi
fi
AC_SUBST([kdeicons])
AC_SUBST([kdemime])
AC_SUBST([kdeapps])
CPPFLAGS="${CPPFLAGS} -DKDEICONS=\\\"${kdeicons}\\\""

# Checks for desktop install
AC_MSG_CHECKING([whether to install desktop integration])
if test "${with_desktop}" = "no"
then
	AC_MSG_RESULT([no])
	desktopapps=
	desktopicons=
else
	AC_MSG_RESULT([maybe])
	if test "${with_desktop}" = "yes"
	then
		desktopinst=
	else
		desktopinst="${with_desktop}"
	fi
	temp="${desktopinst:-/usr/share}/icons"
	AC_CHECK_FILE([${temp}],[desktopicons="${temp}"],[])
	if test "${desktopicons}" = ""
	then
		if test "${with_desktop}" != ""
		then
			AC_MSG_ERROR([Desktop icon directory not found for '--with-desktop'])
		fi
	fi
	desktop=`dirname "${desktopicons}"`

	AC_MSG_CHECKING([location for desktop applications])
	desktopapps="${desktop}/applications"
	AC_MSG_RESULT([${desktopapps}])
	AC_MSG_CHECKING([location for desktop icons])
	desktopicons="${desktop}/icons"
	AC_MSG_RESULT([${desktopicons}])
fi
AC_SUBST([desktopapps])
AC_SUBST([desktopicons])
CPPFLAGS="${CPPFLAGS} -DDESKTOPICONS=\\\"${desktopicons}\\\""

# Output files
AC_CONFIG_FILES([Makefile
                 app/Makefile
                 filter/Makefile
                 share/Makefile
                 examples/Makefile
                 desktop/Makefile
                 po/Makefile])
AC_OUTPUT
