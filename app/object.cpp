/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	01-Feb-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  The Draw object base class, and new object creation.		//
//									//
//  All types of Draw object are subclasses of 'DrawObject' (not to	//
//  be confused with 'Draw::object', the enumeration for an object	//
//  tag).  They have virtual functions to build themselves from file	//
//  data, draw themselves on screen, etc.				//
//									//
//  Each object type also has an associated "creator" class which,	//
//  on startup, registers a static function in the 'objmap' list so	//
//  associating an object tag type with a creator function.		//
//									//
//  To create an object of a specific type (as read from the object	//
//  header), the creator function for that tag type is called;  this	//
//  returns a "bootstrap" object with the header fields initialised	//
//  but with no other data filled in yet.  Since this is now an		//
//  object of the required type, its virtual 'build' function can be	//
//  called to read the object header and bounding box (unless there	//
//  is none) and then the object-specific data following.		//
//									//
//  Subobjects (included in a group, mask, replicate etc) are		//
//  handled by simple recursion.					//
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

#undef DEBUG_CREATE

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qmap.h>
#include <qpainter.h>

#include "reader.h"
#include "diagram.h"
#include "error.h"
#include "paintoptions.h"
#include "coord.h"

#include "unknownobject.h"

#include "object.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWOBJECT -- Abstract base class of all types of object,	//
//  implementing data and operations common to all.			//
//									//
//////////////////////////////////////////////////////////////////////////

DrawObject::DrawObject(Draw::objflags flags,int lay)
{
	mFlags = flags;					// note object flags
	mLayer = lay;					// note object layer
	mAddress = 0;					// file address, for display
}


bool DrawObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_objhdr'
	mAddress = rdr.currentObject();			// note start of this object

	if (mFlags & (Draw::flagNOBBOX|Draw::flagREGENBBOX)) return (true);
	return (mBbox.read(rdr));			// read 'draw_box bbox'
}


bool DrawObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (mFlags & Draw::flagNODISPLAY) return (false);	// not to be displayed
	if (mFlags & Draw::flagHIDDEN) return (false);		// temporarily hidden
	// TODO: check for layer here

	if (mBbox.isValid())				// hope we've got this
	{
		if (opts->flags() & PaintOptions::EnableClipping)
		{					// check clipping region
			const DrawBox *clip = opts->clippingBox();
			if (mBbox.x0()>clip->x1()) return (false);	// off right
			if (mBbox.x1()<clip->x0()) return (false);	// off left
			if (mBbox.y0()>clip->y1()) return (false);	// off top
			if (mBbox.y1()<clip->y0()) return (false);	// off bottom
		}

		if (opts->flags() & PaintOptions::DisplayBoundingBoxes)
		{					// show bounding box
			p.save();			// don't change drawing setup
			p.setBrush(Qt::NoBrush);
			p.setPen(Qt::green);
			p.setRenderHint(QPainter::Antialiasing,false);
			p.drawRect(DrawCoord::toPixelX(mBbox.x0())-1,DrawCoord::toPixelY(mBbox.y1())-1,
				   DrawCoord::toPixelH(mBbox.x1()-mBbox.x0())+1,DrawCoord::toPixelV(mBbox.y1()-mBbox.y0())+1);
			p.restore();
		}

		if (opts->flags() & PaintOptions::DrawSkeletonOnly)
		{					// show skeleton box
			p.save();			// don't change drawing setup
			p.setBrush(Qt::NoBrush);
			p.setPen(Qt::cyan);
			p.setRenderHint(QPainter::Antialiasing,false);
			p.drawRect(DrawCoord::toPixelX(mBbox.x0()),DrawCoord::toPixelY(mBbox.y1()),
				   DrawCoord::toPixelH(mBbox.x1()-mBbox.x0())-1,DrawCoord::toPixelV(mBbox.y1()-mBbox.y0())-1);
			p.restore();
		}
	}

	return (true);					// go ahead and draw
}


void DrawObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	str << indent1 << ((indent2.simplified().isEmpty()) ? "|  " : indent2) << endl;
	str << indent1 << "+- Object at " << mAddress
	    << " (" << hex << qSetFieldWidth(6) << qSetPadChar('0') << mAddress << dec << qSetFieldWidth(0) << "):"
	    << " type " << this->typeName()
	    << " flags " << hex << qSetFieldWidth(2) << qSetPadChar('0') << mFlags
	    << " layer " << dec << qSetFieldWidth(0) << qSetPadChar(' ') << mLayer << endl;
	if (mBbox.isValid()) str << indent1 << indent2 << "bbox " << mBbox << endl;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Mapping from an object type tag to the object creator.		//
