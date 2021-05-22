/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	22-May-21						//
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


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qtextstream.h>
#include <qvector.h>

#include "reader.h"

#include "taggedobject.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTAGGEDOBJECT -- A tagged object				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTaggedObject::DrawTaggedObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	object = NULL;
}


DrawTaggedObject::~DrawTaggedObject()
{
	if (object!=NULL) delete object;
}


bool DrawTaggedObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_taggedstr'
	if (!DrawObject::build(rdr,diag)) return (false);

	if (!rdr.getWord(&id)) return (false);		// read 'int id'

	rdr.save();
	object = DrawObjectManager::create(rdr,diag);	// read subobject
	rdr.restore();
	if (object==NULL) return (false);

	drawword size = rdr.sizeRemaining()/sizeof(drawword);
	data.resize(size);				// optional tagged data
	drawword *ptr = data.data();
	for (unsigned int i = 0; i<size; ++i)
	{
		if (!rdr.getWord(&ptr[i])) return (false);
	}

	return (true);
}


bool DrawTaggedObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (object==NULL) return (false);
	if (!DrawObject::draw(p,diag,opts)) return (false);
	return (object->draw(p,diag,opts));
}


void DrawTaggedObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	str << indent << "id " << Qt::hex << qSetPadChar('0')
	    << qSetFieldWidth(8) << id << qSetFieldWidth(0) << Qt::dec << Qt::endl;

	object->dump(str,indent,(data.empty() ? "   " : "|  "));

	if (!data.empty())
	{
		str << indent << "|" << Qt::endl;
		str << indent << "+- Tagged data size " << data.size() << Qt::endl;

		const drawword *ptr = data.data();
		bool printed = false;

		str << indent << "   " << Qt::hex << qSetPadChar('0');
		for (int i = 0; i<data.size(); ++i)
		{
			if ((i % 8)==0 && printed) str << Qt::endl << indent << "   ";
			str << qSetFieldWidth(8) << ptr[i] << qSetFieldWidth(0) << " ";
			printed = true;
		}

		if (printed) str << Qt::endl;
		str << Qt::dec;
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTAGGEDOBJECTCREATOR - New object creation			//
//  Class DRAWTAGGEDOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTaggedObjectCreator : public DrawObjectCreator
{
	friend class DrawTaggedObjectSetup;
public:
	DrawTaggedObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawTaggedObject(flag,layer));
	}
};


class DrawTaggedObjectSetup
{
private:
	DrawTaggedObjectSetup();
	static DrawTaggedObjectSetup instance;
};


DrawTaggedObjectSetup DrawTaggedObjectSetup::instance;


DrawTaggedObjectSetup::DrawTaggedObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objTAGGED,new DrawTaggedObjectCreator);
}
