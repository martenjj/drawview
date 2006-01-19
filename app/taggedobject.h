/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	16-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Tagged objects, simply enclosing another object.			//
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

#ifndef DRAWTAGGEDOBJECT_H
#define DRAWTAGGEDOBJECT_H

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

class DrawDiagram;
class DrawReader;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTAGGEDOBJECT -- A tagged object				//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTaggedObject : public DrawObject
{
public:
	DrawTaggedObject(Draw::objflags flag,int layer = 0);
	~DrawTaggedObject();

	Draw::object type() const	{ return (Draw::objTAGGED); }
	QString typeName() const	{ return ("TAGGED"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

private:
	drawword id;
	DrawObject *object;
	QVector<drawword> data;
};

#endif							// DRAWTAGGEDOBJECT_H
