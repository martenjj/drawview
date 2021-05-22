/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Replicate objects and skeletons.					//
//									//
//  Here the two types of replicate are called "Simple" and "General",	//
//  as originally devised and named so throughout the RiscOS source	//
//  code.  4Mation called them "Static" and "Dynamic" in the Vector	//
//  user interface and documentation.					//
//									//
//  Arbitrarily transformable general replicates (where each instance	//
//  has a transform matrix, as with NEWGENREP defined) were never	//
//  implemented in any application.  Drawing them under the RiscOS	//
//  graphics system was just far too complicated.			//
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

#include <qpainter.h>
#include <qmatrix.h>
#include <qtextstream.h>
#include <qlist.h>
#include <qalgorithms.h>

#include "reader.h"
#include "coord.h"
#include "paintoptions.h"

#include "replicates.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSIMPLEREPLICATEBASE -- Abstract base class for simple	//
//  replicates and skeletons.						//
//									//
//////////////////////////////////////////////////////////////////////////

DrawSimpleReplicateBase::DrawSimpleReplicateBase(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	object = NULL;
}


DrawSimpleReplicateBase::~DrawSimpleReplicateBase()
{
	if (object!=NULL) delete object;
}


bool DrawSimpleReplicateBase::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_simrepstr'
	if (!DrawObject::build(rdr,diag)) return (false);

	if (!rdr.getWord(&typ) ||		// read 'draw_reptyp type'
	    !rdr.getWord(&hornum) ||	// read 'int hor_num'
	    !rdr.getWord(&horsp) ||	// read 'int hor_sp'
	    !rdr.getWord(&vernum) ||	// read 'int vert_num'
	    !rdr.getWord(&versp)) return (false);
							// read 'int vert_sp'
	rdr.save();
	if (!skeleton) object = DrawObjectManager::create(rdr,diag);
	else object = DrawObjectManager::create(rdr,diag,DrawObjectManager::BlankObjectOnly);
	rdr.restore();					// read subobject or just header
	if (object==NULL) return (false);

	return (true);
}


bool DrawSimpleReplicateBase::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (skeleton && !(opts->flags() & PaintOptions::DisplaySkeletonObjects)) return (false);
	if (!DrawObject::draw(p,diag,opts)) return (false);

	p.save();					// save global state
	const QTransform m = p.transform();		// cheaper than restore()/save()

	PaintOptions newopts = *opts;			// doesn't work when matrix changed
	newopts.setFlags(PaintOptions::EnableClipping,false);
	if (skeleton) newopts.setFlags(PaintOptions::DrawSkeletonOnly);

	for (int i = 0; i<hornum; ++i)
	{
		for (int j = 0; j<vernum; ++j)
		{
			if (typ==Draw::repDIAGONAL && i!=j) continue;
			p.setTransform(m);		// reset and apply this
			p.translate(DrawCoord::toPixelH(i*horsp),-DrawCoord::toPixelV(j*versp));
			object->draw(p,diag,&newopts);	// handles skeleton or not
		}
	}

	p.restore();					// restore global state
	return (true);
}


void DrawSimpleReplicateBase::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	str << indent << "type " << typ
		<< " hornum " << hornum << " horsp " << horsp
		<< " vernum " << vernum << " versp " << versp
		<< " skeleton " << skeleton << Qt::endl;

	object->dump(str,(indent1+indent2),"   ");
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSIMPLEREPLICATEOBJECT -- Simple-replicate object.		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawSimpleReplicateObject::DrawSimpleReplicateObject(Draw::objflags flag,int layer)
	: DrawSimpleReplicateBase(flag,layer)
{
	skeleton = false;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSIMPLEREPLICATEOBJECTCREATOR - New object creation	//
//  Class DRAWSIMPLEREPLICATEOBJECTSETUP - Initialisation for above	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawSimpleReplicateObjectCreator : public DrawObjectCreator
{
	friend class DrawSimpleReplicateObjectSetup;
public:
	DrawSimpleReplicateObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawSimpleReplicateObject(flag,layer));
	}
};


class DrawSimpleReplicateObjectSetup
{
private:
	DrawSimpleReplicateObjectSetup();
	static DrawSimpleReplicateObjectSetup instance;
};


DrawSimpleReplicateObjectSetup DrawSimpleReplicateObjectSetup::instance;


