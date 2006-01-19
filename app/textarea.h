/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	16-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Text area and column objects.					//
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

#ifndef DRAWTEXTAREA_H
#define DRAWTEXTAREA_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qlist.h>

#include "object.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QTextStream;
class QTextCursor;

class DrawDiagram;
class DrawReader;
class DrawTextColumnObject;

class TextAreaDocument;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTAREAOBJECT -- Text area and its formatted document.	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextAreaObject : public DrawObject
{
public:
	DrawTextAreaObject(Draw::objflags flag,int layer = 0);
	~DrawTextAreaObject();

	Draw::object type() const	{ return (Draw::objTEXTAREA); }
	QString typeName() const	{ return ("TEXTAREA"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

private:
	void getCharProperties(const QTextCursor *cur,int *lp,int *pp,int *lm,int *rm) const;

	Draw::coltyp textcolour;
	Draw::coltyp backcolour;
	QByteArray text;

	QList<DrawTextColumnObject *> columns;
	TextAreaDocument *document;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTCOLUMNOBJECT -- Text column contained within a text	//
//  area.								//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextColumnObject : public DrawObject
{
public:
	DrawTextColumnObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objTEXTCOL); }
	QString typeName() const	{ return ("TEXTCOLUMN"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

private:
};

#endif							// DRAWTEXTAREA_H
