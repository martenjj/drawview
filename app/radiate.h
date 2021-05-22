/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Radiate objects.							//
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

#ifndef RADIATE_H
#define RADIATE_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWRADIATEOBJECT -- Radiate object				//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawRadiateObject : public DrawObject
{
public:
	DrawRadiateObject(Draw::objflags flag,int layer = 0);
	~DrawRadiateObject();

	Draw::object type() const override	{ return (Draw::objRADIATE); }
	QString typeName() const override	{ return ("RADIATE"); }

	bool build(DrawReader &rdr,DrawDiagram *diag) override;
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const override;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const override;

protected:
	void findCentre(drawint *px,drawint *py) const;

private:
	double angle;
	int number;
	Draw::radtyp centre;
	DrawObject *object;
};

#endif							// RADIATE_H
