/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	16-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Replicate objects and skeletons.					//
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

#ifndef REPLICATES_H
#define REPLICATES_H

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

class DrawDiagram;
class DrawReader;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSIMPLEREPLICATEBASE -- Abstract base class for simple	//
//  replicates and skeletons.						//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawSimpleReplicateBase : public DrawObject
{
public:
	DrawSimpleReplicateBase(Draw::objflags flag,int layer = 0);
	~DrawSimpleReplicateBase();

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

protected:
	bool skeleton;

private:
	Draw::reptyp typ;
	int hornum,vernum;
	int horsp,versp;
	DrawObject *object;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSIMPLEREPLICATEOBJECT -- Simple-replicate object.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawSimpleReplicateObject : public DrawSimpleReplicateBase
{
public:
	DrawSimpleReplicateObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objSIMREP); }
	QString typeName() const	{ return ("SIMREP"); }
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWSIMPLESKELETONOBJECT -- Simple-skeleton object.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawSimpleSkeletonObject : public DrawSimpleReplicateBase
{
public:
	DrawSimpleSkeletonObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objSIMSKEL); }
	QString typeName() const	{ return ("SIMSKEL"); }
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALINSTANCEOBJECT -- Instance contained within a	//
//  general replicate or skeleton.					//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawGeneralInstanceObject : public DrawObject
{
public:
	DrawGeneralInstanceObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objGENINST); }
	QString typeName() const	{ return ("GENINST"); }

	bool build(DrawReader &rdr,DrawDiagram *diag);
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

private:
#ifdef NEWGENREP
	Draw::Matrix transmat;
#endif
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALREPLICATEBASE -- Abstract base class for general	//
//  replicates and skeletons.						//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawGeneralReplicateBase : public DrawObject
{
public:
	DrawGeneralReplicateBase(Draw::objflags flag,int layer = 0);
	~DrawGeneralReplicateBase();

	bool build(DrawReader &rdr,DrawDiagram *diag);
	bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

protected:
	bool skeleton;

private:
	QList<DrawGeneralInstanceObject *> instances;
	DrawObject *object;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALREPLICATEOBJECT -- General-replicate object.	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawGeneralReplicateObject : public DrawGeneralReplicateBase
{
public:
	DrawGeneralReplicateObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objGENREP); }
	QString typeName() const	{ return ("GENREP"); }
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWGENERALSKELETONOBJECT -- General-skeleton object.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawGeneralSkeletonObject : public DrawGeneralReplicateBase
{
public:
	DrawGeneralSkeletonObject(Draw::objflags flag,int layer = 0);

	Draw::object type() const	{ return (Draw::objGENSKEL); }
	QString typeName() const	{ return ("GENSKEL"); }
};

#endif							// REPLICATES_H
