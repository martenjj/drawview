/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView						//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	13-Feb-22						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Global definitions for the application and library.			//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Copyright (c) 2006 Jonathan Marten <jjm@keelhaul.demon.co.uk>	//
//  Home & download:  http://www.keelhaul.demon.co.uk/acorn/drawview/	//
//									//
//  This program is free software; you can redistribute it and/or	//
//  modify it under the terms of the GNU General Public License as	//
//  published by the Free Software Foundation; either version 2 of	//
//  the License, or (at your option) any later version.			//
//									//
//  It is distributed in the hope that it will be useful, but		//
//  WITHOUT ANY WARRANTY; without even the implied warranty of		//
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	//
//  GNU General Public License for more details.			//
//									//
//  You should have received a copy of the GNU General Public		//
//  License along with this program; see the file COPYING for further	//
//  details.  If not, write to the Free Software Foundation, Inc.,	//
//  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.		//
//									//
//////////////////////////////////////////////////////////////////////////

#ifndef GLOBAL_H
#define GLOBAL_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif							// CONFIG_H
#include "drawtypes.h"

#ifdef KDE4
#include <kdebug.h>
#endif							// KDE4
#ifdef KF5
#include "drawview_debug.h"
#endif							// KF5

//////////////////////////////////////////////////////////////////////////
//									//
//  Debugging messages							//
//									//
//////////////////////////////////////////////////////////////////////////

#ifdef KDE4
#define debugmsg(n)	kdDebug(0)
#define warnmsg()	kdWarning()
#else							// KDE4
#ifdef KF5
#define debugmsg(n)	qCDebug(DRAWVIEW_LOG).nospace()
#define warnmsg()	qCWarning(DRAWVIEW_LOG).nospace()
#else							// KF5
#ifdef QT4
#include <qdebug.h>
#define debugmsg(n)	qDebug().nospace()
#define warnmsg()	qWarning().nospace() << Qt::endl
#else							// QT4
#include <iostream>
#define debugmsg(n)	std::cerr
#define warnmsg()	std::cerr << "\n" << "WARNING: "
#endif							// QT4
#endif							// KF5
#endif							// KDE4

#ifdef KDE4
#define funcinfo	k_funcinfo
#else							// KDE4
#ifdef KF5
#define funcinfo	""
#else							// KF5
#ifdef __GNUC__
#define funcinfo	"[" << __PRETTY_FUNCTION__ << "] "
#else							// GNUC
#define funcinfo	"[" << __FILE__ << ":" << __LINE__ << "] "
#endif							// GNUC
#endif							// KF5
#endif							// KDE4

//////////////////////////////////////////////////////////////////////////
//									//
//  Global functions and variables					//
//									//
//////////////////////////////////////////////////////////////////////////

extern void reportError(const QString &msg, bool fatal = true);
extern bool guiMode;


#endif							// GLOBAL_H
