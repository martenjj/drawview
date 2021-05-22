/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView						//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	07-Mar-17						//
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

#ifndef APPLIC_H
#define APPLIC_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif							// CONFIG_H
#include "drawtypes.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Debugging messages							//
//									//
//////////////////////////////////////////////////////////////////////////

#ifdef KDE
#include <kdebug.h>
#define debugmsg(n)	kdDebug(0)
#define warnmsg()	kdWarning()
#else							// KDE
#ifdef QT4
#include <qdebug.h>
#define debugmsg(n)	qDebug().nospace()
#define warnmsg()	qWarning().nospace() << Qt::endl
#else							// QT4
#include <iostream>
#define debugmsg(n)	cerr
#define warnmsg()	cerr << "\n" << "WARNING: "
#endif							// QT4
#endif							// KDE

#ifdef KDE
#define funcinfo	k_funcinfo
#else							// KDE
#ifdef __GNUC__
#define funcinfo	"[" << __PRETTY_FUNCTION__ << "] "
#else							// GNUC
#define funcinfo	"[" << __FILE__ << ":" << __LINE__ << "] "
#endif							// GNUC
#endif							// KDE


#endif							// APPLIC_H
