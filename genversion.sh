#!/bin/sh
##########################################################################
##									##
##  This script is part of KRepton.					##
##									##
##  This file may be distributed and/or modified under the terms of	##
##  the GNU General Public License version 2 or any later version,	##
##  as published by the	Free Software Foundation and appearing in	##
##  the file COPYING included in the packaging of this file.		##
##									##
##  Author:  Jonathan Marten <jjm AT keelhaul DOT me DOT uk>		##
##									##
##########################################################################

SRC=$1							# CMAKE_CURRENT_SOURCE_DIR
BIN=$2							# CMAKE_CURRENT_BINARY_DIR
VER=$3							# base application version

VCSTYPE=						# VCS type detected
VCSREV=							# VCS revision

if [ -d "$SRC/.svn" ]					# is source under SVN?
then
	VCSTYPE="SVN"
	VCSREV=`svn info $SRC 2>/dev/null | sed -n -e'/^Revision:/ { s/^[^:]*: *//;p;q }'`
	echo "Current $VCSTYPE revision: ${VCSREV:-unknown}"
else							# source not under SVN
	if [ -d "$SRC/.git" ]				# try for GIT instead
	then
		VCSTYPE="GIT"
		VCSREV=`cd $SRC && git describe 2>/dev/null`
							# formatted version
		if [ ! -n "$VCSREV" ]			# if not available then
		then					# hash of last commit
			VCSREV=`cd $SRC && git log -1 --abbrev-commit | sed -e 's/commit  *//;q'`
		fi
		echo "Current $VCSTYPE revision: ${VCSREV:-unknown}"
	fi
fi


TMPFILE="$BIN/version.h.tmp"				# temporary header file
{
	echo "#ifndef VERSION"
	echo "#define VERSION              \"${VER}\""
	echo "#endif"
	echo
	echo "#define VCS_REVISION_STRING  \"${VCSREV}\""
	echo "#define VCS_TYPE_STRING      \"${VCSTYPE}\""
	if [ -n "$VCSREV" ]
	then
		echo "#define VCS_HAVE_VERSION     1"
	fi
	echo
} >$TMPFILE


OUTFILE="$BIN/version.h"				# the real header file
if [ ! -f $OUTFILE ]					# does not exist yet
then
	echo "Creating $OUTFILE..."
else							# already exists
	if cmp -s $TMPFILE $OUTFILE			# is it still current?
	then
		rm $TMPFILE				# yes, nothing to do
		exit 0
	else
		echo "Updating $OUTFILE..."
	fi
fi
mv $TMPFILE $OUTFILE					# update the header file
exit 0							# successfully updated
