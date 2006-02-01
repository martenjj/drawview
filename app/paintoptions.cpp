/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	01-Feb-06						//
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

#undef DEBUG_PAINTOPTS

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qrect.h>

#include "coord.h"

#include "paintoptions.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class PAINTOPTIONS -- Painting options and clipping box		//
//									//
//////////////////////////////////////////////////////////////////////////

PaintOptions::PaintOptions()
{
	mFlags = PaintOptions::EnableClipping |
	         PaintOptions::DisplaySkeletonObjects |
	         PaintOptions::AntiAlias;
}


void PaintOptions::setClippingBox(const QRect &rect)
{							// extend 1 pixel for luck
	mClipbox = DrawBox(DrawCoord::fromPixelX(rect.x()-1),
			   DrawCoord::fromPixelY(rect.y()+rect.height()+1),
			   DrawCoord::fromPixelX(rect.x()+rect.width()+2),
			   DrawCoord::fromPixelY(rect.y()-2));
#ifdef DEBUG_PAINTOPTS
	debugmsg(0) << funcinfo << "rect " << rect << " -> " << mClipbox << " flags=" << mFlags;
#endif
}