DrawSimpleReplicateObjectSetup::DrawSimpleReplicateObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objSIMREP,new DrawSimpleReplicateObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSIMPLESKELETONOBJECT -- Simple-skeleton object.		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawSimpleSkeletonObject::DrawSimpleSkeletonObject(Draw::objflags flag,int layer)
	: DrawSimpleReplicateBase(flag,layer)
{
	skeleton = true;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSIMPLESKELETONOBJECTCREATOR - New object creation	//
//  Class DRAWSIMPLESKELETONOBJECTSETUP - Initialisation for above	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawSimpleSkeletonObjectCreator : public DrawObjectCreator
{
	friend class DrawSimpleSkeletonObjectSetup;
public:
	DrawSimpleSkeletonObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawSimpleSkeletonObject(flag,layer));
	}
};


class DrawSimpleSkeletonObjectSetup
{
private:
	DrawSimpleSkeletonObjectSetup();
	static DrawSimpleSkeletonObjectSetup instance;
};


DrawSimpleSkeletonObjectSetup DrawSimpleSkeletonObjectSetup::instance;


DrawSimpleSkeletonObjectSetup::DrawSimpleSkeletonObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objSIMSKEL,new DrawSimpleSkeletonObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALINSTANCEOBJECT -- Instance contained within a	//
//  general replicate or skeleton.					//
//									//
//////////////////////////////////////////////////////////////////////////

DrawGeneralInstanceObject::DrawGeneralInstanceObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
}


bool DrawGeneralInstanceObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_geninststr'
	if (!DrawObject::build(rdr,diag)) return (false);
#ifdef NEWGENREP
	if (!transmat.read(rdr)) return (false);	// read 'draw_transmat mat'
#endif
	return (true);
}


void DrawGeneralInstanceObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
#ifdef NEWGENREP
	str << indent1 << indent2 << "transmat " << transmat << Qt::endl;
#endif
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALINSTANCEOBJECTCREATOR - New object creation	//
//  Class DRAWGENERALINSTANCEOBJECTSETUP - Initialisation for above	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawGeneralInstanceObjectCreator : public DrawObjectCreator
{
	friend class DrawGeneralInstanceObjectSetup;
public:
	DrawGeneralInstanceObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawGeneralInstanceObject(flag,layer));
	}
};


class DrawGeneralInstanceObjectSetup
{
private:
	DrawGeneralInstanceObjectSetup();
	static DrawGeneralInstanceObjectSetup instance;
};


DrawGeneralInstanceObjectSetup DrawGeneralInstanceObjectSetup::instance;


DrawGeneralInstanceObjectSetup::DrawGeneralInstanceObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objGENINST,new DrawGeneralInstanceObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALREPLICATEBASE -- Abstract base class for general	//
//  replicates and skeletons.						//
//									//
//////////////////////////////////////////////////////////////////////////

DrawGeneralReplicateBase::DrawGeneralReplicateBase(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	object = NULL;
}


DrawGeneralReplicateBase::~DrawGeneralReplicateBase()
{
	qDeleteAll(instances);
	if (object!=NULL) delete object;
}


bool DrawGeneralReplicateBase::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_genrepstr'
	if (!DrawObject::build(rdr,diag)) return (false);
#ifdef NEWGENREP
	drawword w;					// skip 'pad[6]'
	for (int i = 0; i<6; ++i) if (!rdr.getWord(&w)) return (false);
#endif
	for (;;)					// read 'draw_geninststr inst[]'
	{
		rdr.save();
		DrawObject *obj = DrawObjectManager::create(rdr,diag,DrawObjectManager::ExpectEndMark);
		rdr.restore();
		if (obj==NULL) return (false);

		if (obj->type()==Draw::objENDMARK)	// have read 'int endmark'
		{					// end of replicate instances
			delete obj;			// don't need that any more
			break;				// finished reading instances
		}

		if (obj->type()!=Draw::objGENINST)	// shouldn't be anything else
		{
			rdr.addError(QString("General replicate contains non-instance object %1").arg(obj->type()),Draw::errorWARNING);
			continue;
		}

		instances.append(static_cast<DrawGeneralInstanceObject *>(obj));
	}

	rdr.save();
	if (!skeleton) object = DrawObjectManager::create(rdr,diag);
	else object = DrawObjectManager::create(rdr,diag,DrawObjectManager::BlankObjectOnly);
	rdr.restore();					// read subobject or just header
	if (object==NULL) return (false);

	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Setting the matrix Y-offset is complicated because of the need to	//
