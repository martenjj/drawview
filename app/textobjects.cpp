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
//  More than any other sort of object, the accuracy (or, more		//
//  precisely, the fidelity compared with the original RiscOS file)	//
//  of displaying these objects depends a lot on both Qt and the	//
//  system's available fonts.  Best results are obtained with fully	//
//  scalable (TrueType) fonts - with the standard X11 bitmap ones all	//
//  bets are off.  Raid your nearest unsuspecting Windows XP machine	//
//  to obtain some.							//
//									//
//  Even so, multi-line text objects often end up with the wrong	//
//  line spacing and hence overall size.  Sorry - without access to	//
//  the original RiscOS font metrics there is no way to fix this.	//
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

#undef DEBUG_TEXT

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qtextstream.h>
#include <qpainter.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qfontinfo.h>
#include <qstringlist.h>
#include <qbytearray.h>

#include "coord.h"
#include "diagram.h"
#include "reader.h"
#include "fonts.h"

#include "textobjects.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTOBJECTBASE -- Abstract base class for the two types	//
//  of text object.							//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTextObjectBase::DrawTextObjectBase(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
}


bool DrawTextObjectBase::build2(DrawReader &rdr,DrawDiagram *diag)
{							// read 'draw_textinfo i'
	if (!rdr.getWord(&textcolour) ||		// read 'draw_coltyp textcolour'
	    !rdr.getWord(&backcolour) ||		// read 'draw_coltyp background'
	    !textstyle.read(rdr) ||			// read 'draw_textstyle textstyle'
	    !rdr.getWord(&xsize) ||			// read 'draw_fontsize fsizex'
	    !rdr.getWord(&ysize) ||			// read 'draw_fontsize fsizey'
	    !rdr.getWord(&xcoord) ||			// read 'draw_objcoord coord'
	    !rdr.getWord(&ycoord)) return (false);

	Draw::fontid ref = textstyle.ref();		// check font reference
	if (ref!=0)					// if fancy font used
	{
		if (diag->fontMap()==NULL)		// should have seen table by now
		{
			rdr.addError("Missing font table",Draw::errorWARNING);
		}
		else
		{
			if (!diag->fontMap()->isKnown(ref))
			{				// reference should be in table
				rdr.addError(QString("Reference %1 not in font table").arg(ref),Draw::errorWARNING);
			}
		}
	}

	if (!rdr.getString(&text)) return (false);	// read 'char text[]'
	DrawUtil::substituteRiscosSpecialChars(text);
	return (rdr.discardWordAlign());		// possible word alignment
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Text object drawing							//
//									//
//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_TEXT

static QString describeFont(const QFont &ff)
{
	QString s("[fam '%1' pt %2 sty %3 wgt %4 str %5]");
	return (s.arg(ff.family()).arg(ff.pointSize()).arg(ff.style())
		.arg(ff.weight()).arg(ff.stretch()));
}

static QString describeInfo(const QFontInfo &fi)
{
	QString s("[fam '%1' pt %2 sty %3 wgt %4]");
	return (s.arg(fi.family()).arg(fi.pointSize()).arg(fi.style()).arg(fi.weight()));
}

#endif


bool DrawTextObjectBase::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts,
			      Draw::textopt flags,const DrawMatrix *mat) const
{
	if (!DrawObject::draw(p,diag,opts)) return (false);

	const FontReferenceMap *fontmap = diag->fontMap();
	if (fontmap==NULL) return (false);		// shouldn't happen at this stage

	if (DrawUtil::isTransparent(textcolour)) return (false);
	p.setPen(DrawUtil::toQColor(textcolour));	// must have a colour to paint

	p.save();
	QFont font = fontmap->findFont(textstyle.ref(),ysize,xsize);
	font.setKerning(flags & Draw::textKERN);
	p.setFont(font);				// set up painter font
#ifdef DEBUG_TEXT
	debugmsg(0) << "  requested " << describeFont(p.font()) << " using " << describeInfo(p.fontInfo());
#endif

	// This was never implemented in any RiscOS application...
	// if (flags & Draw::textREVERSE) p.setLayoutDirection(Qt::RightToLeft);

	// RiscOS just uses font Y-size here (see dru_lineSpace() in lib/draw/drawutil8.c)
#ifdef DEBUG_TEXT
	debugmsg(0) << "  metrics height=" << p.fontMetrics().height() << " leading=" << p.fontMetrics().leading() << " ascent=" << p.fontMetrics().ascent();
#endif
//	const int linespace = (p.fontMetrics().lineSpacing()*textstyle.lineSpacing())/100;
//	const int linespace = (p.fontMetrics().height()*textstyle.lineSpacing())/100;
	const int linespace = (p.fontMetrics().ascent()*textstyle.lineSpacing())/100;

	const Draw::textalign just = static_cast<Draw::textalign>(flags & Draw::justMASK);

	const int xco = DrawCoord::toPixelX(xcoord);
	int yco = DrawCoord::toPixelY(ycoord);

	if (mat!=NULL)					// transformed text
	{
		p.translate(xco,yco);
		p.setMatrix(mat->toQMatrix(true),true);
		p.translate(-xco,-yco);
	}

	QList<QByteArray> lines = text.split('\n');	// handle multi-line text
	for (QList<QByteArray>::const_iterator it = lines.begin(); it!=lines.end(); ++it)
	{
		int x = xco;
		switch (just)				// adjust line justification
		{
case Draw::justCENTRE:	x -= p.fontMetrics().horizontalAdvance(*it)/2;	break;
case Draw::justRIGHT:	x -= p.fontMetrics().horizontalAdvance(*it);	break;
default:								break;
		}

		p.drawText(x,yco,QString::fromLatin1(*it));
		yco += linespace;			// move down to next line
	}

	p.restore();
	return (true);
}


