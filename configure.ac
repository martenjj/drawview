############################################## -*- mode:autoconf; -*- ####
##									##
##  Project:	DrawView						##
##  Edit:	24-May-21						##
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

AC_INIT([DrawView],[1.10],[jjm@keelhaul.me.uk])
AC_DEFINE([PACKAGE_URL],["http://www.keelhaul.me.uk/acorn/drawview"],[The package's home page URL])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
# Dependency tracking does not work with 'utils'
AM_INIT_AUTOMAKE([foreign subdir-objects no-dependencies])

# Additional arguments accepted
AC_ARG_WITH([qt5],[  --with-qt5=DIR          specify Qt 4 base location])
AC_ARG_WITH([qt5-libs],[  --with-qt5-libs=DIR     specify Qt 4 library location])
AC_ARG_WITH([qt5-includes],[  --with-qt5-includes=DIR specify Qt 4 include location])

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

AC_ARG_VAR([MOC],[Qt 5 'moc' program])
AC_ARG_VAR([UIC],[Qt 5 'uic' program])

# Checks for header files
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h fcntl.h strings.h math.h sys/time.h unistd.h limits.h])

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
case "`basename ${CXX}`"
in
g++)	rpathopt="-Wl,-R"		;;
*)	rpathopt="-R"			;;
esac
AC_MSG_RESULT([${rpathopt}])

# Global compiler flags
CPPFLAGS="${CPPFLAGS} -W -Wall -Wredundant-decls -Wunused-macros"
CXXFLAGS="${CXXFLAGS} --std=c++11 -Wno-deprecated -fno-exceptions"

# Checks for Qt
AC_PATH_PROG([qt5binprog],[qlalr],[],[${with_qt5:+${with_qt5}/bin:}$PATH:/usr/qt/5/bin:/usr/lib/qt5/bin:${srcdir}/qt5/bin:${srcdir}/../qt5/bin:${prefix}/bin])

AC_MSG_CHECKING([where Qt 5 is installed])
if test "${qt5binprog}" = ""
then
	AC_MSG_ERROR([Qt 5 not found - specify with '--with-qt5'])
fi
qt5dir1=`echo "${qt5binprog}" | sed -e 's,/qlalr$,,'`
qt5dir=`cd ${qt5dir1} && cd .. && pwd`
if test "${qt5dir}" = ""
then
	AC_MSG_ERROR([Qt 5 directory '${qt5dir1}' not accessible - check permissions])
fi
AC_MSG_RESULT([${qt5dir}])
AC_SUBST([qt5dir])

AC_CHECK_FILE([${qt5dir}/bin],,AC_MSG_ERROR([Qt 5 binary directory not found - check installation]))

#                         here on FC9 x64 ----------------------+
#              presumably here on FC9 32-bit --+                |
#                                              |                |
#                                              v                v
AC_PATH_PROG([MOC],[moc],,[${qt5dir}/bin:$PATH:/usr/lib/qt5/bin:/usr/lib64/qt5/bin])
test "X${MOC}" != "X" || AC_MSG_ERROR([Qt 5 'moc' program not found - check installation])
AC_PATH_PROG([UIC],[uic],,[${qt5dir}/bin:$PATH:/usr/lib/qt5/bin:/usr/lib64/qt5/bin])
test "X${UIC}" != "X" || AC_MSG_ERROR([Qt 5 'uic' program not found - check installation])

# AC_PATH_PROG checks that the found target is executable.  This may fail for libraries
# and include files.
#AC_PATH_PROG([qt5lib],[libQtCore.so],[],[${with_qt5_libs:+${with_qt5_libs}:}${with_qt5:+${with_qt5}/lib:}/usr/qt/4/lib:/usr/lib/qt5:/usr/lib:/usr/lib64:${srcdir}/qt5/lib:${srcdir}/../qt5/lib:${prefix}/lib])

