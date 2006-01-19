/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	28-Nov-05						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Font lookup and substitutions.					//
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

#ifndef FONTS_H
#define FONTS_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qmap.h>

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QString;
class QTextStream;
class QFont;

class FontNameMapEntry;
class FontRefMapEntry;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class FONTNAMEMAP -- Mapping from a RiscOS font name to a Qt font	//
//  family, style and weight.  There is only one mapping and only one	//
//  of these objects.							//
//									//
//////////////////////////////////////////////////////////////////////////

class FontNameMap
{
public:
	static FontNameMap *self();
	const FontNameMapEntry *findFont(const QString &name);

protected:
	FontNameMap();
	~FontNameMap();

private:
	QMap<QString,FontNameMapEntry *> list;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class FONTREFERENCEMAP -- Mapping from a drawing font reference	//
//  to a RiscOS font name, and therefore a Qt font.			//
//									//
//////////////////////////////////////////////////////////////////////////

class FontReferenceMap
{
public:
	FontReferenceMap();
	~FontReferenceMap();

	void add(Draw::fontid ref,const char *name,int size = -1,int width = -1);
	void dump(QTextStream &str,const QString &indent) const;

	QFont findFont(Draw::fontid ref,int size = -1,int width = -1) const;
	bool isKnown(Draw::fontid ref) const;

private:
	QMap<Draw::fontid,FontRefMapEntry *> list;
};

#endif							// FONTS_H
