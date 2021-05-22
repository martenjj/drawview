/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Sprites					//
//  SCCS:	<%Z% %M% %I%>				//
//  Edit:	22-May-21						//
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

#undef DEBUG_SPRITEREADER

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qimage.h>
#include <qmap.h>
#include <qalgorithms.h>
#include <qstringlist.h>

#include "reader.h"
#include "error.h"
#include "files.h"
#include "palette.h"

#include "spritereader.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Structure SPRITEMODEDATA -- Image data for a RiscOS screen mode.	//
//									//
//////////////////////////////////////////////////////////////////////////

struct SpriteModeData
{
	SpriteModeData(int bp,int px,int xe,int ye)	{ bpp = bp; pix = px; xeig = xe; yeig = ye; }
	int bpp;
	int pix;
	int xeig;
	int yeig;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class SPRITEMODEMAP -- Mapping from a RiscOS screen mode to image	//
//  data.								//
//									//
//  The format of the configuration file ("modes") is:			//
//									//
//    mode   bpp   pix   xeig   yeig					//
//									//
//  'mode' is the RiscOS mode number.  'bpp' is the bits per pixel	//
//  of the sprite, 'pix' is the number of bits stored per pixel (these	//
//  two may not be the same).  'xeig' and 'yeig' are the expansion	//
//  factors, 1 is equivalent to 90dpi.					//
//									//
//  New "deep" sprite modes do not use this mapping, the pixel data	//
//  is encoded directly.						//
//									//
//////////////////////////////////////////////////////////////////////////

class SpriteModeMap
{
public:
	static SpriteModeMap *self();
	const struct SpriteModeData *findMode(int mode) const	{ return (map[mode]); }

protected:
	SpriteModeMap();
	~SpriteModeMap();

private:
	QMap<int,SpriteModeData *> map;
};


SpriteModeMap::SpriteModeMap()
{
	FileReader fr("modes");
	if (!fr.isValid())
	{
		fr.reportError("screen modes");
		return;
	}

	QStringList fields;
	while ((fields = fr.getParsedLine()).count()>0)
	{
		if (fields.count()!=5)
		{
synerr:			warnmsg() << funcinfo << "syntax error line #" << fr.lineNumber();
			continue;
		}

		bool ok;
		int mode = fields[0].toInt(&ok,0);
		if (!ok) goto synerr;

		int bpp = fields[1].toInt();
		int pix = fields[2].toInt();
		int xeig = fields[3].toInt();
		int yeig = fields[4].toInt();

		if (bpp==0 || pix==0 || xeig==0 || yeig==0) goto synerr;

		map[mode] = new SpriteModeData(bpp,pix,xeig,yeig);
#ifdef DEBUG_SPRITEREADER
		debugmsg(0) << funcinfo << " mode " << mode << " bpp " << bpp << " pix " << pix << " eig " << xeig << " " << yeig;
#endif
	}
}


SpriteModeMap::~SpriteModeMap()
{
	qDeleteAll(map);
}


SpriteModeMap *SpriteModeMap::self()
{
	static SpriteModeMap *mapInstance = new SpriteModeMap();
	return (mapInstance);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class SPRITEREADER -- Read and convert a sprite image.		//
//									//
//////////////////////////////////////////////////////////////////////////

SpriteReader::SpriteReader(DrawReader &rdr)
{							// from 'sprite_header'
#ifdef DEBUG_SPRITEREADER
	debugmsg(0) << funcinfo;
#endif
	errstr.clear();					// no error found yet

	mode = 0;					// default in case of failure
	bpp = pix = cols = 0;
	xpixels = ypixels = 0;
	xeig = yeig = 0;
	linesize = 0;
	height = width = 0;
	rightbit = leftbit = 0;
	hasmask = hadpalette = false;
	palent = 0;

	drawuint size;					// read 'int next' = size
	if (!rdr.getWord(&size)) return;
#ifdef DEBUG_SPRITEREADER
	debugmsg(0) << "  size=" << size;
#endif
	QByteArray name(12,'\0');			// read 'char name[12]'
	if (!rdr.getByte(((drawbyte *) name.data()),name.size())) return;
	name.resize(qstrlen(name));
#ifdef DEBUG_SPRITEREADER
	debugmsg(0) << "  name=" << name;
#endif
	if (!rdr.getWord((drawword *) &width) ||	// read 'int width,height'
	    !rdr.getWord((drawword *) &height)) return;

	if (!rdr.getWord(&leftbit) ||			// read 'int lbit,rbit'
	    !rdr.getWord(&rightbit)) return;
#ifdef DEBUG_SPRITEREADER
	debugmsg(0) << "  words=(" << width << "," << height << ")"
		<< " lbit=" << leftbit << " rbit=" << rightbit;
#endif
	drawuint imageoffset;				// read 'int image'
	if (!rdr.getWord(&imageoffset)) return;
	drawuint maskoffset;				// read 'int mask'
	if (!rdr.getWord(&maskoffset)) return;
#ifdef DEBUG_SPRITEREADER
	debugmsg(0) << "  imgoff=" << imageoffset << " maskoff=" << maskoffset;
#endif
	if (imageoffset>maskoffset)			// mask must follow image
	{
		errstr = "Sprite image/mask wrong order";
		return;
	}

	if (!rdr.getWord(&mode)) return;		// read 'sprite_mode mode'
#ifdef DEBUG_SPRITEREADER
	debugmsg(0) << "  mode=" << mode << "(" << hex << mode << dec << ")";
#endif
	if (!decodeMode()) return;

	if ((leftbit % pix)!=0)				// pixel data must be word aligned
	{
		errstr = QString("Incompatible wastage/BPP %1/%2").arg(leftbit).arg(bpp);
		return;
	}

	palent = (imageoffset-44)/(2*sizeof(drawword));	// find palette, see PRM 1-750
#ifdef DEBUG_SPRITEREADER
 	debugmsg(0) << "  palent=" << palent;
#endif
	if (palent>0)					// sprite has a palette
	{
		hadpalette = true;			// note had one originally

		if (cols>256)
		{
			errstr = QString("Palette apparently present for %1 colours").arg(cols);
			return;
		}

		if (palent>256)
		{
			errstr = QString("Palette too big with %1 entries").arg(palent);
			return;
		}

		for (int i = 0; i<palent; ++i)
		{
			if (!rdr.getWord(&palette[i]) || !rdr.getWord(NULL)) return;
		}

		if (cols==256)				// check for old format palette
		{					// if so dump it, use default
			if (palent==16 || palent==64) defaultPalette();
		}
	}
	else defaultPalette();				// sprite without palette

	if (palent>0 && palent<(1<<pix))
	{
		errstr = QString("Palette too small with %1 entries for %2 bits").arg(palent).arg(pix);
		return;
	}

	hasmask = (maskoffset>imageoffset);		// create with alpha or not
	img = QImage(xpixels,ypixels,(hasmask ? QImage::Format_ARGB32 : QImage::Format_RGB32));

	for (int pass = 1; pass<=(hasmask ? 2 : 1); ++pass)
	{						// read image and maybe mask
		for (int y = 0; y<ypixels; ++y)
		{
			drawword linebuf[linesize];
			for (int i = 0; i<linesize; ++i)
			{
				if (!rdr.getWord(&linebuf[i])) return;
			}

			int ptr = 0;
			int bits = 32-leftbit;
			unsigned int word = linebuf[ptr++]>>leftbit;

			for (int x = 0; x<xpixels; ++x)
			{
				if (bits==0)
				{
					word = linebuf[ptr++];
					bits = 32;
				}

				drawuint pixval;
				if (pix==32)
				{
					pixval = word;
					bits = 0;
				}
				else
				{
					pixval = word & ((1<<pix)-1);
					word >>= pix;
					bits -= pix;
				}

				if (pass==1)		// the sprite image
				{
					drawuint pixcol = (palent==0 ? pixval : palette[pixval]);
							// pixel colour now BBGGRRXX
					int pixr = (pixcol & 0x0000FF00)>>8;
					int pixg = (pixcol & 0x00FF0000)>>16;
					int pixb = (pixcol & 0xFF000000)>>24;
					img.setPixel(x,y,qRgb(pixr,pixg,pixb));
				}
				else			// the sprite mask
				{
					int alpha = (pixval & 1) ? 255 : 0;
					QRgb pixel = img.pixel(x,y);
					img.setPixel(x,y,qRgba(qRed(pixel),qGreen(pixel),qBlue(pixel),alpha));
				}
			}
		}

		if (!rdr.discardWordAlign()) return;
	}

	if (xeig>1 || yeig>1)				// scale now for EigFactor
	{
		img = img.scaled(xeig*img.width(),yeig*img.height());
	}

	img.setDotsPerMeterX(900000/254);		// now always at 90dpi
	img.setDotsPerMeterY(900000/254);

#ifdef DEBUG_SPRITEREADER
	debugmsg(0) << funcinfo << "done";
#endif
}

//////////////////////////////////////////////////////////////////////////
//									//
//  RiscOS screen mode decoding						//
//									//
//////////////////////////////////////////////////////////////////////////

bool SpriteReader::decodeMode()
{
	const SpriteModeData *md = SpriteModeMap::self()->findMode(mode);
	if (md!=NULL)
	{
		bpp = md->bpp;
		pix = md->pix;
		xeig = md->xeig;
		yeig = md->yeig;
	}
	else
	{
		int xres = (mode & 0x00003FFE)>>1;	// extract 'int hdpi : 13'
		int yres = (mode & 0x07FFC000)>>14;	// extract 'int vdpi : 13'
		int type = (mode & 0xF8000000)>>27;	// extract 'int type : 5'
		switch (type)
		{
case 0:			errstr = QString("Unknown sprite mode %1(%2)").arg(mode).arg(mode,8,16,QChar('0'));
			return (false);

case 1:			bpp =  1;  pix =  1;  break;
case 2:			bpp =  2;  pix =  2;  break;
case 3:			bpp =  4;  pix =  4;  break;
case 4:			bpp =  8;  pix =  8;  break;
case 5:			bpp = 15;  pix = 16;  break;
case 6:			bpp = 24;  pix = 32;  break;
//case 7:		                      break;	// 32 bit CMYK not supported
//case 8:		bpp = 24;  pix = 24;  break;	// DEEJ extension to deep sprite format
							// not supported, pixel crosses words

default:		errstr = QString("Unsupported sprite type %1").arg(type);
			return (false);
		}

		xeig = (xres==45) ? 2 : 1;		// convert DPI -> equivalent Eig
		yeig = (yres==45) ? 2 : 1;
	}

	cols = 1<<bpp;					// number of colours
	linesize = (width+1);				// words used per line
	xpixels = (width*32+(rightbit-leftbit)+1)/pix;	// width in pixels
	ypixels = height+1;				// height in pixels = lines

#ifdef DEBUG_SPRITEREADER
	debugmsg(0) << funcinfo << endl << "  eig=(" << xeig << "," << yeig << ")"
		   << " cols=" << cols << " bpp=" << bpp << " pix=" << pix
		   << " linesize=" << linesize << " pixels=(" << xpixels << "," << ypixels << ")";
#endif
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  RiscOS palette, if the sprite doesn't have one			//
//									//
//////////////////////////////////////////////////////////////////////////

void SpriteReader::defaultPalette()
{
	if (bpp<=8)					// configured or fixed palette
	{
		const drawword *pal = Palette::self()->paletteForBpp(bpp,&palent);
		if (pal==NULL)
		{
			errstr = QString("No palette defined for %1 bpp").arg(bpp);
			return;
		}
							// copy in defined palette
		for (int i = 0; i<palent; ++i) palette[i] = pal[i];
	}
	else palent = 0;				// >8bpp always true colour
}