void DrawTextObjectBase::dump1(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);

	const QString indent = indent1+indent2;
	str << indent << qSetPadChar('0')
	    << "textcol " << Qt::hex << qSetFieldWidth(8) << textcolour << qSetFieldWidth(0) << Qt::dec
	    << " backcol " << Qt::hex << qSetFieldWidth(8) << backcolour << qSetFieldWidth(0) << Qt::dec
	    << " style " << textstyle << Qt::endl;
	str << indent
	    << "size (" << xsize << "," << ysize << ")"
	    << " point (" << (xsize/640) << "," << (ysize/640) << ")"
	    << " coord (" << xcoord << "," << ycoord << ")"
	    << Qt::endl;
}


void DrawTextObjectBase::dump2(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	QByteArray tx(text);				// because 'replace()' modifies
	str << indent1 << indent2
	    << "text length " << tx.size()
	    << " \"" << tx.replace('\n',"\\n") << "\""
	    << Qt::endl;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTOBJECT -- Simple (untransformed) text object.		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTextObject::DrawTextObject(Draw::objflags flag,int layer)
	: DrawTextObjectBase(flag,layer)
{
}


bool DrawTextObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_textstr'
	if (!DrawObject::build(rdr,diag)) return (false);
	return (DrawTextObjectBase::build2(rdr,diag));	// read style and text
}


bool DrawTextObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	return (DrawTextObjectBase::draw(p,diag,opts,textstyle.flags()));
}


void DrawTextObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawTextObjectBase::dump1(str,indent1,indent2);
	DrawTextObjectBase::dump2(str,indent1,indent2);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTOBJECTCREATOR - New object creation			//
//  Class DRAWTEXTOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextObjectCreator : public DrawObjectCreator
{
	friend class DrawTextObjectSetup;
public:
	DrawTextObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawTextObject(flag,layer));
	}
};


class DrawTextObjectSetup
{
private:
	DrawTextObjectSetup();
	static DrawTextObjectSetup instance;
};


DrawTextObjectSetup DrawTextObjectSetup::instance;


