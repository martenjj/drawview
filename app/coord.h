/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	23-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Coordinate conversion from Draw to painter units, and vice versa.	//
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

#ifndef COORD_H
#define COORD_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QPaintDevice;

//////////////////////////////////////////////////////////////////////////
//									//
//  Namespace DRAWCOORD -- Draw unit coordinate conversion.		//
//									//
//////////////////////////////////////////////////////////////////////////

namespace DrawCoord
{
	extern int toPixelX(drawint x);
	extern int toPixelY(drawint y);
	extern int toPixelH(drawint h);
	extern int toPixelV(drawint v);
	extern int toPixel(drawint d);
	extern int toUnscaledH(drawint h);
	extern int toUnscaledV(drawint v);
	extern int fontToPoint(int p,bool scaled = true);
	extern int pointToUnscaledH(int h);
	extern int pointToUnscaledV(int v);

	extern double zoom();
	extern int scaleBy(int d,double zoom);

	extern drawint fromPixelX(int x);
	extern drawint fromPixelY(int y);

	extern void begin(const QPaintDevice *pd,double scale = 1.0);
	extern void end();
}


#endif							// COORD_H
