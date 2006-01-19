/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Sprites					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	16-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  RiscOS colour palette lookup.					//
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

#undef DEBUG_PALETTE

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qmap.h>
#include <qalgorithms.h>
#include <qstringlist.h>

#include "files.h"

#include "palette.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Structure PALETTEDATA -- RiscOS palette data for a given number	//
//  of colours.								//
//									//
//////////////////////////////////////////////////////////////////////////

struct PaletteData
{
	PaletteData(int n)	{ ncols = n; cols = new drawword[ncols]; }
	~PaletteData()		{ delete[] cols; }
	int ncols;
	drawword *cols;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class PALETTE - Mapping from a RiscOS palette index to a true	//
//  colour.
//									//
//  The format of the configuration file ("palette") is:		//
//									//
//    [bpp]   index   colour						//
//									//
//  'ncols' starts a palette table for that number of bits per pixel.	//
//  'index' is the palette index for which the true 'colour' is in	//
//  BBRRGG00 format.							//
//									//
//////////////////////////////////////////////////////////////////////////

Palette::Palette()
{
	FileReader fr("palette");
	if (!fr.isValid())
	{
		fr.reportError("palette");
		return;
	}

	QStringList fields;
	while ((fields = fr.getParsedLine()).count()>0)
	{
		if (fields.count()!=3)
		{
synerr:			warnmsg() << funcinfo << "syntax error line #" << fr.lineNumber();
			continue;
		}

		bool ok;
		int bpp = fields.takeFirst().toInt(&ok,0);
		if (!ok) goto synerr;

		int ncols = 1<<bpp;
		PaletteData *p = new PaletteData(ncols);

		for (int i = 0; i<ncols; ++i)
		{
			if (i>0) fields = fr.getParsedLine();
			if (fields.count()!=2) goto synerr;

			int idx = fields[0].toInt();
			if (idx<0 || idx>=ncols) goto synerr;

			drawword ent = fields[1].toUInt(NULL,16);
			p->cols[idx] = ent;
		}

		map[bpp] = p;
#ifdef DEBUG_PALETTE
		debugmsg(0) << funcinfo << "for " << bpp << " bpp";
#endif
	}

	PaletteData *p256 = new PaletteData(256);	// for 8bpp is fixed,
	for (int i = 0; i<256; ++i)			// not from config file
	{
		drawuint t = i & 3;
		drawuint r = ((i>>1) & 8) | (i      & 4) | t;
		drawuint g = ((i>>3) & 12)               | t;
		drawuint b = ((i>>4) & 8) | ((i>>1) & 4) | t;
		p256->cols[i] = ((r*0x11)<<8) + ((g*0x11)<<16) + ((b*0x11)<<24);
	}

	map[8] = p256;
#ifdef DEBUG_PALETTE
	debugmsg(0) << funcinfo << "for " << 8 << " bpp";
#endif
}


Palette::~Palette()
{
	qDeleteAll(map);
}


Palette *Palette::self()
{
	static Palette *palInstance = new Palette();
	return (palInstance);
}


const drawword *Palette::paletteForBpp(int bpp,int *palent) const
{
	PaletteData *p = map[bpp];
	if (p==NULL) return (NULL);
	if (palent!=NULL) *palent = p->ncols;
	return (p->cols);
}
