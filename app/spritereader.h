/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Sprites					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	20-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  RiscOS sprite format image decoding.				//
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

#ifndef SPRITEREADER_H
#define SPRITEREADER_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qimage.h>

#include "drawtypes.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawDiagram;
class DrawReader;
class DrawError;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class SPRITEREADER - Read and convert a sprite image.		//
//									//
//////////////////////////////////////////////////////////////////////////

class SpriteReader
{
public:
	SpriteReader(DrawReader &rdr);

	QImage getImage() const		{ return (img); }
	QString getError() const	{ return (errstr); }

	drawuint getMode() const	{ return (mode); }
	int getBpp() const		{ return (bpp); }
	int getColours() const		{ return (cols); }
	int getXsize() const		{ return (xpixels); }
	int getYsize() const		{ return (ypixels); }

	bool hadPalette() const		{ return (hadpalette); }
	bool hasPalette() const		{ return (palent>0); }
	bool hasMask() const		{ return (hasmask); }

private:
	bool decodeMode();
	void defaultPalette();

	QImage img;
	QString errstr;

	drawuint mode;
	int bpp;
	int pix;
	int xpixels,ypixels;
	int xeig,yeig;
	int cols;
	int linesize;
	drawuint height,width;
	drawuint rightbit,leftbit;
	bool hasmask;
	bool hadpalette;

	int palent;
	drawword palette[256];
};

#endif							// SPRITEREADER_H
