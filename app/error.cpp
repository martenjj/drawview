//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	24-Nov-05						//
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


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qlist.h>
#include <string.h>
#include <qalgorithms.h>

#include "error.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWERROR -- A single error found in a Draw file.		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawError::DrawError(const QString &msg,Draw::error lvl,drawuint obj)
{
	mMessage = msg;					// record error message
	mLevel = lvl;					// and its severity

	mObject = obj;					// record affected object
	if (mObject>0) mMessage += QString(", object %1(0x%2)").arg(mObject,0,10).arg(mObject,6,16,QChar('0'));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWERRORLIST -- Error objects assembled into a list.		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawErrorList::DrawErrorList()
{
	mLevel = Draw::errorOK;				// no error found yet
}


DrawErrorList::~DrawErrorList()
{
	qDeleteAll(mList);				// we created them, so delete
}


void DrawErrorList::add(DrawError *err)
{
	if (err==NULL) return;
	debugmsg(0) << funcinfo << "level " << err->level() << " '" << err->message() << "'";
	mList.append(err);				// add error to list
	if (err->level()>mLevel) mLevel = err->level();	// track maximum severity
}


void DrawErrorList::add(const QString &msg,Draw::error lvl,drawuint obj)
{
	this->add(new DrawError(msg,lvl,obj));		// create and add to list
}
