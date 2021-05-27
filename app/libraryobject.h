/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  Edit:	26-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Library objects, simply enclosing another object.			//
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

#ifndef DRAWLIBRARYOBJECT_H
#define DRAWLIBRARYOBJECT_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qdatetime.h>

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
//  Class DRAWLIBRARYOBJECT -- A library object				//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawLibraryObject : public DrawObject
{
public:
	DrawLibraryObject(Draw::objflags flag,int layer = 0);
	~DrawLibraryObject();

	Draw::object type() const override	{ return (Draw::objLIB); }
	QString typeName() const override	{ return ("LIBRARY"); }

	QByteArray libraryName() const		{ return (objname); }

	bool build(DrawReader &rdr,DrawDiagram *diag) override;
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const override;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const override;

private:
	DrawObject *object;
	QByteArray objname;
	QDateTime objtime;
};

#endif							// DRAWLIBRARYOBJECT_H