qtcorelib="libQt5Core.so"
qt5lib=
for dir in ${with_qt5_libs:+${with_qt5_libs}} ${with_qt5:+${with_qt5}/lib} ${qt5dir}/lib /usr/qt/4/lib /usr/lib/qt5 /usr/lib /usr/lib64 ${srcdir}/qt5/lib ${srcdir}/../qt5/lib ${prefix}/lib
do
	test "X${dir}" = "X" && continue
	test -d "${dir}" || continue
	AC_MSG_CHECKING([for ${dir}/${qtcorelib}])
	if test -f "${dir}/${qtcorelib}"
	then
		qt5lib="${dir}"
		AC_MSG_RESULT([yes])
		break
	else
		AC_MSG_RESULT([no])
	fi
done
if test "${qt5lib}" = ""
then
	AC_MSG_ERROR([Qt 5 library directory not found - specify with '--with-qt5-libs'])
fi

AC_MSG_CHECKING([Qt 5 library directory])
qt5lib=`cd ${qt5lib} && pwd`
if test "${qt5lib}" = ""
then
	AC_MSG_ERROR([Qt 5 library directory '${qt5lib1}' not accessible - check permissions])
fi
AC_MSG_RESULT([${qt5lib}])
AC_SUBST([qt5lib])

qt5inc=
for dir in ${with_qt5_includes:+${with_qt5_includes}} ${with_qt5:+${with_qt5}/include } ${qt5dir}/include /usr/qt/5/include /usr/include/qt5 /usr/include ${srcdir}/qt5/include ${srcdir}/../qt5/include ${prefix}/include
do
	test "X${dir}" = "X" && continue
	test -d "${dir}" || continue

	qtcoreinc="QtCore"
	AC_MSG_CHECKING([for ${dir}/${qtcoreinc}])
	if test -d "${dir}/${qtcoreinc}"
	then
		qt5inc="${dir}"
		AC_MSG_RESULT([yes])
		break
	else
		AC_MSG_RESULT([no])

		qtcoreinc="Qt/QtCore"
		AC_MSG_CHECKING([for ${dir}/${qtcoreinc}])
		if test -d "${dir}/${qtcoreinc}"
		then
			qt5inc="${dir}"
			AC_MSG_RESULT([yes])
			break
		else
			AC_MSG_RESULT([no])
		fi
	fi
done

if test "${qt5inc}" = ""
then
	AC_MSG_ERROR([Qt 5 include directory not found - specify with '--with-qt5-includes'])
fi

AC_MSG_CHECKING([Qt 5 include directory])
#qt5inc1=`echo "${qt5inc}" | sed -e 's,/[[^/]]*$,,'`
qt5inc=`cd ${qt5inc} && pwd`
if test "${qt5inc}" = ""
then
	AC_MSG_ERROR([Qt 5 include directory '${qt5inc1}' not accessible - check permissions])
fi
AC_MSG_RESULT([${qt5inc}])
AC_SUBST([qt5inc])

qtcomps="QtCore QtGui QtWidgets QtPrintSupport QtSvg"
for arg in ${qtcomps}
do
	AC_CHECK_FILE([${qt5inc}/${arg}],,AC_MSG_ERROR([Qt 5 include directory <${arg}> not found - check installation]))
	arg=`echo "${arg}" | sed -e 's/Qt/Qt5/'`
	qt5comps="${qt5comps} ${arg}"
done

AC_PATH_PROG([pkgconfigprog],[pkg-config],[])
if test "${pkgconfigprog}" = ""
then
	AC_MSG_ERROR([Program 'pkg-config' not found])
fi

AC_MSG_CHECKING([required Qt 5 compiler includes])
temp=`PKG_CONFIG_PATH=${qt5lib}/pkgconfig:${qt5dir}/lib/pkgconfig; export PKG_CONFIG_PATH; ${pkgconfigprog} --cflags ${qt5comps}`
qt5cxxflags="-I${qt5inc} -I${qt5inc}/Qt"
for arg in ${temp} ""
do
	if test "X${arg}" != "X"
	then
		case "X${arg}"
		in
