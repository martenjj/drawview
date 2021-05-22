/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Simple and transformed text objects, and the font table.		//
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

#ifndef DRAWTEXTOBJECTS_H
#define DRAWTEXTOBJECTS_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qbytearray.h>

#include "object.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QTextStream;
class QPainter;

class DrawDiagram;
class DrawReader;

class FontReferenceMap;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTOBJECTBASE -- Abstract base class for the two sorts	//
//  of text object.							//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextObjectBase : public DrawObject
{
public:
	DrawTextObjectBase(Draw::objflags flag,int layer = 0);
	virtual ~DrawTextObjectBase()				{}

	bool build2(DrawReader &rdr,DrawDiagram *diag);

	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts,
		  Draw::textopt flags,const DrawMatrix *mat = NULL) const;

	void dump1(QTextStream &str,const QString &indent1,const QString &indent2) const;
	void dump2(QTextStream &str,const QString &indent1,const QString &indent2) const;

protected:
	DrawTextStyle textstyle;

private:
	Draw::coltyp textcolour;
	Draw::coltyp backcolour;
	drawuint xsize,ysize;
	drawint xcoord,ycoord;
	QByteArray text;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTOBJECT -- Simple (untransformed) text object.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextObject : public DrawTextObjectBase
{
public:
	DrawTextObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const override	{ return (Draw::objTEXT); }
	QString typeName() const override	{ return ("TEXT"); }

	bool build(DrawReader &rdr,DrawDiagram *diag) override;
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const override;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const override;

private:
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTRANSFORMEDTEXTOBJECT -- Transformed text object.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTransformedTextObject : public DrawTextObjectBase
{
public:
	DrawTransformedTextObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const override	{ return (Draw::objTRANSTEXT); }
	QString typeName() const override	{ return ("TRANSTEXT"); }

	bool build(DrawReader &rdr,DrawDiagram *diag) override;
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const override;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const override;

private:
	DrawMatrix mat;
	DrawTextFlags textflags;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWFONTTABLEOBJECT -- Font table object for mapping fonts	//
//  used in text objects.						//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawFontTableObject : public DrawObject
{
public:
	DrawFontTableObject(Draw::objflags flag,int layer = 0);
	~DrawFontTableObject();

	Draw::object type() const override	{ return (Draw::objFONTLIST); }
	QString typeName() const override	{ return ("FONTTABLE"); }

	bool build(DrawReader &rdr,DrawDiagram *diag) override;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const override;

private:
	FontReferenceMap *fontmap;
};

#endif							// DRAWTEXTOBJECTS_H
