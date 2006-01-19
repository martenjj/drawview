/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	16-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Path and mask objects						//
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

#ifndef DRAWPATHOBJECT_H
#define DRAWPATHOBJECT_H

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
class QPainter;
class QPainterPath;

class DrawPathDashPattern;
class DrawPathElement;
class DrawDiagram;
class DrawReader;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWPATHOBJECT - Path object					//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawPathObject : public DrawObject
{
public:
	DrawPathObject(Draw::objflags flag,int layer = 0);
	~DrawPathObject();

	Draw::object type() const	{ return (Draw::objPATH); }
	QString typeName() const	{ return ("PATH"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

	bool hasDashPattern() const	{ return (dash!=NULL); }
	bool hasLineColour() const	{ return (pathcolour!=Draw::TRANSPARENT); }

	void ensureFillTransparent()	{ fillcolour = Draw::TRANSPARENT; }
	QPainterPath constructPath() const;

	bool drawPath(QPainterPath &pp,QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;

private:
	Draw::coltyp fillcolour;
	Draw::coltyp pathcolour;
	Draw::pathwidth pathwidth;
	DrawPathStyle pathstyle;
	bool singledot;

	DrawPathDashPattern *dash;
	QList<DrawPathElement *> path;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWMASKOBJECT - True-mask object				//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawMaskObject : public DrawObject
{
public:
	DrawMaskObject(Draw::objflags flag,int layer = 0);
	~DrawMaskObject();

	Draw::object type() const	{ return (Draw::objMASK); }
	QString typeName() const	{ return ("MASK"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

private:
	DrawObject *object;
	DrawPathObject *path;
};


#endif							// DRAWPATHOBJECT_H