X-I*)			arg=`echo "${arg}" | sed -e 's,^-I,,'`
			test -d "${arg}" && qt5cxxflags="${qt5cxxflags} -I${arg}"
			test -d "${arg}/Qt" && qt5cxxflags="${qt5cxxflags} -I${arg}/Qt"	;;
*)			qt5cxxflags="${qt5cxxflags} ${arg}"				;;
		esac
	fi
done
AC_MSG_RESULT([${qt5cxxflags}])

AC_MSG_CHECKING([for additional compile flags])
qt5extflags="-DQT4 -DQT5 -g"
AC_MSG_RESULT([${qt5extflags}])

# Even if Qt needs -fPIC (i.e. it was built with '-reduce-relocations',
# this does not seem to be included in the result of 'pkg-config --cflags'.
# Check for it manually here.
AC_CHECK_FILE([${qt5inc}/QtCore/qconfig.h],,AC_MSG_ERROR([Qt 5 include file <QtCore/qtconfig.h> not found - check installation]))
AC_MSG_CHECKING([whether Qt 5 needs -fPIC])
if grep "^.define .*QT_REDUCE_RELOCATIONS" ${qt5inc}/QtCore/qconfig.h >/dev/null
then
	AC_MSG_RESULT([yes])
	qt5extflags="${qt5extflags} -fPIC"
else
	AC_MSG_RESULT([no])
fi

qt5cxxflags="${qt5cxxflags} ${qt5extflags}"
AC_SUBST([qt5cxxflags])
CXXFLAGS="${CXXFLAGS} ${qt5cxxflags}"

AC_MSG_CHECKING([required Qt 5 linker flags])
temp=`PKG_CONFIG_PATH=${qt5lib}/pkgconfig:${qt5dir}/lib/pkgconfig; export PKG_CONFIG_PATH; ${pkgconfigprog} --libs ${qt5comps}`
qt5ldflags="-L${qt5lib}"
test "X${rpathopt}" != "X" && qt5ldflags="${qt5ldflags} ${rpathopt}${qt5lib}"

qt5inplace=
#qt5libseen=

for arg in ${temp} ""
do
	if test "X${arg}" != "X"
	then
		case "X${arg}"
		in
X-L*)			dir=`echo "${arg}" | sed -e 's,^-L,,'`
			if test -d "${dir}/../src/corelib"
			then
				qt5inplace="${dir}"
				continue
			fi
#			if test "`ls -1 ${dir}/libQtCore* 2>/dev/null`" != ""
#			then
#				qt5libseen="yes"
#			fi
			qt5ldflags="${qt5ldflags} -L${dir}"
			test "X${rpathopt}" != "X" && qt5ldflags="${qt5ldflags} ${rpathopt}${dir}"
			;;

X-l*)			qt5libs="${qt5libs} ${arg}"
			;;

*)			qt5ldflags="${qt5ldflags} ${arg}"
			;;
		esac
	fi
done
AC_MSG_RESULT([${qt5ldflags}])
AC_SUBST([qt5ldflags])

AC_MSG_CHECKING([required Qt 5 libraries])
AC_MSG_RESULT([${qt5libs}])
AC_SUBST([qt5libs])

AC_MSG_CHECKING([whether Qt 5 installation was in-place])
if test "X${qt5inplace}" = "X"
then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])

	AC_MSG_CHECKING([additional Qt 5 linker flags])
	qt5ldflags="${qt5ldflags} -L${qt5inplace}"
	test "X${rpathopt}" != "X" && qt5ldflags="${qt5ldflags} ${rpathopt}${qt5inplace}"
	AC_MSG_RESULT([${qt5ldflags}])
