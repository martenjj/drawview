/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	23-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Painting options and clipping box, encapsulated as a single		//
//  convenient object.							//
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

#ifndef PAINTOPTIONS_H
#define PAINTOPTIONS_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

#include "drawtypes.h"

class QRect;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class PAINTOPTIONS -- Painting options and clipping box		//
//									//
//////////////////////////////////////////////////////////////////////////

class PaintOptions
{
public:
	enum PaintFlag
	{
		EnableClipping		= 0x01,
		DisplayBoundingBoxes	= 0x02,
		DisplaySkeletonObjects	= 0x04,
		DrawSkeletonOnly	= 0x08,
		Preview			= 0x10,
		AntiAlias		= 0x20,
	};
#ifdef QT4
	Q_DECLARE_FLAGS(PaintFlags,PaintFlag)
#endif
	PaintOptions();

	PaintOptions::PaintFlags flags() const		{ return (mFlags); }
	const DrawBox *clippingBox() const		{ return (&mClipbox); }

	void setClippingBox(const QRect &rect);
	void setFlags(PaintOptions::PaintFlags f,bool on = true)
							{ if (on) mFlags |= f; else mFlags &= ~f; }
private:
	PaintOptions::PaintFlags mFlags;
	DrawBox mClipbox;
};

#ifdef QT4
Q_DECLARE_OPERATORS_FOR_FLAGS(PaintOptions::PaintFlags)
#endif

#endif							// PAINTOPTIONS_H
