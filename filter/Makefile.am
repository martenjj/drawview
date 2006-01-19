############################################## -*- mode:makefile; -*- ####
##									##
##  Project:	DrawView - Decompression filter				##
##  SCCS:	<%Z% %M% %I%>					##
##  Edit:	17-Jan-06						##
##									##
##########################################################################
##									##
##  Automake template makefile for the Draw file decompression filter.	##
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

AM_CPPFLAGS =			-DDRAWFILTER -I@utilsrc@ -I@riscosrc@ -I@riscosrc@/..

libexec_PROGRAMS =		drawfilter

dist_drawfilter_SOURCES =	main.c riscoscompat.c lzw.c
nodist_drawfilter_SOURCES =	@utilsrc@/cmdutils.c @riscosrc@/draw/decompress.c

drawfilter_LDADD =		-ldl
