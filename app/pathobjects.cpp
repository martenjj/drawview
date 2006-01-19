/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	16-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Path and mask objects, and the paths contained within them.		//
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

#undef DEBUG_CAPS

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qtextstream.h>
#include <qvector.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpen.h>
#include <qbrush.h>
#include <qalgorithms.h>

#include "reader.h"
#include "diagram.h"
#include "coord.h"
#include "error.h"
#include "paintoptions.h"

#include "pathobjects.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWPATHDASHPATTERN - Line dash patterns.			//
//									//
//  Currently the implementation uses the built-in Qt line patterns	//
//  only, guessed from the object dash pattern data based on the dash	//
//  patterns built in to the Acorn applications (DrawPlus and Vector	//
//  had user-defined dash patterns, but always based on the same basic	//
//  interval, DASHBIT Draw units).					//
//									//
//  TODO:  Qt 4.1 has user-defined dash patterns, could use these in	//
//  cases where the defined pattern doesn't match any of the built-in	//
//  ones.								//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawPathDashPattern
{
public:
	DrawPathDashPattern()		{}

	bool isValid()			{ return (!elements.isEmpty()); }
	Qt::PenStyle getPen();

	bool read(DrawReader &rdr);
	void dump(QTextStream &str,const QString &indent = QString::null);

private:
	drawuint start;
	QVector<drawuint> elements;
};


bool DrawPathDashPattern::read(DrawReader &rdr)
{							// from 'struct draw_dashstr'
	int count;

	if (!rdr.getWord(&start) ||			// read 'int dashstart,dashcount'
	    !rdr.getWord((drawword *) &count)) return (false);

	elements.resize(count);
	drawuint *ptr = elements.data();
	for (int i = 0; i<count; ++i)
	{						// read 'int dashelements[]'
		if (!rdr.getWord(&ptr[i])) return (false);
	}

	bool regular = true;				// see if can be simplified
	const drawuint first = ptr[0];			// note first element
	for (int i = 1; i<count; ++i)			// compare against all others
	{
		if (ptr[i]!=first)			// if mismatch, stop here
		{
			regular = false;
			break;
		}
	}
	if (regular) elements.resize(1);		// regular, so can simplify

	return (true);
}


void DrawPathDashPattern::dump(QTextStream &str,const QString &indent)
{
	str << indent << "Dash pattern start " << start << " count " << elements.size() << " elements ";
	const drawuint *ptr = elements.data();
	for (int i = 0; i<elements.size(); ++i) str << ptr[i] << " ";
	str << endl;
}


// Standard dash patterns for Vector and DrawPlus (from 'prefuser.c'):
//
//	{ start, count, { elements } }		looks like		draw as
//
//	{ 0, 2, { 2304, 2304 } },		- - - - - - 		DotLine
//	{ 0, 2, { 4608, 4608 } },		-- -- -- --		DashLine
//	{ 0, 2, { 9216, 9216 } },		--- --- ---		DashLine
//	{ 0, 4, { 9216, 2304, 2304, 2304 } },	--- - --- -		DashDotLine
//	{ 0, 2, { 2304, 4608 } },		-  -  -  -		DotLine
//	{ 0, 2, { 2304, 9216 } },		-   -   -		DotLine
//	{ 0, 2, { 4608, 9216 } },		--   --   -		DashLine
//	{ 0, 4, { 2304, 2304, 4608, 9216 } },	- --   - --		DashDotLine
//	{ 0, 4, { 2304, 4608, 9216, 4608 } },	-  ---  -		DashDotLine
//	{ 0, 4, { 2304, 2304, 4608, 2304 } },   - -- - -- -		DashDotLine