//									//
//  The map pointer needs to be a simple type to guarantee that it is	//
//  initialised before any object types try to register themselves.	//
//									//
//////////////////////////////////////////////////////////////////////////

static QMap<Draw::object,DrawObjectCreator *> *objmap = NULL;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWOBJECTMANAGER -- Manage the list of known object types,	//
//  and create an object of a specified type.				//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawObjectManager::registerCreator(Draw::object type,DrawObjectCreator *cr)
{
	debugmsg(0) << funcinfo << "for type " << type;

	if (objmap==NULL) objmap = new QMap<Draw::object,DrawObjectCreator *>;
	objmap->insert(type,cr);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Create an object by reading it from a file (word-by-word via the	//
//  file reader).  The tag and size is read here, and a "bootstrap"	//
//  object of the required type is created.  This is then requested to	//
//  read and set up the rest of itself.					//
//									//
//////////////////////////////////////////////////////////////////////////

DrawObject *DrawObjectManager::create(DrawReader &rdr,DrawDiagram *diag,DrawObjectManager::CreateFlags crflags)
{							// from 'struct draw_objhdr'
	drawuint tag;
	drawuint size;

	if (rdr.getStatus()!=DrawReader::Ok) return (NULL);
	rdr.noteCurrentObject();
	rdr.setExpectedSize(2*sizeof(drawword));	// just tag and size so far

	if (!rdr.getWord(&tag,true)) return (NULL);	// read 'struct draw_extag'
	if (tag==0 && (crflags & DrawObjectManager::ExpectEndMark)) return (new DrawEndMarkObject());
							// end of list, return special
	Draw::object type = static_cast<Draw::object>(tag & 0x0000FF);
							// extract 'draw_tagtyp tag'
	int layer = (tag & 0x00FF00)>>8;		// extract 'draw_layer layer'
	Draw::objflag objflag = static_cast<Draw::objflag>((tag & 0xFF0000)>>16);
							// extract 'draw_objflags flag'
	if (layer<0 || layer>Draw::MAXLAYER)
		rdr.addError(QString("Invalid layer number %1").arg(layer),Draw::errorWARNING);
	if ((objflag & ~Draw::flagFILEOK)!=0)
		rdr.addError(QString("Reserved flags %1 not zero").arg(objflag,2,16,QChar('0')),Draw::errorWARNING);

	if (!rdr.getWord(&size)) return (NULL);		// read 'draw_sizetyp size'
							// word, even if compressed file
	if ((size % 4)!=0)				// maybe unaligned, we don't care
	{						// but object must be whole words
		rdr.addError(QString("Object size %1 not multiple of 4").arg(size));
		return (NULL);
	}

#ifdef DEBUG_CREATE
	debugmsg(0) << funcinfo << "at " << rdr.currentObject()
		   << " (" << hex << rdr.currentObject() << dec << ")"
		   << " type=" << type << " layer=" << layer << " flag=" << objflag << " size=" << size;
#endif

	const drawuint taken = 2*sizeof(drawword);	// how much read so far
	if (taken>=size)				// check size not too small
	{
		rdr.addError(QString("Impossible object size %1").arg(size));
		return (NULL);
	}
	size -= taken;					// account for tag/comp and size
	rdr.setExpectedSize(size);			// that much more to go

	DrawObject *obj;
	if (crflags & DrawObjectManager::BlankObjectOnly)
	{						// read object header only
		obj = new DrawBlankObject();		// special blank object type
		rdr.setExpectedSize(4*sizeof(drawword));
	}
	else
	{
		rdr.setExpectedSize(size);		// that much more to go
		DrawObjectCreator *cr = objmap->value(type,NULL);
		if (cr!=NULL)				// get creator for object type
		{					// create the bootstrap object
			obj = cr->create(objflag,layer);
		}
		else					// object type not known
		{					// create a default one
			obj = new DrawUnknownObject(type,objflag,layer);
		}
	}

	if (obj==NULL) return (NULL);			// should never happen
	if (obj->build(rdr,diag))			// now read in the object
	{
		rdr.discardRest();			// move on to next object
		return (obj);				// object that was built
	}

	delete obj;					// object building failed
	return (NULL);
}
