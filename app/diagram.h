/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	26-Nov-05						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Draw diagram:  a collection of objects, with associated font	//
//  mapping and possible loading/verify errors. 			//
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

#ifndef DIAGRAM_H
#define DIAGRAM_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qlist.h>

#include "error.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawObject;
class QTextStream;
class QPainter;
class QPrinter;
class QByteArray;

class DrawReader;
class PaintOptions;
class FontReferenceMap;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWDIAGRAM -- Draw diagram handling, as a linear collection	//
//  of objects.								//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawDiagram
{
public:
	DrawDiagram(const QString &file = QString::null);
	~DrawDiagram();

	bool isValid() const;
	QString drawError() const;

	void dump(QTextStream &str) const;
	void draw(QPainter &p,const PaintOptions *opts) const;
	void print(QPrinter &p);

	FontReferenceMap *fontMap() const		{ return (fontmap); }
	void setFontMap(FontReferenceMap *fm) 		{ fontmap = fm; }
	const DrawBox *boundingBox() const		{ return (&bbox); }

protected:
	bool readDrawFile(DrawReader &rdr,QByteArray title = "");

private:
	DrawErrorList errors;
	FontReferenceMap *fontmap;
	QList<DrawObject *> objects;

	DrawBox bbox;
	DrawBox *clipbox;
};

#endif							// DIAGRAM_H