Qt::PenStyle DrawPathDashPattern::getPen()
{
	switch (elements.size())
	{
case 0:		return (Qt::SolidLine);			// plain line

case 1:
case 2:		if (elements[0]<2*Draw::DASHBIT) return (Qt::DotLine);
		else return (Qt::DashLine);		// dotted line, guess spacing

case 4:		return (Qt::DashDotLine);		// dash-dot line

default:	return (Qt::DashDotDotLine);		// anything more complex
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWPATHELEMENT - One element of a Draw path, becoming the	//
//  corresponding element of a QPainterPath.  Each visible element	//
//  can draw itself, and its custom caps if required.			//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawPathElement
{
public:
	DrawPathElement(drawint xx = -1,drawint yy = -1)	{ x = xx; y = yy; }
	virtual operator QString() = NULL;
	virtual void draw(QPainterPath &pp) = NULL;
	virtual bool isStroke()		{ return (false); }
	virtual bool hasCoords()	{ return (true); }
	virtual void drawStartCap(QPainter &p,drawint px,drawint py,Draw::startcaptyp type,int w,int l = 0)	{}
	virtual void drawEndCap(QPainter &p,drawint px,drawint py,Draw::endcaptyp type,int w,int l = 0)		{}
	drawint X() const		{ return (x); }
	drawint Y() const		{ return (y); }

protected:
	drawint x,y;
};


class DrawPathTermElement : public DrawPathElement
{
public:
	DrawPathTermElement() : DrawPathElement()		{}
	operator QString() 		{ return ("TERM"); }
	void draw(QPainterPath &pp) 				{}
	bool hasCoords()		{ return (false); }
};


class DrawPathMoveElement : public DrawPathElement
{
public:
	DrawPathMoveElement(drawint xx,drawint yy) : DrawPathElement(xx,yy)	{}
	operator QString() 		{ return (QString("MOVE->(%1,%2)").arg(x).arg(y)); }
	void draw(QPainterPath &pp)	{ pp.moveTo(DrawCoord::toPixelX(x),DrawCoord::toPixelY(y)); }
};


class DrawPathLineElement : public DrawPathElement
{
public:
	DrawPathLineElement(drawint xx,drawint yy) : DrawPathElement(xx,yy)	{}
	operator QString() 		{ return (QString("LINE->(%1,%2)").arg(x).arg(y)); }
	void draw(QPainterPath &pp)	{ pp.lineTo(DrawCoord::toPixelX(x),DrawCoord::toPixelY(y)); }
	bool isStroke()						{ return (true); }
	void drawStartCap(QPainter &p,drawint px,drawint py,Draw::startcaptyp type,int w,int l = 0);
	void drawEndCap(QPainter &p,drawint px,drawint py,Draw::endcaptyp type,int w,int l = 0);
};


class DrawPathCurveElement : public DrawPathElement
{
public:
	DrawPathCurveElement(drawint xx,drawint yy,
			     drawint x1,drawint y1,
			     drawint x2,drawint y2) : DrawPathElement(xx,yy)	{
					cx1 = x1; cy1 = y1; cx2 = x2; cy2 = y2; }
	operator QString() 		{ return (QString("CURVE->(%1,%2)@(%3,%4)@(%5,%6)")
						  .arg(x).arg(y)
						  .arg(cx1).arg(cy1).arg(cx2).arg(cy2)); }
	void draw(QPainterPath &pp)	{ pp.cubicTo(DrawCoord::toPixelX(cx1),DrawCoord::toPixelY(cy1),
						             DrawCoord::toPixelX(cx2),DrawCoord::toPixelY(cy2),
						             DrawCoord::toPixelX(x),DrawCoord::toPixelY(y)); }
	bool isStroke()						{ return (true); }
	void drawStartCap(QPainter &p,drawint px,drawint py,Draw::startcaptyp type,int w,int l = 0);
	void drawEndCap(QPainter &p,drawint px,drawint py,Draw::endcaptyp type,int w,int l = 0);

protected:
	drawint cx1,cy1;
	drawint cx2,cy2;
};


class DrawPathCloseElement : public DrawPathElement
{
public:
	DrawPathCloseElement() : DrawPathElement()				{}
	operator QString()		{ return ("CLOSE"); }
	void draw(QPainterPath &pp)	{ pp.closeSubpath(); }
	bool isStroke()			{ return (true); }
	bool hasCoords()		{ return (false); }
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Drawing of custom caps						//
//									//
//////////////////////////////////////////////////////////////////////////

static void drawRoundCap(QPainter &p,drawint sx,drawint sy,int wid)
{
#ifdef DEBUG_CAPS
	debugmsg(0) << funcinfo;
	p.setPen(Qt::cyan);
	p.setBrush(Qt::NoBrush);
#endif							// DEBUG_CAPS

	const int hw = wid/2;
	p.drawEllipse(DrawCoord::toPixelX(sx-hw),DrawCoord::toPixelY(sy+hw),
		      DrawCoord::toPixelH(wid),DrawCoord::toPixelV(wid));
}


static void drawSquareCap(QPainter &p,drawint sx,drawint sy,double angle,int wid)
{
#ifdef DEBUG_CAPS
	debugmsg(0) << funcinfo;
	p.setPen(Qt::red);
	p.setBrush(Qt::NoBrush);
#endif							// DEBUG_CAPS

	const double ws = (wid*sin(angle))/2;
	const double wc = (wid*cos(angle))/2;

	const int t1x = DrawCoord::toPixelX(sx-static_cast<int>(ws+0.5));
	const int t1y = DrawCoord::toPixelY(sy+static_cast<int>(wc+0.5));

	const int t2x = DrawCoord::toPixelX(sx-static_cast<int>((wc+ws)+0.5));
	const int t2y = DrawCoord::toPixelY(sy-static_cast<int>((ws-wc)+0.5));

	const int t3x = DrawCoord::toPixelX(sx-static_cast<int>((wc-ws)+0.5));
	const int t3y = DrawCoord::toPixelY(sy-static_cast<int>((ws+wc)+0.5));

	const int t4x = DrawCoord::toPixelX(sx+static_cast<int>(ws+0.5));
	const int t4y = DrawCoord::toPixelY(sy-static_cast<int>(wc+0.5));

	QPoint pts[4];
	pts[0] = QPoint(t1x,t1y);
	pts[1] = QPoint(t2x,t2y);
	pts[2] = QPoint(t3x,t3y);
	pts[3] = QPoint(t4x,t4y);
	p.drawConvexPolygon(pts,4);			// we know it is convex
}


static void drawTriangleCap(QPainter &p,drawint sx,drawint sy,double angle,int wid,int len)
{
#ifdef DEBUG_CAPS
	debugmsg(0) << funcinfo;
	p.setPen(Qt::green);
	p.setBrush(Qt::NoBrush);
#endif							// DEBUG_CAPS

	const int ws = static_cast<int>((wid*sin(angle))/16+0.5);
	const int wc = static_cast<int>((wid*cos(angle))/16+0.5);
	const int ls = static_cast<int>((len*sin(angle))/16+0.5);
	const int lc = static_cast<int>((len*cos(angle))/16+0.5);
							// cap size is 16ths of line width
	const int t1x = DrawCoord::toPixelX(sx-ws);
	const int t1y = DrawCoord::toPixelY(sy+wc);
	const int t2x = DrawCoord::toPixelX(sx-lc);
	const int t2y = DrawCoord::toPixelY(sy-ls);
	const int t3x = DrawCoord::toPixelX(sx+ws);
	const int t3y = DrawCoord::toPixelY(sy-wc);

	QPoint pts[3];
	pts[0] = QPoint(t1x,t1y);
	pts[1] = QPoint(t2x,t2y);
	pts[2] = QPoint(t3x,t3y);
	p.drawConvexPolygon(pts,3);			// we know it is convex
}


void DrawPathLineElement::drawStartCap(QPainter &p,drawint px,drawint py,Draw::startcaptyp type,int wid,int len)
{
	switch (type)
	{
case Draw::startcapROUND:	drawRoundCap(p,px,py,wid);				break;
case Draw::startcapSQUARE:	drawSquareCap(p,px,py,atan2(y-py,x-px),wid);		break;
case Draw::startcapTRIANGLE:	drawTriangleCap(p,px,py,atan2(y-py,x-px),wid,len);	break;
default:										break;
	}
}


void DrawPathLineElement::drawEndCap(QPainter &p,drawint px,drawint py,Draw::endcaptyp type,int wid,int len)
{
	switch (type)
	{
case Draw::endcapROUND:		drawRoundCap(p,x,y,wid);				break;
case Draw::endcapSQUARE:	drawSquareCap(p,x,y,atan2(py-y,px-x),wid);		break;
case Draw::endcapTRIANGLE:	drawTriangleCap(p,x,y,atan2(py-y,px-x),wid,len);	break;
default:										break;
	}
}


void DrawPathCurveElement::drawStartCap(QPainter &p,drawint px,drawint py,Draw::startcaptyp type,int wid,int len)
{
	switch (type)
	{
case Draw::startcapROUND:	drawRoundCap(p,px,py,wid);				break;
case Draw::startcapSQUARE:	drawSquareCap(p,px,py,atan2(y-py,x-px),wid);		break;
case Draw::startcapTRIANGLE:	drawTriangleCap(p,px,py,atan2(cy1-py,cx1-px),wid,len);	break;
default:										break;
	}
}


void DrawPathCurveElement::drawEndCap(QPainter &p,drawint px,drawint py,Draw::endcaptyp type,int wid,int len)
{
	switch (type)
	{
case Draw::endcapROUND:		drawRoundCap(p,x,y,wid);				break;
case Draw::endcapSQUARE:	drawSquareCap(p,x,y,atan2(y-py,x-px),wid);		break;
case Draw::endcapTRIANGLE:	drawTriangleCap(p,x,y,atan2(cy2-y,cx2-x),wid,len);	break;
default:										break;
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWPATHOBJECT - Path object					//
//									//
//  The RiscOS Draw module will fill (and also stroke of course) an	//
//  open subpath.  Qt is a bit more restrictive in this respect:  it	//
//   will only fill a closed path.  There is nothing that can be done	//
//  about a path that is really open, but a path that appears closed	//
//  (i.e. returns to its starting point) but is not finished with a	//
//  close element needs to have a close inserted there so that it	//
//  will be filled properly.  This is done while reading and building	//
//  the path.								//
//									//
//  Also detected here is the (very useful) construction where a	//
//  thick line of zero length with round caps is used to look like	//
//  a small circle (under RiscOS, this is more accurate and also more	//
//  efficient than actually drawing a circle).  Qt again doesn't	//
//  handle this particularly well, so a single dot is detected here	//
//  and drawn specially later (using a real circle).			//
//									//
//  A (very small amount of) tolerance - 8 Draw units - is used in	//
//  the checks for both those cases above.				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawPathObject::DrawPathObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	dash = NULL;					// no dash pattern yet
}


DrawPathObject::~DrawPathObject()
{
	if (dash!=NULL) delete dash;
	qDeleteAll(path);
}


bool DrawPathObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_pathstr'
	if (!DrawObject::build(rdr,diag)) return (false);

	if (!rdr.getWord(&fillcolour) ||		// read 'draw_coltyp fillcolour'
	    !rdr.getWord(&pathcolour) ||		// read 'draw_coltyp pathcolour'
	    !rdr.getWord(&pathwidth) ||			// read 'draw_pathwidth pathwidth'
	    !pathstyle.read(rdr)) return (false);	// read 'draw_pathstyle pathstyle'

	if (pathstyle.hasDashPattern())
	{
		dash = new DrawPathDashPattern();
		if (!dash->read(rdr)) return (false);	// read 'draw_dashstr data'
	}

	drawint x,y;					// current point
	drawint x1,y1,x2,y2;				// curve control points

	singledot = true;				// possibly so far, anyway
	int elm = 0;					// current element index
	int tag = Draw::pathMOVE;			// current element type
	int startx,starty;				// start point of subpath
	bool open = false;				// subpath not closed yet

	while (tag!=Draw::pathTERM)
	{
		if (rdr.sizeRemaining()<=0)		// check not end of object
		{					// if so try to finish here
			rdr.addError("Path not terminated",Draw::errorWARNING);
			path.append(new DrawPathTermElement());
			break;
		}
							// read 'enum draw_path_tagtype'
		if (!rdr.getWord((drawword *) &tag)) return (false);
		if (elm==0 && tag!=Draw::pathMOVE)	// check starts with a move
		{
			rdr.addError("Path doesn't start with move",Draw::errorWARNING);
			path.append(new DrawPathMoveElement(0,0));
			++elm;
		}
							// if not single dot, sigh relief
		if (elm==1 && tag!=Draw::pathLINE) singledot = false;
		if (elm==2 && tag!=Draw::pathTERM) singledot = false;

		switch (tag)				// add element of that type
		{
case Draw::pathTERM:	path.append(new DrawPathTermElement());
			break;

case Draw::pathMOVE:	if (open)			// previous subpath is still open
			{				// are we back at starting point?
				if (abs(x-startx)<8 && abs(y-starty)<8) path.append(new DrawPathCloseElement());
			}

			rdr.getWord((drawword *) &x); rdr.getWord((drawword *) &y);
			path.append(new DrawPathMoveElement(x,y));
			open = true;			// note new subpath now open
			startx = x; starty = y;		// note start of this subpath
			break;

case Draw::pathLINE:	rdr.getWord((drawword *) &x); rdr.getWord((drawword *) &y);
			path.append(new DrawPathLineElement(x,y));
			break;

case Draw::pathCURVE:	rdr.getWord((drawword *) &x1); rdr.getWord((drawword *) &y1);
			rdr.getWord((drawword *) &x2); rdr.getWord((drawword *) &y2);
			rdr.getWord((drawword *) &x); rdr.getWord((drawword *) &y);
			path.append(new DrawPathCurveElement(x,y,x1,y1,x2,y2));
			break;

case Draw::pathCLOSE:	path.append(new DrawPathCloseElement());
			open = false;			// this subpath now closed
			break;

default:		rdr.addError(QString("Unknown path element %1").arg(tag),Draw::errorWARNING);
			break;				// try to ignore unknown element
		}
		++elm;					// count up this element
	}

	if (singledot)					// single dot element sequence
	{						// check line is really zero length
		if (abs(path[0]->X()-path[1]->X())>8 ||
		    abs(path[0]->Y()-path[1]->Y())>8) singledot = false;
	}

	return (true);					// object finished
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Drawing and text dump						//
//									//
//////////////////////////////////////////////////////////////////////////

QPainterPath DrawPathObject::constructPath() const
{
	QPainterPath pp;
	if (!singledot)
	{
		for (QList<DrawPathElement *>::const_iterator it = path.begin();
		     it!=path.end(); ++it)
		{
			(*it)->draw(pp);
		}
	}

	return (pp);
}


static void drawPathCaps(QPainter &p,const QList<DrawPathElement *> &path,
			 Draw::startcaptyp startcap,Draw::endcaptyp endcap,
			 Draw::pathwidth width,const DrawPathStyle &pathstyle)
{
	const int triwid = pathstyle.triangleWidth()*width;
	const int trilen = pathstyle.triangleLength()*width;

	p.setBrush(p.pen().color());			// cap filled with path colour
	p.setPen(Qt::NoPen);				// and no outline

	drawint prevX = 0;
	drawint prevY = 0;
	for (int i = 0; i<path.size()-1; ++i)		// scan the original path
	{
		if (path[i]->isStroke())		// this element is a stroke
		{
			if (!path[i-1]->isStroke())
			{				// not preceded by another
				switch (startcap)
				{
case Draw::startcapBUTT:										break;
case Draw::startcapTRIANGLE:		path[i]->drawStartCap(p,prevX,prevY,startcap,triwid,trilen);	break;
default:				path[i]->drawStartCap(p,prevX,prevY,startcap,width);		break;
				}
			}

			if (!path[i+1]->isStroke())
			{				// not followed by another
				switch (endcap)
				{
case Draw::endcapBUTT:											break;
case Draw::endcapTRIANGLE:		path[i]->drawEndCap(p,prevX,prevY,endcap,triwid,trilen);	break;
default:				path[i]->drawEndCap(p,prevX,prevY,endcap,width);		break;
				}
			}
		}

		if (path[i]->hasCoords())		// note as start of next
		{
			prevX = path[i]->X();
			prevY = path[i]->Y();
		}
	}
}


bool DrawPathObject::drawPath(QPainterPath &pp,QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	const bool filltransp = DrawUtil::isTransparent(fillcolour);
	const bool pathtransp = DrawUtil::isTransparent(pathcolour);
	if (pathtransp && filltransp) return (false);	// no colour at all

	bool supportedcap = true;			// do we need custom caps?
	Draw::startcaptyp startcap = Draw::startcapBUTT;
	Draw::endcaptyp endcap = Draw::endcapBUTT;

	const int overlaypct = pathstyle.overlaySize();
	const Qt::PenJoinStyle joinstyle = pathstyle.joinStyle();
	const Qt::PenStyle penstyle = (hasDashPattern() ? dash->getPen() : Qt::SolidLine);

	const Draw::pathwidth width = (opts->flags() & PaintOptions::Preview) ? 0 : pathwidth;
	const Qt::PenCapStyle capstyle = (width==0 ? Qt::FlatCap : pathstyle.capStyle(&supportedcap,&startcap,&endcap));

	if (singledot && capstyle==Qt::RoundCap)	// special case this,
	{						// just a single dot
		const int x = path[0]->X();
		const int y = path[0]->Y();
		const int hw = width/2;

		p.setPen(Qt::NoPen);
		if (!pathtransp)			// draw the dot
		{
			p.setBrush(QBrush(DrawUtil::toQColor(pathcolour)));
			p.drawEllipse(DrawCoord::toPixelX(x-hw),DrawCoord::toPixelY(y+hw),
				      DrawCoord::toPixelH(width),DrawCoord::toPixelV(width));
		}

		if (overlaypct>0 && !filltransp)	// draw the overlay
		{
			const int overlaywidth = (width*overlaypct)/100;
			const int how = overlaywidth/2;
			p.setBrush(QBrush(DrawUtil::toQColor(fillcolour)));
			p.drawEllipse(DrawCoord::toPixelX(x-how),DrawCoord::toPixelY(y+how),
				      DrawCoord::toPixelH(overlaywidth),DrawCoord::toPixelV(overlaywidth));
		}
		return (true);				// nothing more to do
	}

	if (pp.isEmpty()) return (false);		// no path to draw

	if (pathtransp) p.setPen(Qt::NoPen);		// select path colour
	else
	{
		p.setPen(Qt::NoPen);			// need to reset, Qt 4.1 bug?
							// else caps and joins persist
							// from previous path object!
		p.setPen(QPen(QBrush(DrawUtil::toQColor(pathcolour)),
			      DrawCoord::toPixel(width),
			      penstyle,capstyle,joinstyle));
	}

	if (filltransp) p.setBrush(Qt::NoBrush);	// select fill colour
	else
	{
		if (overlaypct!=0) p.setBrush(Qt::NoBrush);
		else p.setBrush(QBrush(DrawUtil::toQColor(fillcolour)));
		if (pathstyle.fillNonZero()) pp.setFillRule(Qt::WindingFill);
	}

	p.drawPath(pp);					// primary fill/stroke and caps
	if (!supportedcap) drawPathCaps(p,path,startcap,endcap,width,pathstyle);

	if (overlaypct!=0 && !filltransp && width>0)	// overlay is present
	{
		const int overlaywidth = (width*overlaypct)/100;

		p.setPen(QPen(QBrush(DrawUtil::toQColor(fillcolour)),
			      DrawCoord::toPixel(overlaywidth),
			      penstyle,capstyle,joinstyle));
		p.setBrush(Qt::NoBrush);

		p.drawPath(pp);				// overlay line on top
		if (!supportedcap) drawPathCaps(p,path,startcap,endcap,overlaywidth,pathstyle);
	}

	return (true);
}


bool DrawPathObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (!DrawObject::draw(p,diag,opts)) return (false);
	QPainterPath pp = this->constructPath();
	return (drawPath(pp,p,diag,opts));
}


void DrawPathObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	str << indent << qSetPadChar('0')
	    << "fillcol " << hex << qSetFieldWidth(8) << fillcolour << qSetFieldWidth(0) << dec
	    << " pathcol " << hex << qSetFieldWidth(8) << pathcolour << qSetFieldWidth(0) << dec
	    << " width " << pathwidth << endl;
	str << indent << "style " << pathstyle
	    << " " << (singledot ? "singledot" : "") << endl;

	if (hasDashPattern()) dash->dump(str,indent);

	for (QList<DrawPathElement *>::const_iterator it = path.begin();
	     it!=path.end(); ++it)
	{
		str << indent << *(*it) << endl;
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWPATHOBJECTCREATOR - New object creation			//
//  Class DRAWPATHOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawPathObjectCreator : public DrawObjectCreator
{
	friend class DrawPathObjectSetup;
public:
	DrawPathObject *create(Draw::objflags flag,int layer = 0)
	{
		return (new DrawPathObject(flag,layer));
	}
};


class DrawPathObjectSetup
{
private:
	DrawPathObjectSetup();
	static DrawPathObjectSetup instance;
};


DrawPathObjectSetup DrawPathObjectSetup::instance;


DrawPathObjectSetup::DrawPathObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objPATH,new DrawPathObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWMASKOBJECT - True-mask object				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawMaskObject::DrawMaskObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	object = NULL;
	path = NULL;
}


DrawMaskObject::~DrawMaskObject()
{
	if (object!=NULL) delete object;
	if (path!=NULL) delete path;
}


bool DrawMaskObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_maskstr'
	if (!DrawObject::build(rdr,diag)) return (false);

	rdr.save();
	object = DrawObjectManager::create(rdr,diag);	// read 'draw_objhdr object'
	rdr.restore();
	if (object==NULL) return (false);

	rdr.save();					// read 'draw_pathstr path'
	path = static_cast<DrawPathObject *>(DrawObjectManager::create(rdr,diag));
	rdr.restore();
	if (path==NULL) return (false);
	path->ensureFillTransparent();

	return (true);					// object finished
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Drawing and text dump						//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawMaskObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (!DrawObject::draw(p,diag,opts)) return (false);

	QPainterPath pp = path->constructPath();
	if (pp.isEmpty()) return (false);		// single dot, meaningless for mask

	p.save();
	p.setClipPath(pp);				// the key to why this rendering is
	object->draw(p,diag,opts);			// so trivial compared with RiscOS!
	p.restore();

	if (!path->hasLineColour()) return (true);	// nothing more to do
	return (path->drawPath(pp,p,diag,opts));	// draw the mask outline
}


void DrawMaskObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	object->dump(str,(indent1+indent2),"|  ");
	path->dump(str,(indent1+indent2),"   ");
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWMASKOBJECTCREATOR - New object creation			//
//  Class DRAWMASKOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawMaskObjectCreator : public DrawObjectCreator
{
	friend class DrawMaskObjectSetup;
public:
	DrawMaskObject *create(Draw::objflags flag,int layer = 0)
	{
		return (new DrawMaskObject(flag,layer));
	}
};


class DrawMaskObjectSetup
{
private:
	DrawMaskObjectSetup();
	static DrawMaskObjectSetup instance;
};


DrawMaskObjectSetup DrawMaskObjectSetup::instance;


DrawMaskObjectSetup::DrawMaskObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objMASK,new DrawMaskObjectCreator);
}
