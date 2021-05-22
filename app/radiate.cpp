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


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#ifdef HAVE_MATH_H
#include <math.h>
#endif

#include <qpainter.h>
#include <qmatrix.h>
#include <qtextstream.h>

#include "reader.h"
#include "coord.h"
#include "paintoptions.h"

#include "radiate.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWRADIATEOBJECT -- Radiate object				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawRadiateObject::DrawRadiateObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	object = NULL;
}


DrawRadiateObject::~DrawRadiateObject()
{
	if (object!=NULL) delete object;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Building the radiate object						//
//									//
//  Ok, so it was a really silly idea storing the angle as a binary	//
//  'double' in the Draw file!  Even though it is theoretically a	//
//  standard IEEE Double Precision value, assuming that the platform's	//
//  floating point format is the same and trying to simply convert	//
//  via a cast is far too dodgy.  Trying:				//
//									//
//    angle = *(reinterpret_cast<double *>(&ang[0]))			//
//									//
//  failed miserably.							//
//									//
//  See RiscOS PRM 4-166 figure 78.2 for the Acorn representation.	//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawRadiateObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_radiatestr'
	if (!DrawObject::build(rdr,diag)) return (false);

	drawuint ang[2];
	if (!rdr.getWord((drawword *) &ang[0]) ||	// read 'double angle'
	    !rdr.getWord((drawword *) &ang[1])) return (false);

	const bool sign = (ang[0] & 0x80000000)!=0;	// convert to native 'double'
	const int exp = (ang[0] & 0x7FF00000)>>20;
	const double frac = 1+ldexp((ang[0] & 0x000FFFFF),-20)+ldexp(ang[1],-52);
	angle = ldexp(frac,exp-1023);
	if (sign) angle = -angle;

	if (!rdr.getWord((drawword *) &number) ||	// read 'int number'
	    !rdr.getWord((drawword *) &centre)) return (false);
							// read 'draw_radtyp centre'
	rdr.save();
	object = DrawObjectManager::create(rdr,diag);	// read 'draw_pathstrhdr path'
	rdr.restore();
	if (object==NULL) return (false);

	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Find the centre point about which the contained object is rotated.	//
//									//
//  See dru_radiateCentre() in lib/draw/drawutil1.c			//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawRadiateObject::findCentre(drawint *px,drawint *py) const
{
	switch (centre)					// horizontal...
	{
case Draw::radTOPLEFT:
case Draw::radCENTLEFT:
case Draw::radBOTTLEFT:
		*px = object->bbox()->x0();		// left
		break;

case Draw::radTOPRIGHT:
case Draw::radCENTRIGHT:
case Draw::radBOTTRIGHT:
		*px = object->bbox()->x1();		// right
		break;

case Draw::radTOPCENT:
case Draw::radCENTRE:
case Draw::radBOTTCENT:					// centre
		*px = (object->bbox()->x0()+object->bbox()->x1())/2;
		break;
	}

	switch (centre)					// vertical...
	{
case Draw::radBOTTLEFT:
case Draw::radBOTTCENT:
case Draw::radBOTTRIGHT:
		*py = object->bbox()->y0();		// bottom
		break;

case Draw::radTOPLEFT:
case Draw::radTOPCENT:
case Draw::radTOPRIGHT:
		*py = object->bbox()->y1();		// top
		break;

case Draw::radCENTLEFT:
case Draw::radCENTRE:
case Draw::radCENTRIGHT:				// centre
		*py = (object->bbox()->y0()+object->bbox()->y1())/2;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Drawing and text dump						//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawRadiateObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (!DrawObject::draw(p,diag,opts)) return (false);

	p.save();					// save global state
	QMatrix m = p.matrix();				// cheaper than restore()/save()
	PaintOptions newopts = *opts;			// doesn't work when matrix changed
	newopts.setFlags(PaintOptions::EnableClipping,false);

	int cx,cy;					// drawing coordinates of centre
	findCentre(&cx,&cy);
	const int scx = DrawCoord::toPixelX(cx);	// pixel position of centre
	const int scy = DrawCoord::toPixelY(cy);

	for (int i = 0; i<number; ++i)
	{
		p.setMatrix(m);				// reset to original each time

		double ang = -angle*i;			// rotation in Qt is clockwise
		double c = cos(ang);
		double s = sin(ang);

		int dx = scx-qRound(c*scx-s*scy);	// offset from original centre
		int dy = scy-qRound(s*scx+c*scy);	// to transformed centre

		p.setMatrix(QMatrix(c,s,-s,c,dx,dy),true);
		object->draw(p,diag,&newopts);
	}

	p.restore();					// restore global state
	return (true);
}


void DrawRadiateObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	str << indent << "centre " << centre
	    << " angle " << angle << " degrees " << (angle*180/acos(-1))
	    << " number " << number << Qt::endl;

	object->dump(str,indent,"   ");
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWRADIATEOBJECTCREATOR - New object creation		//
//  Class DRAWRADIATEOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawRadiateObjectCreator : public DrawObjectCreator
{
	friend class DrawRadiateObjectSetup;
public:
	DrawRadiateObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawRadiateObject(flag,layer));
	}
};


class DrawRadiateObjectSetup
{
private:
	DrawRadiateObjectSetup();
	static DrawRadiateObjectSetup instance;
};


DrawRadiateObjectSetup DrawRadiateObjectSetup::instance;


DrawRadiateObjectSetup::DrawRadiateObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objRADIATE,new DrawRadiateObjectCreator);
}
