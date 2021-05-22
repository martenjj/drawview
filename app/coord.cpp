/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Coordinate conversion from Draw to painter units, and vice versa.	//
//									//
//  The main complication in doing this is that the Y axis directions	//
//  are reversed:  Draw files have their origin at the bottom left and	//
//  positive Y-direction upwards, while Qt widgets and paint devices	//
//  have their origin at the top left and positive Y-direction		//
//  downwards.  While it would be possible to allow for this - and	//
//  also perform the Draw->pixel conversion in the process - using	//
//  painter transforms, using those seems to affect performance		//
//  considerably especially when inversion is involved (i.e. a scale	//
//  factor is negative).  Instead of doing that, the relatively simple	//
//  Y-transform is applied here and the rest is just multiplication.	//
//									//
//  Before (most) of these conversion routines can be used, the paint	//
//  device needs to be specified with 'begin(pd,scale)'.  Conversion	//
//  factors appropriate to that device are calculated and used.		//
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

#undef DEBUG_COORD

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qpaintdevice.h>

#include "coord.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Internal variables							//
//									//
//////////////////////////////////////////////////////////////////////////

static int sDpiX = 90;					// specific DPI for those
static int sDpiY = 90;
static int sDpi = 90;					// general average DPI
static int sHeight = 0;					// size of destination

static const int DRAWPERINCH = 46080;			// Draw units per inch
static double sZoom = 1.0;				// viewing zoom factor
static int sFactor = DRAWPERINCH;			// conversion scaled by that

static volatile const QPaintDevice *sActive = NULL;	// currently active painter

//////////////////////////////////////////////////////////////////////////
//									//
//  Draw unit -> painter pixel						//
//									//
//////////////////////////////////////////////////////////////////////////

int DrawCoord::toPixelX(drawint x)			// convert from Draw coordinate
{
	return ((x*sDpiX)/sFactor);
}


int DrawCoord::toPixelY(drawint y)			// invert this coordinate
{
	
	return (sHeight-((y*sDpiY)/sFactor));
}


int DrawCoord::toPixelH(drawint h)			// convert from Draw units
{
	return ((h*sDpiX)/sFactor);
}


int DrawCoord::toPixelV(drawint v)			// no inversion
{
	return ((v*sDpiY)/sFactor);
}


int DrawCoord::toPixel(drawint d)
{
	return ((d*sDpi)/sFactor);
}


int DrawCoord::toUnscaledH(drawint h)			// from Draw units no zooming
{
	return ((h*sDpiX)/DRAWPERINCH);
}


int DrawCoord::toUnscaledV(drawint v)			// no inversion
{
	return ((v*sDpiY)/DRAWPERINCH);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Draw file font unit -> font size point				//
//									//
//////////////////////////////////////////////////////////////////////////

int DrawCoord::fontToPoint(int p,bool scaled)		// font units = 1/640 point
{
	if (!scaled) return (qMax(((p+320)/640),1));
	return (qMax((qRound(((p*sZoom)+320)/640)), 1));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Font size point -> painter pixel					//
//									//
//////////////////////////////////////////////////////////////////////////

int DrawCoord::pointToUnscaledH(int h)			// points = 1/72 inch
{
	return (((h*sDpiX)+36)/72);
}


int DrawCoord::pointToUnscaledV(int v)			// no inversion
{
	return (((v*sDpiY)+36)/72);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Zoom scaling							//
//									//
//////////////////////////////////////////////////////////////////////////

double DrawCoord::zoom()
{
	return (sZoom);
}


int DrawCoord::scaleBy(int d,double zoom)
{
	return (qRound(d*zoom+.5));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Painter pixel -> Draw unit						//
//									//
//////////////////////////////////////////////////////////////////////////

drawint DrawCoord::fromPixelX(int x)			// convert to Draw coordinate
{
	return ((x*sFactor)/sDpiX);
}


drawint DrawCoord::fromPixelY(int y)			// with inversion
{
	return (((sHeight-y)*sFactor)/sDpiY);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Painter and scale factor tracking					//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawCoord::begin(const QPaintDevice *pd,double scale)
{
	if (sActive!=NULL)				// check not already active
	{
		warnmsg() << funcinfo << "already active";
		return;
	}

	sActive = pd;					// note painter is active

	sZoom = scale;
	sFactor = qRound(DRAWPERINCH/scale);

	sDpiX = pd->logicalDpiX();			// DPI in each direction
	sDpiY = pd->logicalDpiY();
	sDpi = (sDpiX+sDpiY)/2;				// average for general
	sHeight = pd->height();				// note this to invert
#ifdef DEBUG_COORD
	debugmsg(0) << funcinfo << "at zoom=" << sZoom << " using factor=" << sFactor << " dpiX=" << sDpiX << " dpiY=" << sDpiY << " dpi=" << sDpi << " height=" << sHeight;
#endif
}


void DrawCoord::end()
{
	sActive = NULL;					// note no longer active
}