fi
LDFLAGS="${LDFLAGS} ${qt5libs} ${qt5ldflags}"

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
		AC_PATH_PROG([kdeconfig],[kf5-config],[],[${kdeinst:+${kdeinst}/bin:}$PATH:/usr/local/kde/bin:/usr/kde/bin])
		if test "$kdeconfig" = ""
		then
			if test "${with_kde}" != ""
			then
				AC_MSG_ERROR([KDE 'kde-config' or 'kde4-config' program not found for '--with-kde'])
			fi
			kdeicons=
			kdemime=
		fi
	fi

	if test "$kdeconfig" != ""
	then
		AC_MSG_CHECKING([location for KDE icons])
		kdeicons=`${kdeconfig} --expandvars --install icon`
		AC_MSG_RESULT([${kdeicons}])
		AC_MSG_CHECKING([location for KDE mimetypes])
		kdemime=`${kdeconfig} --expandvars --install mime`

		if test -n "${kdemime}"
		then
			AC_MSG_RESULT([${kdemime}])
		else
			AC_MSG_RESULT([not found])

			xdd=
			AC_MSG_CHECKING([for XDG_DATA_DIRS])
			if test -n "$XDG_DATA_DIRS"
			then
				xdd="$XDG_DATA_DIRS"
				AC_MSG_RESULT([set to ${xdd}])
			else
				xdd="/usr/share"
				AC_MSG_RESULT([assuming ${xdd}])
			fi

			smi=
			AC_MSG_CHECKING([for XDG shared MIME info])
			for d in `echo "$xdd" | sed -e 's/:/ /g'` ""
			do
				test -n "$d" || continue
				if test -d "$d/mime/packages"
				then
					smi="$d/mime"
					break
				fi
			done
			if test -n "$smi"
			then
				AC_MSG_RESULT([${smi}])
				kdemime="${smi}"
			else
				AC_MSG_RESULT([not found])
			fi
		fi

		if test -n "${kdemime}"
		then
			AC_MSG_CHECKING([whether KDE uses the freedesktop MIME database])
			if test -d "`dirname ${kdemime}`/mime"
			then
				AC_MSG_RESULT([yes])

				AC_MSG_CHECKING([location for KDE freedesktop MIME database])
				# Using "${kdeconfig} --install xdgdata-apps" returns nothing
				kdemimedb="`dirname ${kdemime}`/mime/packages/"
				AC_MSG_RESULT([${kdemimedb}])
				kdemime=
			else
				AC_MSG_RESULT([no])
			fi
		fi

		AC_MSG_CHECKING([location for KDE applications])
		kdeapps=`${kdeconfig} --expandvars --install xdgdata-apps`
		AC_MSG_RESULT([${kdeapps}])

		AC_MSG_CHECKING([whether to install KDE desktop files for MIME])
		if test "$kdemime" != ""
		then
			AC_MSG_RESULT([yes])
		else
			AC_MSG_RESULT([no])
		fi
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

AC_MSG_CHECKING([whether KDE integration will be installed])
if test "${kdeicons}" = ""
then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])
	AC_SUBST([kdeicons])
	AC_SUBST([kdemime])
	AC_SUBST([kdemimedb])
	AC_SUBST([kdeapps])
	CPPFLAGS="${CPPFLAGS} -DKDEICONS=\\\"${kdeicons}\\\""
fi

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

AC_MSG_CHECKING([whether desktop integration will be installed])
if test "${desktopicons}" = ""
then
	AC_MSG_RESULT([no])
else
	AC_MSG_RESULT([yes])
	AC_SUBST([desktopapps])
	AC_SUBST([desktopicons])
	CPPFLAGS="${CPPFLAGS} -DDESKTOPICONS=\\\"${desktopicons}\\\""
fi

# Output files
AC_CONFIG_FILES([Makefile
                 app/Makefile
                 filter/Makefile
                 share/Makefile
                 examples/Makefile
                 desktop/Makefile
                 po/Makefile])
AC_OUTPUT
