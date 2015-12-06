############################################## -*- mode:makefile; -*- ####
##									##
##  Project:	DrawView						##
##  Edit:	06-Dec-15						##
##									##
##########################################################################
##									##
##  Automake template makefile for the DrawView application.		##
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

bin_PROGRAMS =		drawview

drawview_SOURCES =	main.cpp drawview.cpp paper.cpp				\
			drawwidget.cpp diagram.cpp object.cpp coord.cpp		\
			reader.cpp error.cpp misc.cpp fonts.cpp files.cpp	\
			paintoptions.cpp spritereader.cpp palette.cpp		\
			unknownobject.cpp pathobjects.cpp groupobject.cpp	\
			taggedobject.cpp textobjects.cpp spriteobjects.cpp	\
			textarea.cpp replicates.cpp radiate.cpp

drawview_LDFLAGS =	$(qt4ldflags)
drawview_LDADD =	$(qt4libs)

noinst_HEADERS =	global.h drawtypes.h drawview.h paper.h			\
			drawwidget.h diagram.h object.h coord.h			\
			reader.h error.h fonts.h files.h			\
			paintoptions.h spritereader.h palette.h			\
			unknownobject.h pathobjects.h groupobject.h		\
			taggedobject.h textobjects.h spriteobjects.h		\
			textarea.h replicates.h radiate.h

drawview.o:		drawview.moc
drawwidget.o:		drawwidget.moc

%.moc:			%.h
			$(MOC) -o $@ $<

clean-local:;		$(RM) *.moc *.moc.cpp *~
