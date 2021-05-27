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


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qtextstream.h>

#include "reader.h"
#include "paintoptions.h"

#include "libraryobject.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWLIBRARYOBJECT -- A library object				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawLibraryObject::DrawLibraryObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	object = NULL;
}


DrawLibraryObject::~DrawLibraryObject()
{
	if (object!=NULL) delete object;
}


bool DrawLibraryObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_libstr'
	if (!DrawObject::build(rdr,diag)) return (false);

        char namebuf[Draw::NAMELEN+1];
        if (!rdr.getByte(namebuf, sizeof(namebuf))) return (false);
        objname = QByteArray(namebuf);			// read 'char name[]'
        rdr.discardWordAlign();				// discard padding

        drawword t;
        if (!rdr.getWord(&t)) return (false);		// read 'unsigned int time'
        objtime = QDateTime::fromSecsSinceEpoch(t);	// 32-bit time_t from RiscOS

	rdr.save();
	object = DrawObjectManager::create(rdr,diag);	// read subobject
	rdr.restore();
	if (object==NULL) return (false);

	return (true);
}


bool DrawLibraryObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (object==NULL) return (false);

	const QByteArray &wantedName = opts->libraryName();
	if (!wantedName.isEmpty() && wantedName!=objname) return (false);

	if (!DrawObject::draw(p,diag,opts)) return (false);
	return (object->draw(p,diag,opts));
}


void DrawLibraryObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	str << indent << "name \"" << objname << "\" time " << objtime.toString() << Qt::endl;

	object->dump(str, indent, "   ");
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWLIBRARYOBJECTCREATOR - New object creation		//
//  Class DRAWLIBRARYOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawLibraryObjectCreator : public DrawObjectCreator
{
	friend class DrawLibraryObjectSetup;
public:
	DrawLibraryObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawLibraryObject(flag,layer));
	}
};


class DrawLibraryObjectSetup
{
private:
	DrawLibraryObjectSetup();
	static DrawLibraryObjectSetup instance;
};


DrawLibraryObjectSetup DrawLibraryObjectSetup::instance;


DrawLibraryObjectSetup::DrawLibraryObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objLIB, new DrawLibraryObjectCreator);
}
