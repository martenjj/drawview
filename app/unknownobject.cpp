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
//  These are not constructed by the standard "creator" function,	//
//  but explictly by the object reader when required.			//
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

#include "unknownobject.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWUNKNOWNOBJECT -- Unknown object as raw data.		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawUnknownObject::DrawUnknownObject(Draw::object typ,Draw::objflag flag,int layer)
	: DrawObject((flag|Draw::flagNOBBOX),layer)
{
	tag = typ;
}


bool DrawUnknownObject::build(DrawReader &rdr,DrawDiagram *diag)
{
	if (!DrawObject::build(rdr,diag)) return (false);

	drawuint size = rdr.sizeRemaining()/sizeof(drawword);
	data.resize(size);
	drawword *ptr = data.data();
	for (unsigned int i = 0; i<size; ++i)
	{
		if (!rdr.getWord(&ptr[i])) return (false);
	}

	return (true);
}


void DrawUnknownObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	const drawword *ptr = data.data();
	bool printed = false;

	str << indent << Qt::hex << qSetPadChar('0');
	for (int i = 0; i<data.size(); ++i)
	{
		if ((i % 8)==0 && printed) str << Qt::endl << indent;
		str << qSetFieldWidth(8) << ptr[i] << qSetFieldWidth(0) << " ";
		printed = true;
	}

	if (printed) str << Qt::endl;
	str << Qt::dec;
}
