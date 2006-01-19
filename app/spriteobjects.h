/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	16-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Simple and transformed sprite objects, and JPEG image objects.	//
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

#ifndef DRAWSPRITEOBJECTS_H
#define DRAWSPRITEOBJECTS_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qimage.h>

#include "object.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QTextStream;
class QPainter;
class PaintOptions;

class DrawDiagram;
class DrawReader;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWIMAGEOBJECTBASE -- Abstract base class for all three	//
//  sorts of image object.						//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawImageObjectBase : public DrawObject
{
public:
	DrawImageObjectBase(Draw::objflags flag,int layer = 0);
	virtual ~DrawImageObjectBase()		{}

	bool build3();

	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts,
		  const DrawMatrix *mat = NULL) const;

	void dump1(QTextStream &str,const QString &indent1,const QString &indent2) const;
	void dump2(QTextStream &str,const QString &indent1,const QString &indent2) const;

protected:
	QImage image;
	bool simple;
	DrawMatrix transmat;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSPRITEOBJECTBASE -- Abstract base class for the two	//
//  sorts of sprite image object.					//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawSpriteObjectBase : public DrawImageObjectBase
{
public:
	DrawSpriteObjectBase(Draw::objflags flag,int layer = 0);
	virtual ~DrawSpriteObjectBase()		{}

	bool build2(DrawReader &rdr,DrawDiagram *diag);

	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts,
		  const DrawMatrix *mat = NULL) const;

	void dump1(QTextStream &str,const QString &indent1,const QString &indent2) const;
	void dump2(QTextStream &str,const QString &indent1,const QString &indent2) const;

private:
	drawuint mode;
	int bpp;
	int cols;
	int xsize,ysize;
	bool haspalette;
	bool hadpalette;
	bool hasmask;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSPRITEOBJECT -- Simple (untransformed) sprite object.	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawSpriteObject : public DrawSpriteObjectBase
{
public:
	DrawSpriteObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objSPRITE); }
	QString typeName() const	{ return ("SPRITE"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTRANSFORMEDSPRITEOBJECT -- Transformed sprite object.	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTransformedSpriteObject : public DrawSpriteObjectBase
{
public:
	DrawTransformedSpriteObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objTRANSPRITE); }
	QString typeName() const	{ return ("TRANSPRITE"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWJPEGOBJECT -- JPEG image object.				//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawJpegObject : public DrawImageObjectBase
{

public:
	DrawJpegObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objJPEG); }
	QString typeName() const	{ return ("JPEG"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

private:
	drawuint width,height;
	drawuint pdx,pdy;
};

#endif							// DRAWSPRITEOBJECTS_H
