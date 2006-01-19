/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	16-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Unknown object types, just stored as raw data.			//
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

#ifndef DRAWUNKNOWNOBJECT_H
#define DRAWUNKNOWNOBJECT_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qvector.h>

#include "object.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QTextStream;
class QPainter;

class DrawReader;
class DrawDiagram;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWUNKNOWNOBJECT -- Unknown object as raw data.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawUnknownObject : public DrawObject
{
public:
	DrawUnknownObject(Draw::object type,Draw::objflag flag,int layer = 0);

	Draw::object type() const 	{ return (tag); }
	QString typeName() const	{ return (QString("UNKNOWN(%1)").arg(tag)); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

private:
	Draw::object tag;
	QVector<drawword> data;
};

#endif							// DRAWUNKNOWNOBJECT_H
