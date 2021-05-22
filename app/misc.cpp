//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Drawfile-related classes and functions that don't fit in anywhere	//
//  else.								//
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

#include "global.h"

#include <qstring.h>
#include <qcolor.h>
#include <qmatrix.h>

#include "reader.h"
#include "coord.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWBOX -- Bounding box in Draw coordinates		 	//
//									//
//////////////////////////////////////////////////////////////////////////

DrawBox::DrawBox(drawint xx0,drawint yy0,drawint xx1,drawint yy1)
{							// struct draw_box
	mx0 = xx0;					//   int x0;
	my0 = yy0;					//   int y0;
	mx1 = xx1;					//   int x1;
	my1 = yy1;					//   int y1;
};


DrawBox::DrawBox()
{
	mx0 = my0 = mx1 = my1 = 0;
}


bool DrawBox::read(DrawReader &rdr)
{
	return (rdr.sizeRemaining()>=(4*sizeof(drawword)) &&
		rdr.getWord(&mx0) &&
		rdr.getWord(&my0) &&
		rdr.getWord(&mx1) &&
		rdr.getWord(&my1));
}


DrawBox::operator QString() const
{
	if (!isValid()) return ("INVALID");
	QString s("[(%1,%2)-(%3,%4)]");
	return (s.arg(mx0).arg(my0).arg(mx1).arg(my1));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWMATRIX -- Transform matrix with the displacements in	//
//  Draw coordinates.						 	//
//									//
//////////////////////////////////////////////////////////////////////////

DrawMatrix::DrawMatrix(drawint m11,drawint m12,drawint m21,drawint m22,drawint dx,drawint dy)
{
	mm[0] = double(m11)/Draw::SCALE1;		// convert to Qt scale factor
	mm[1] = double(m12)/Draw::SCALE1;
	mm[1] = double(m21)/Draw::SCALE1;
	mm[3] = double(m22)/Draw::SCALE1;
	dd[0] = dx; dd[1] = dy;				// these stay in Draw units
	identity = false;
}


DrawMatrix::DrawMatrix(double m11,double m12,double m21,double m22,drawint dx,drawint dy)
{
	mm[0] = m11; mm[1] = m12;
	mm[1] = m21; mm[3] = m22;
	dd[0] = dx;  dd[1] = dy;
	identity = false;
}


DrawMatrix::DrawMatrix()
{
	mm[0] = mm[3] = 1.0;
	mm[1] = mm[2] = 0.0;
	dd[0] = dd[1] = 0;
	identity = true;
}


bool DrawMatrix::read(DrawReader &rdr)
{
	for (int i = 0; i<=3; ++i)			// transformation
	{
		drawint w;
		if (!rdr.getWord(&w)) return (false);
		mm[i] = double(w)/Draw::SCALE1;
	}
	for (int i = 0; i<=1; ++i)			// translations
	{
		if (!rdr.getWord(&dd[i])) return (false);
	}

	identity = false;
	return (true);
}


DrawMatrix::operator QString() const
{
	if (identity) return ("[identity]");
	return (QString("[%1 %2 %3 %4 %5 %6]").arg(mm[0]).arg(mm[1]).arg(mm[2])
	                                      .arg(mm[3]).arg(dd[0]).arg(dd[1]));
}


QMatrix DrawMatrix::toQMatrix(bool nodisplacement) const
{
	return (QMatrix(mm[0],-mm[1],-mm[2],mm[3],
			(nodisplacement ? 0 : DrawCoord::toPixelH(dd[0])),
			(nodisplacement ? 0 : -DrawCoord::toPixelV(dd[1]))));
}


DrawMatrix DrawMatrix::simplifyScale(bool *ix,bool *iy) const
{
	double sx = mm[0];
	double sy = mm[3];

	if ((*ix = (sx<0))) sx = -sx;
	if ((*iy = (sy<0))) sy = -sy;

	return (DrawMatrix(sx,mm[1],mm[2],sy,dd[0],dd[1]));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWPATHSTYLE -- Path style description			//
//									//
//////////////////////////////////////////////////////////////////////////

DrawPathStyle::DrawPathStyle()
{
	joincapwind = overlay = tricapwid = tricaphei = 0;
}


bool DrawPathStyle::read(DrawReader &rdr)
{
	drawuint packed;
	if (!rdr.getWord(&packed)) return (false);	// read 'struct draw_pathstyle'
							// from 'struct draw_pathstyle'
	joincapwind = (packed & 0x000000FF);		// extract 'unsigned char joincapwind'
	overlay = (packed & 0x0000FF00) >> 8;		// extract 'unsigned char overlay'
	tricapwid = (packed & 0x00FF0000) >> 16;	// extract 'unsigned char tricapwid'
	tricaphei = (packed & 0xFF000000) >> 24;	// extract 'unsigned char tricaphei'
	return (true);
}


DrawPathStyle::operator QString() const
{
	const QString s("[jcw %1 over %2 triw %3 trih %4]");
	return (s.arg(joincapwind,0,16).arg(overlay).arg(tricapwid).arg(tricaphei));
}


bool DrawPathStyle::hasDashPattern() const
{
	return ((joincapwind & Draw::dashMASK)==Draw::dashPRESENT);
}


bool DrawPathStyle::fillNonZero() const
{
	return ((joincapwind & Draw::windMASK)==Draw::windNONZERO);
}


Qt::PenCapStyle DrawPathStyle::capStyle(bool *supported,Draw::startcaptyp *startp,Draw::endcaptyp *endp) const
{
	*supported = true;				// be optimistic for now

	const Draw::startcaptyp startcap = static_cast<Draw::startcaptyp>(joincapwind & Draw::startcapMASK);
	const Draw::endcaptyp endcap = static_cast<Draw::endcaptyp>(joincapwind & Draw::endcapMASK);

	if (startcap==Draw::startcapBUTT && endcap==Draw::endcapBUTT) return (Qt::FlatCap);
	if (startcap==Draw::startcapROUND && endcap==Draw::endcapROUND) return (Qt::RoundCap);
	if (startcap==Draw::startcapSQUARE && endcap==Draw::endcapSQUARE) return (Qt::SquareCap);

	*supported = false;				// not directly supported by Qt
	if (startp!=NULL) *startp = startcap;		// give to caller if wanted
	if (endp!=NULL) *endp = endcap;
	return (Qt::FlatCap);				// use this for drawing
}


Qt::PenJoinStyle DrawPathStyle::joinStyle() const
{
	switch (joincapwind & Draw::joinMASK)
	{
default:
case Draw::joinMITRED:		return (Qt::MiterJoin);
case Draw::joinROUND:		return (Qt::RoundJoin);
case Draw::joinBEVELLED:	return (Qt::BevelJoin);
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTSTYLE -- Basic text style description			//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTextStyle::DrawTextStyle()
{
	fontref = flag = 0;
	spacing = 0;
}


bool DrawTextStyle::read(DrawReader &rdr)
{
	drawuint packed;
	if (!rdr.getWord(&packed)) return (false);	// read  'struct draw_textstyle'
							// from 'struct draw_textstyle'
	fontref = (packed & 0x000000FF);		// extract 'draw_fontid fontref'
	flag = (packed & 0x0000FF00) >> 8;		// extract 'unsigned char flags'
	spacing = (packed & 0xFFFF0000) >> 16;		// extract 'short spacing'
	return (true);
}


DrawTextStyle::operator QString() const
{
	const QString s("[ref %1 flags %2 spacing %3]");
	return (s.arg(fontref).arg(flag,0,16).arg(spacing));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTFLAGS -- Additional text style description		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTextFlags::DrawTextFlags()
{
	flag = 0;
}


bool DrawTextFlags::read(DrawReader &rdr)
{
	drawuint packed;
	if (!rdr.getWord(&packed)) return (false);	// read 'int'
							// from 'int'
	flag = (packed & 0x000000FF);			// extract 'unsigned char flags'
	return (true);
}


DrawTextFlags::operator QString() const
{
	return (QString("[%1]").arg(flag,0,16));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Namespace DRAWUTIL -- Utility routines				//
//									//
//////////////////////////////////////////////////////////////////////////

QColor DrawUtil::toQColor(Draw::coltyp c)
{
	const int r = (c & 0x0000FF00) >> 8;
	const int g = (c & 0x00FF0000) >> 16;
	const int b = (c & 0xFF000000) >> 24;
	return (QColor(r,g,b));
}


bool DrawUtil::isTransparent(Draw::coltyp c)
{
	return (c==Draw::TRANSPARENT);
}


// TODO: can we get better equivalents with Unicode?
QByteArray &DrawUtil::substituteRiscosSpecialChars(QByteArray &s)
{
	s.replace('\x81','W');				// uppercase W circumflex
	s.replace('\x82','w');				// lowercase W circumflex
	s.replace('\x85','Y');				// uppercase Y circumflex
	s.replace('\x86','y');				// lowercase Y circumflex
	s.replace('\x8C',"...");			// ellipsis
	s.replace('\x8D',"(TM)");			// trademark
	s.replace('\x8E','%');				// per mille
	s.replace('\x8F','.');				// centered dot
	s.replace('\x90','\'');				// open single quote
	s.replace('\x91','\'');				// close single quote
	s.replace('\x92','<');				// open small angle
	s.replace('\x93','>');				// close small angle
	s.replace('\x94','"');				// open double quote
	s.replace('\x95','"');				// close double quote
	s.replace('\x96','"');				// lowered close double quote
	s.replace('\x97',"--");				// just more slightly longer dash
	s.replace('\x98',"---");			// much longer dash
	s.replace('\x99','-');				// slightly longer dash
	s.replace('\x9A',"OE");				// uppercase diphthong OE
	s.replace('\x9B',"oe");				// lowercase diphthong OE
	s.replace('\x9C','+');				// single dagger
	s.replace('\x9D','|');				// double dagger
	s.replace('\x9E',"fi");				// ligature fi
	s.replace('\x9F',"fl");				// ligature fl
	return (s);
}
