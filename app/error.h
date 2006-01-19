/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	25-Nov-05						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Draw file error tracking.						//
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

#ifndef DRAWERROR_H
#define DRAWERROR_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qlist.h>

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWERROR -- A single error found in a Draw file.		//
//									//
//  The error object records the error message, error level (severity)	//
//  and (optionally) the affected or causing object.			//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawError
{
public:
	DrawError(const QString &msg,Draw::error lvl = Draw::errorFATAL,drawuint obj = 0);

	Draw::error level() const	{ return (mLevel); }
	QString message() const		{ return (mMessage); }

private:
	QString mMessage;				// error message
	Draw::error mLevel;				// severity level
	drawuint mObject;				// object affected
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWERRORLIST -- Error objects assembled into a list.		//
//									//
//  Most error objects in this application are created by our		//
//  convenience function 'add(msg,lvl,obj)', itself called indirectly	//
//  from the reader which tracks the object address.			//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawErrorList
{
public:
	DrawErrorList();
	~DrawErrorList();

	void add(DrawError *err);
	void add(const QString &msg,Draw::error lvl = Draw::errorFATAL,drawuint obj = 0);

	Draw::error level() const			{ return (mLevel); }
	const QList<const DrawError *> *list() const	{ return (&mList); }
							// 3 'const's in one line,
private:						// was that a record?
	Draw::error mLevel;				// maximum severity level
	QList<const DrawError *> mList;			// list of error objects
};


#endif							// DRAWERROR_H
