/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	22-May-21						//
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


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qtextstream.h>
#include <qpainter.h>
#include <qmatrix.h>

#include "coord.h"
#include "diagram.h"
#include "reader.h"
#include "spritereader.h"

#include "spriteobjects.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWIMAGEOBJECTBASE -- Abstract base class for all three	//
//  sorts of image object.						//
//									//
//////////////////////////////////////////////////////////////////////////

DrawImageObjectBase::DrawImageObjectBase(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	simple = true;					// assume so for now
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Post-process the image and transform matrix (if there is one).	//
//  If the matrix specifies simple scaling with inversion, invert	//
//  the image now and update the matrix accordingly.			//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawImageObjectBase::build3()
{
	if (!transmat.isIdentity())			// anything to do?
	{
		if (transmat.isSimple())		// is just simple scaling?
		{
			bool invx,invy;			// see if invert needed
			transmat.simplifyScale(&invx,&invy);
			if (invx || invy) image = image.mirrored(invx,invy);
			simple = true;			// note no matrix now needed
		}					// (try saying that quickly)
		else
		{
			simple = false;			// note needed for plotting
			image = image.mirrored(false,true);
		}					// correct now for Qt<->RiscOS Y
	}
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Using the supplied transform matrix is complicated because of the	//
//  Draw<->Qt Y-coordinate inversion.  If the image is simply scaled,	//
//  that is if the transform matrix is of the form [s1 0 0 s2 dx dy]	//
//  where 's1' and 's2' are both positive, then it is easier and more	//
//  accurate to draw the image into the bounding box rectangle.		//
//									//
//  Note that if the transform matrix was simply scaled but reversed	//
//  in the horizontal or vertical direction or both, then the image	//
//  and matrix will have been adjusted at the object building stage	//
//  and the above will still apply.					//
//									//
//  RiscOS (up to 4.02 anyway) didn't support anything more than	//
//  simple scaling for JPEG images, so we should only see that for	//
//  those objects here.							//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawImageObjectBase::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts,
			       const DrawMatrix *mat) const
{
	if (image.isNull()) return (false);
	if (!DrawObject::draw(p,diag,opts)) return (false);
	p.save();


	if (mat==NULL)					// simple scaling only
	{
		QRect r(DrawCoord::toPixelX(mBbox.x0()),DrawCoord::toPixelY(mBbox.y1()),
			DrawCoord::toPixelH(mBbox.x1()-mBbox.x0()),DrawCoord::toPixelV(mBbox.y1()-mBbox.y0()));
		p.drawImage(r,image);
	}
	else						// complex transform
	{
		p.translate(DrawCoord::toPixelX(mat->dx()), DrawCoord::toPixelY(mat->dy()));
		p.scale(DrawCoord::zoom(),-DrawCoord::zoom());
		p.setMatrix(QMatrix(mat->m11(),mat->m12(),mat->m21(),mat->m22(),0,0),true);
		p.scale(double(p.device()->logicalDpiX())/image.logicalDpiX(),
                        double(p.device()->logicalDpiY())/image.logicalDpiY());
		p.drawImage(0,0,image);
	}

	p.restore();
	return (true);
}


void DrawImageObjectBase::dump1(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	if (!transmat.isIdentity())
	{
		str << indent1 << indent2
		    << "transmat " << transmat << " simple " << simple
		    << Qt::endl;
	}
}


void DrawImageObjectBase::dump2(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	str << indent1 << indent2;
	if (image.isNull())
	{
		str << "null image";
	}
	else
	{
		str << "image size (" << image.width() << "," << image.height() << ")"
		    << " dpi (" << image.logicalDpiX() << "," << image.logicalDpiY() << ")"
		    << " format " << image.format()
		    << " depth " << image.depth() << " alpha " << image.hasAlphaChannel();
	}
	str << Qt::endl;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSPRITEOBJECTBASE -- Abstract base class for the two	//
//  sorts of sprite image object.					//
//									//
//////////////////////////////////////////////////////////////////////////

DrawSpriteObjectBase::DrawSpriteObjectBase(Draw::objflags flag,int layer)
	: DrawImageObjectBase(flag,layer)
{
}


bool DrawSpriteObjectBase::build2(DrawReader &rdr,DrawDiagram *diag)
{							// from 'sprite_header sprite'
	SpriteReader sr(rdr);				// read sprite, convert to image

	mode = sr.getMode();				// copy attributes from sprite
	bpp = sr.getBpp(); cols = sr.getColours();
	xsize = sr.getXsize(); ysize = sr.getYsize();
	haspalette = sr.hasPalette(); hadpalette = sr.hadPalette();
	hasmask = sr.hasMask();

	const QString es = sr.getError();		// see if read OK
	if (es.isNull()) image = sr.getImage();		// copy out the image
	else rdr.addError(es,Draw::errorWARNING);	// note sprite read error
	return (true);
}


bool DrawSpriteObjectBase::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts,
				const DrawMatrix *mat) const
{
	return (DrawImageObjectBase::draw(p,diag,opts,mat));
}


void DrawSpriteObjectBase::dump1(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawImageObjectBase::dump1(str,indent1,indent2);

	str << indent1 << indent2 << qSetPadChar('0')
	    << "mode " << mode << " (" << Qt::hex << qSetFieldWidth(8) << mode << qSetFieldWidth(0) << Qt::dec << ")"
	    << Qt::endl;

	if (bpp!=0)					// valid sprite image
	{
		str << indent1 << indent2
		    << "sprite size (" << xsize << "," << ysize << ")"
		    << " bpp " << bpp << " colours " << cols
		    << " hadpalette " << hadpalette
		    << " haspalette " << haspalette
		    << " mask " << hasmask
		    << Qt::endl;
	}

	DrawImageObjectBase::dump2(str,indent1,indent2);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSPRITEOBJECT -- Simple (untransformed) sprite object.	//
//									//
//////////////////////////////////////////////////////////////////////////

DrawSpriteObject::DrawSpriteObject(Draw::objflags flag,int layer)
	: DrawSpriteObjectBase(flag,layer)
{
}


bool DrawSpriteObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_spristr'
	if (!DrawObject::build(rdr,diag)) return (false);
	if (!DrawSpriteObjectBase::build2(rdr,diag)) return (false);
	return (DrawImageObjectBase::build3());
}


bool DrawSpriteObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	return (DrawSpriteObjectBase::draw(p,diag,opts));
}


void DrawSpriteObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawSpriteObjectBase::dump1(str,indent1,indent2);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSPRITEOBJECTCREATOR - New object creation			//
//  Class DRAWSPRITEOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawSpriteObjectCreator : public DrawObjectCreator
{
	friend class DrawSpriteObjectSetup;
public:
	DrawSpriteObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawSpriteObject(flag,layer));
	}
};


class DrawSpriteObjectSetup
{
private:
	DrawSpriteObjectSetup();
	static DrawSpriteObjectSetup instance;
};


DrawSpriteObjectSetup DrawSpriteObjectSetup::instance;


DrawSpriteObjectSetup::DrawSpriteObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objSPRITE,new DrawSpriteObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTRANSFORMEDSPRITEOBJECT -- Transformed sprite object.	//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTransformedSpriteObject::DrawTransformedSpriteObject(Draw::objflags flag,int layer)
	: DrawSpriteObjectBase(flag,layer)
{
}


bool DrawTransformedSpriteObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_transpristr'
	if (!DrawObject::build(rdr,diag)) return (false);
	if (!transmat.read(rdr)) return (false);	// read 'draw_transmat mat'

	if (!DrawSpriteObjectBase::build2(rdr,diag)) return (false);
	return (DrawImageObjectBase::build3());
}


bool DrawTransformedSpriteObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	return (DrawSpriteObjectBase::draw(p,diag,opts,(simple ? NULL : &transmat)));
}


void DrawTransformedSpriteObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawSpriteObjectBase::dump1(str,indent1,indent2);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTRANSFORMEDSPRITEOBJECTCREATOR - New object creation	//
//  Class DRAWTRANSFORMEDSPRITEOBJECTSETUP - Initialisation for above	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTransformedSpriteObjectCreator : public DrawObjectCreator
{
	friend class DrawTransformedSpriteObjectSetup;
public:
	DrawTransformedSpriteObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawTransformedSpriteObject(flag,layer));
	}
};


class DrawTransformedSpriteObjectSetup
{
private:
	DrawTransformedSpriteObjectSetup();
	static DrawTransformedSpriteObjectSetup instance;
};


DrawTransformedSpriteObjectSetup DrawTransformedSpriteObjectSetup::instance;


DrawTransformedSpriteObjectSetup::DrawTransformedSpriteObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objTRANSPRITE,new DrawTransformedSpriteObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWJPEGOBJECT -- JPEG image object.				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawJpegObject::DrawJpegObject(Draw::objflags flag,int layer)
	: DrawImageObjectBase(flag,layer)
{
}


bool DrawJpegObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'draw_jpegstr'
	if (!DrawObject::build(rdr,diag)) return (false);

	if (!rdr.getWord(&width) ||			// read 'unsigned int width,height'
	    !rdr.getWord(&height) ||
	    !rdr.getWord(&pdx) ||			// read 'unsigned int pdx,pdy'
	    !rdr.getWord(&pdy) ||
	    !transmat.read(rdr)) return (false);	// read 'draw_transmat mat'

	drawuint datasize;
	if (!rdr.getWord(&datasize)) return (false);	// read 'unsigned int datasize'
	if (datasize>rdr.sizeRemaining())
	{
		rdr.addError("JPEG data overflows object");
		return (false);
	}

	QByteArray buf(datasize,0);			// read 'int data[]'
	if (!rdr.getByte(((drawbyte *) buf.data()),datasize)) return (false);
	if (!image.loadFromData(buf,"JPEG"))
	{
		rdr.addError("JPEG image load failed",Draw::errorWARNING);
		return (false);
	}

	return (DrawImageObjectBase::build3());
}


bool DrawJpegObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	return (DrawImageObjectBase::draw(p,diag,opts,(simple ? NULL : &transmat)));
}


void DrawJpegObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawImageObjectBase::dump1(str,indent1,indent2);

	str << indent1 << indent2
	    << "jpeg size (" << width << "," << height << ")"
	    << " pdx/pdy (" << pdx << "," << pdy << ")"
	    << Qt::endl;

	DrawImageObjectBase::dump2(str,indent1,indent2);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWJPEGOBJECTCREATOR - New object creation			//
//  Class DRAWJPEGOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawJpegObjectCreator : public DrawObjectCreator
{
	friend class DrawJpegObjectSetup;
public:
	DrawJpegObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawJpegObject(flag,layer));
	}
};


class DrawJpegObjectSetup
{
private:
	DrawJpegObjectSetup();
	static DrawJpegObjectSetup instance;
};


DrawJpegObjectSetup DrawJpegObjectSetup::instance;


DrawJpegObjectSetup::DrawJpegObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objJPEG,new DrawJpegObjectCreator);
}