DrawTextObjectSetup::DrawTextObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objTEXT,new DrawTextObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTRANSFORMEDTEXTOBJECT -- Transformed text object.		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTransformedTextObject::DrawTransformedTextObject(Draw::objflags flag,int layer)
	: DrawTextObjectBase(flag,layer)
{
}


bool DrawTransformedTextObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_transtextstr'
	if (!DrawObject::build(rdr,diag)) return (false);
	if (!mat.read(rdr)) return (false);		// read 'draw_transmat mat'
	if (!rdr.getWord(&textflags)) return (false);	// read 'int flags'
	return (DrawTextObjectBase::build2(rdr,diag));	// read style and text
}


bool DrawTransformedTextObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	return (DrawTextObjectBase::draw(p,diag,opts,textflags.flags(),&mat));
}


void DrawTransformedTextObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawTextObjectBase::dump1(str,indent1,indent2);

	const QString indent = indent1+indent2;
	str << indent << "matrix " << mat << " flags " << textflags << Qt::endl;
	DrawTextObjectBase::dump2(str,indent1,indent2);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTRANSFORMEDTEXTOBJECTCREATOR - New object creation	//
//  Class DRAWTRANSFORMEDTEXTOBJECTSETUP - Initialisation for above	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTransformedTextObjectCreator : public DrawObjectCreator
{
	friend class DrawTransformedTextObjectSetup;
public:
	DrawTransformedTextObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawTransformedTextObject(flag,layer));
	}
};


class DrawTransformedTextObjectSetup
{
private:
	DrawTransformedTextObjectSetup();
	static DrawTransformedTextObjectSetup instance;
};


DrawTransformedTextObjectSetup DrawTransformedTextObjectSetup::instance;


DrawTransformedTextObjectSetup::DrawTransformedTextObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objTRANSTEXT,new DrawTransformedTextObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWFONTTABLEOBJECT -- Font table object for mapping fonts	//
//  used in text objects.						//
//									//
//////////////////////////////////////////////////////////////////////////

DrawFontTableObject::DrawFontTableObject(Draw::objflags flag,int layer)
	: DrawObject((flag|Draw::flagNOBBOX),layer)
{
	fontmap = NULL;
}


DrawFontTableObject::~DrawFontTableObject()
{
	if (fontmap!=NULL) delete fontmap;
}


bool DrawFontTableObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_fontliststr'
	if (!DrawObject::build(rdr,diag)) return (false);

	if (diag->fontMap()!=NULL)			// should only ever be one
	{
		rdr.addError("Multiple font tables",Draw::errorWARNING);
		return (rdr.discardRest());
	}

	fontmap = new FontReferenceMap();		// create new map
	fontmap->add(0,"system");			// set up default font

	quint8 ref;
	QByteArray name;
	while (rdr.sizeRemaining()>3)			// at least ref,name[1],term
	{
		if (!rdr.getByte(&ref)) return (false);	// read 'draw_fontid fontref'
		if (ref=='\0') break;			// end of table?
							// read 'char fontname[]'
		if (!rdr.getString(&name)) return (false);
#ifdef DEBUG_TEXT
		debugmsg(0) << funcinfo << "ref " << ref << " -> name '" << name << "'";
#endif
		fontmap->add(ref,name);
	}

	diag->setFontMap(fontmap);			// use map in diagram
	return (rdr.discardWordAlign());		// possible word alignment
}


void DrawFontTableObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	if (fontmap!=NULL) fontmap->dump(str,indent1+indent2);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWFONTTABLEOBJECTCREATOR - New object creation		//
//  Class DRAWFONTTABLEOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawFontTableObjectCreator : public DrawObjectCreator
{
	friend class DrawFontTableObjectSetup;
public:
	DrawFontTableObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawFontTableObject(flag,layer));
	}
};


class DrawFontTableObjectSetup
{
private:
	DrawFontTableObjectSetup();
	static DrawFontTableObjectSetup instance;
};


DrawFontTableObjectSetup DrawFontTableObjectSetup::instance;


DrawFontTableObjectSetup::DrawFontTableObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objFONTLIST,new DrawFontTableObjectCreator);
}