//  take account of the already existing Y offset and reversal in	//
//  toPixelY():								//
//									//
//    i = so + d		Simple scaling from object (o) to	//
//				instance (i), screen coordinates	//
//									//
//    y = (h-FY)		Convert Draw coordinate (Y) to screen	//
//				coordinate (y), using widget height	//
//				(h) and zoom+conversion factor (F)	//
//									//
//    h-FI = s(h-FO) + d	Scaling from object (O) to instance	//
//				(I), in Draw coordinates		//
//									//
//    d = h(1-s) + F(sO - I)	Rearrange previous for offset (d).	//
//				Now widget height (h) is equivalent	//
//				to toPixelY(0), while the conversion	//
//				factor (F) can be implemented by	//
//				toPixelV().				//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawGeneralReplicateBase::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (skeleton && !(opts->flags() & PaintOptions::DisplaySkeletonObjects)) return (false);
	if (!DrawObject::draw(p,diag,opts)) return (false);

	p.save();					// save global state
	const QTransform m = p.transform();		// cheaper than restore()/save()

	PaintOptions newopts = *opts;			// doesn't work when matrix changed
	newopts.setFlags(PaintOptions::EnableClipping,false);
	if (skeleton) newopts.setFlags(PaintOptions::DrawSkeletonOnly);

	for (QList<DrawGeneralInstanceObject *>::const_iterator it = instances.begin();
	     it!=instances.end(); ++it)
	{
		const DrawGeneralInstanceObject *inst = (*it);

		const int ox0 = object->bbox()->x0();
		const int oy0 = object->bbox()->y0();
		const int ox1 = object->bbox()->x1();
		const int oy1 = object->bbox()->y1();

		if (ox0==ox1) continue;			// zero sized base is impossible
		if (oy0==oy1) continue;

		const int ix0 = inst->bbox()->x0();
		const int iy0 = inst->bbox()->y0();
		const int ix1 = inst->bbox()->x1();
		const int iy1 = inst->bbox()->y1();

		const double sx = double(ix1-ix0)/(ox1-ox0);
		const double sy = double(iy1-iy0)/(oy1-oy0);

		const int mx = DrawCoord::toPixelH(ix0-qRound(sx*ox0));
		const int my = qRound(DrawCoord::toPixelY(0)*(1-sy)) +
			       DrawCoord::toPixelV(qRound(sy*oy0)-iy0);

		p.setTransform(m);				// reset and transform for this
		p.setTransform(QTransform(sx, 0, 0, sy, mx, my), true);

		object->draw(p,diag,&newopts);
	}

	p.restore();					// restore global state
	return (true);
}


void DrawGeneralReplicateBase::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	for (QList<DrawGeneralInstanceObject *>::const_iterator it = instances.begin();
	     it!=instances.end(); ++it)
	{
		(*it)->dump(str,indent,"|  ");
	}

	object->dump(str,indent,"   ");
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALREPLICATEOBJECT -- General-replicate object.	//
//									//
//////////////////////////////////////////////////////////////////////////

DrawGeneralReplicateObject::DrawGeneralReplicateObject(Draw::objflags flag,int layer)
	: DrawGeneralReplicateBase(flag,layer)
{
	skeleton = false;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALREPLICATEOBJECTCREATOR - New object creation	//
//  Class DRAWGENERALREPLICATEOBJECTSETUP - Initialisation for above	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawGeneralReplicateObjectCreator : public DrawObjectCreator
{
	friend class DrawGeneralReplicateObjectSetup;
public:
	DrawGeneralReplicateObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawGeneralReplicateObject(flag,layer));
	}
};


class DrawGeneralReplicateObjectSetup
{
private:
	DrawGeneralReplicateObjectSetup();
	static DrawGeneralReplicateObjectSetup instance;
};


DrawGeneralReplicateObjectSetup DrawGeneralReplicateObjectSetup::instance;


DrawGeneralReplicateObjectSetup::DrawGeneralReplicateObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objGENREP,new DrawGeneralReplicateObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALSKELETONOBJECT -- General-skeleton object.		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawGeneralSkeletonObject::DrawGeneralSkeletonObject(Draw::objflags flag,int layer)
	: DrawGeneralReplicateBase(flag,layer)
{
	skeleton = true;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALSKELETONOBJECTCREATOR - New object creation	//
//  Class DRAWGENERALSKELETONOBJECTSETUP - Initialisation for above	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawGeneralSkeletonObjectCreator : public DrawObjectCreator
{
	friend class DrawGeneralSkeletonObjectSetup;
public:
	DrawGeneralSkeletonObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawGeneralSkeletonObject(flag,layer));
	}
};


class DrawGeneralSkeletonObjectSetup
{
private:
	DrawGeneralSkeletonObjectSetup();
	static DrawGeneralSkeletonObjectSetup instance;
};


DrawGeneralSkeletonObjectSetup DrawGeneralSkeletonObjectSetup::instance;


DrawGeneralSkeletonObjectSetup::DrawGeneralSkeletonObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objGENSKEL,new DrawGeneralSkeletonObjectCreator);
}
