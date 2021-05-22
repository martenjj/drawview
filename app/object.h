/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  The Draw object base class, object creation, and some very special	//
//  object types.							//
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

#ifndef DRAWOBJECT_H
#define DRAWOBJECT_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QTextStream;
class QPainter;

class DrawDiagram;
class DrawReader;
class PaintOptions;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWOBJECT -- Abstract base class of all types of object.	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawObject
{
public:
	DrawObject(Draw::objflags flag,int layer = 0);
	virtual ~DrawObject()					{}
	virtual Draw::object type() const = 0;
	virtual QString typeName() const = 0;

	const DrawBox *bbox() const				{ return (&mBbox); }

	virtual bool build(DrawReader &rdr,DrawDiagram *diag);
	virtual bool draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const;
	virtual void dump(QTextStream &str,const QString &indent1,const QString &indent2) const;

protected:
	DrawBox mBbox;

private:
	Draw::objflags mFlags;
	int mLayer;
	drawuint mAddress;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWOBJECTCREATOR -- Abstract base class of object creator.	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawObjectCreator
{
public:
	virtual DrawObject *create(Draw::objflags flag,int layer = 0) = 0;
};

typedef DrawObjectCreator *(*ObjectCreatorFunction)();

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWOBJECTMANAGER -- Manage the list of known object types,	//
//  and create an object of a specified type.				//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawObjectManager
{
public:
	enum CreateFlag { ExpectEndMark = 0x1, BlankObjectOnly = 0x2 };
#ifdef QT4
	Q_DECLARE_FLAGS(CreateFlags,CreateFlag);
#endif

	static DrawObject *create(DrawReader &rdr,DrawDiagram *diag,
				  DrawObjectManager::CreateFlags flags = DrawObjectManager::CreateFlags());
	static void registerCreator(Draw::object,DrawObjectCreator *);
};

#ifdef QT4
Q_DECLARE_OPERATORS_FOR_FLAGS(DrawObjectManager::CreateFlags);
#endif

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWENDMARKOBJECT -- Special object type returned from the	//
//  reader, to mark the end of a list of text columns or		//
//  replicate/skeleton instances.					//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawEndMarkObject : public DrawObject
{
public:
	DrawEndMarkObject() : DrawObject(Draw::flagNULL)	{}

	Draw::object type() const override 	{ return (Draw::objENDMARK); }
	QString typeName() const override	{ return ("ENDMARK"); }

	bool build(DrawReader &rdr,DrawDiagram *diag) override		{ return (true); }
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWBLANKOBJECT -- An object with header (and bounding box)	//
//  only, with no additional information.  Used in skeleton objects.	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawBlankObject : public DrawObject
{
public:
	DrawBlankObject() : DrawObject(Draw::flagNULL)		{}

	Draw::object type() const override 	{ return (Draw::objBLANK); }
	QString typeName() const override	{ return ("BLANK"); }
};

#endif							// DRAWOBJECT_H
