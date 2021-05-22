/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Group objects, a simple list of included objects.			//
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
#include <qlist.h>
#include <qalgorithms.h>

#include "reader.h"

#include "groupobject.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGROUPOBJECT - Group object				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawGroupObject::DrawGroupObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
}


DrawGroupObject::~DrawGroupObject()
{
	qDeleteAll(objects);
}


bool DrawGroupObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'draw_groustr'
	if (!DrawObject::build(rdr,diag)) return (false);

	if (!rdr.getWord(NULL) || 			// read 'draw_groupnametyp name'
	    !rdr.getWord(NULL) ||
	    !rdr.getWord(NULL)) return (false);		// nothing ever uses this!

	while (rdr.sizeRemaining()>0)			// read 'draw_objhdr object[]'
	{
		rdr.save();
		DrawObject *obj = DrawObjectManager::create(rdr,diag);
		rdr.restore();
		if (obj==NULL) return (false);
		objects.append(obj);
	}

	return (true);
}


bool DrawGroupObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (!DrawObject::draw(p,diag,opts)) return (false);

	for (QList<DrawObject *>::const_iterator it = objects.begin();
	     it!=objects.end(); ++it)
	{
		(*it)->draw(p,diag,opts);
	}

	return (true);
}


void DrawGroupObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	QString in2 = "|  ";
	for (QList<DrawObject *>::const_iterator it = objects.begin();
	     it!=objects.end(); ++it)
	{
		if ((*it)==objects.last()) in2 = "   ";
		(*it)->dump(str,(indent1+indent2),in2);
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGROUPOBJECTCREATOR - New object creation			//
//  Class DRAWGROUPOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawGroupObjectCreator : public DrawObjectCreator
{
	friend class DrawGroupObjectSetup;
public:
	DrawGroupObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawGroupObject(flag,layer));
	}
};


class DrawGroupObjectSetup
{
private:
	DrawGroupObjectSetup();
	static DrawGroupObjectSetup instance;
};


DrawGroupObjectSetup DrawGroupObjectSetup::instance;


DrawGroupObjectSetup::DrawGroupObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objGROUP,new DrawGroupObjectCreator);
}
