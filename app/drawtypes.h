/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	23-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Types and definitions for Acorn format Draw files and this		//
//  application.							//
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

#ifndef DRAWTYPES_H
#define DRAWTYPES_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Fundamental data types						//
//									//
//////////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#ifndef SIZEOF_INT
#include "config.h"
#endif							// SIZEOF_INT
#if (SIZEOF_INT==4)
typedef int drawint;
typedef unsigned int drawuint;
#else							// SIZEOF_INT
#if (SIZEOF_LONG==4)
typedef long drawint;
typedef unsigned long drawuint;
#else							// SIZEOF_LONG
#error "No suitable type found to use for Draw word data"
#endif							// SIZEOF_LONG
#endif							// SIZEOF_INT
#else							// HAVE_CONFIG_H
#ifdef QT5
#include <qglobal.h>
typedef qint32 drawint;
typedef quint32 drawuint;
#else							// QT5
#ifdef __GNUC__
#warning "No 'config.h' available, assuming 'long' for 32 bit data type"
#endif							// GNUC
typedef long drawint;					// hopefully 4 bytes each
typedef unsigned long drawuint;
#endif							// QT5
#endif							// HAVE_CONFIG_H
typedef unsigned char drawbyte;
typedef drawuint drawword;

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files and classes						//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qnamespace.h>

class QColor;
class QRect;
class QTransform;

class DrawReader;

//////////////////////////////////////////////////////////////////////////
//									//
//  Namespace DRAW -- Constants and types			 	//
//									//
//////////////////////////////////////////////////////////////////////////

namespace Draw
{
	const unsigned int fileVERSION	= 201;		// Version of drawing file
	const int setstrVERSION		= 4;		// Version of settings structure
	const int textAREAMARGIN	= 2048;		// Text area margin outside columns
	const int pathMITRELIMIT	= 0xA0000;	// Mitre limit for paths
	const int DASHBIT		= 2304;		// One dash dot (Draw units)
	const unsigned int SCALE1	= 0x00010000;	// 1:1 in transform units
	const unsigned int TRANSPARENT	= 0xFFFFFFFF;	// Transparent colour
	const int MAXLAYER		= 31;		// How many layers

	typedef drawuint coltyp;			// os_colour draw_coltyp
	typedef drawuint pathwidth;			// int draw_pathwidth
	typedef drawbyte fontid;			// unsigned char draw_fontid

	enum jointyp					// Path join types 
	{						// 'draw_packmaskJOIN 0x03'
		joinMITRED		= 0x00,
		joinROUND		= 0x01,
		joinBEVELLED		= 0x02,
		joinMASK		= 0x03
	};

	enum endcaptyp					// Path end cap types 
	{						// 'draw_packmaskENDCAP 0x0C'
		endcapBUTT		= 0x00,
		endcapROUND		= 0x04,
		endcapSQUARE		= 0x08,
		endcapTRIANGLE		= 0x0C,
		endcapMASK		= 0x0C
	};

	enum startcaptyp				// Path start cap types 
	{						// 'draw_packmaskSTARTCAP 0x30'
		startcapBUTT		= 0x00,
		startcapROUND		= 0x10,
		startcapSQUARE		= 0x20,
		startcapTRIANGLE	= 0x30,
		startcapMASK		= 0x30
	};

	enum windtyp					// Fill winding rule 
	{						// 'draw_packmaskWINDRULE 0x40'
		windNONZERO		= 0x00,
		windEVENODD		= 0x40,
		windMASK		= 0x40
	};

	enum dashtyp					// Dash pattern flags 
	{						// 'draw_packmaskDASHED 0x80'
		dashABSENT		= 0x00,
		dashPRESENT		= 0x80,
		dashMASK		= 0x80
	};

	enum reptyp					// Replication type
	{
		repHORIZONTAL		= 1,
		repVERTICAL		= 2,
		repDIAGONAL		= 3,
		repMATRIX		= 4
	};

	enum radtyp					// Radiation centre
	{
		radTOPLEFT		= 1,
		radTOPCENT		= 2,
		radTOPRIGHT		= 3,
		radCENTRIGHT		= 4,
		radBOTTRIGHT		= 5,
		radBOTTCENT		= 6,
		radBOTTLEFT		= 7,
		radCENTLEFT		= 8,
		radCENTRE		= 9
	};

	enum textopt					// Flags for text display
	{
		textKERN		= 0x01,
		textREVERSE		= 0x02,		// Reverse direction (never used)
		textBLEND		= 0x04,
		textBLOCK		= 0x40,		// Background block (never used)
		textMASK		= 0x47
	};

	enum textalign					// Text line justification 
	{
		justLEFT		= 0x00,
		justCENTRE		= 0x10,
		justRIGHT		= 0x20,
		justMASK		= 0x30
	};

	enum pathtagtype				// Path elements 
	{
		pathTERM		= 0,		// end of path 
		pathMOVE		= 2,		// move, starts new subpath 
		pathLINE		= 8,		// line 
		pathCURVE		= 6,		// bezier curve with 2 control points 
		pathCLOSE		= 5,		// close subpath with a line 
#ifdef VECTOR
		pathARC			= 100,		// arc segment (while creating) 
		pathFOUR		= 102		// four-point curve (ditto) 
#endif
	};

	enum object					// Basic object type
	{
		objFONTLIST		= 0,		// Font table 
		objTEXT			= 1,		// Text object 
		objPATH			= 2,		// Path object 
		objSPRITE		= 5,		// Sprite object 
		objGROUP		= 6,		// Group object 
		objTAGGED		= 7,		// Tagged object 
		objTEXTAREA		= 9,		// Text area 
		objTEXTCOL		= 10,		// Text column 

		objACORNSET		= 11,		// Acorn's settings object 
		objTRANSTEXT		= 12,		// Transformed text object 
		objTRANSPRITE		= 13,		// Transformed sprite object 
		objJPEG			= 16,		// JPEG image object 

		objLIB			= 100,		// Library object 
		objSET			= 101,		// Settings object 
		objSIMREP		= 102,		// Simple replicate 
		objGENREP		= 103,		// General replicate 
		objGENINST		= 104,		// General instance 
		objMASK			= 105,		// Background mask 
		objRADIATE		= 106,		// Angular replicate 
		objSIMSKEL		= 107,		// Simple-replicate skeleton 
		objGENSKEL		= 108,		// General-replicate skeleton 
		objPATTERN		= 109,		// Pattern data (GridPro)

		objUNKNOWN		= 110,		// Unknown (internal use)
		objENDMARK		= 111,		// End marker (internal use)
		objBLANK		= 112		// Blank object (internal use)
	};

	enum objflag					// Object flags
	{
		flagNULL		= 0x00,		// Null value
		flagNODISPLAY		= 0x01,		// Object is never displayed 
		flagLOCKED		= 0x02,		// Object is locked 
		flagFILEOK		= 0x03,		// These allowed in file
		flagHIDDEN		= 0x04,		// Temporarily hidden
		flagNOBBOX		= 0x08,		// Has no bounding box
		flagREGENBBOX		= 0x10		// Need to regenerate box
	};
#ifdef QT4
	Q_DECLARE_FLAGS(objflags,objflag)
#endif

	enum comptype					// Compression types
	{
		compNOCOMPRESSION	= 0,		// None (except 1 or 3 byte tag)
		compLZWALL		= 1,		// LZW bounding box and rest
		compLZWBBOX		= 2,		// Keep bounding box, LZW rest
		compLZWNOBBOX		= 3,		// Discard bounding box, LZW rest
		compPATHPACKCOORDS	= 128,		// Path: coordinate delta encoding
		compGROUPSIMPLE		= 128,		// Group: remove name, compress objects
		compTAGGEDSIMPLE	= 128,		// Tagged: keep ID+data, compress object
		compTAGGEDEXTRALZW	= 129,		// Tagged: keep ID, compress object+data
		compTEXTAREASIMPLE	= 128,		// Textarea: compress columns, copy data+text
		compTEXTAREATEXTLZW	= 129,		// Textarea: compress columns+text, copy data
	};

	enum compflag					// File compression flags
	{
		compflagONEBYTE		= 0x00000000,	// 1-byte (Draw) object tag
		compflagTHREEBYTE	= 0x00000002,	// 3-byte (Vector) object tag
		compflagNONE		= 0x00000040,	// No compression (internal use)
		compflagMASK		= 0x00000003
	};

	enum error					// Error severity
	{
		errorOK		= 0,
		errorWARNING	= 1,
		errorFATAL	= 2
	};
}

#ifdef QT4
Q_DECLARE_OPERATORS_FOR_FLAGS(Draw::objflags)
#endif

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWBOX -- Bounding box in Draw coordinates		 	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawBox
{
public:
	DrawBox(drawint xx0,drawint yy0,drawint xx1,drawint yy1);
	DrawBox();

	bool read(DrawReader &rdr);
	bool isValid() const			{ return (!(mx0==0 && my0==0 && mx1==0 && my1==0)); }
	operator QString() const;

	drawint x0() const			{ return (mx0); };
	drawint y0() const			{ return (my0); }
	drawint x1() const			{ return (mx1); }
	drawint y1() const			{ return (my1); }

private:
	drawint mx0;
	drawint my0;
	drawint mx1;
	drawint my1;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWMATRIX -- Transform matrix with the displacements in	//
//  Draw coordinates.						 	//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawMatrix
{
public:
	DrawMatrix(drawint m11,drawint m12,drawint m21,drawint m22,drawint dx,drawint dy);
	DrawMatrix(double m11,double m12,double m21,double m22,drawint dx,drawint dy);
	DrawMatrix();

	bool read(DrawReader &rdr);
	operator QString() const;

	double m11() const			{ return (mm[0]); }
	double m12() const			{ return (mm[1]); }
	double m21() const			{ return (mm[2]); }
	double m22() const			{ return (mm[3]); }
	drawint dx() const			{ return (dd[0]); }
	drawint dy() const			{ return (dd[1]); }

	bool isIdentity() const			{ return (identity); }
	bool isSimple() const			{ return (mm[1]==0.0 && mm[2]==0.0); }
	DrawMatrix simplifyScale(bool *ix,bool *iy) const;
	QTransform toTransform(bool nodisplacement = false) const;

private:
	double mm[4];
	drawint dd[2];
	bool identity;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWPATHSTYLE -- Path style description			//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawPathStyle
{
public:
	DrawPathStyle();

	bool read(DrawReader &rdr);
	operator QString() const;

	bool hasDashPattern() const;
	bool fillNonZero() const;
	Qt::PenCapStyle capStyle(bool *supported,Draw::startcaptyp *startp,Draw::endcaptyp *endp) const;
	Qt::PenJoinStyle joinStyle() const;

//	void needTriangleCaps(bool *startcap,bool *endcap) const;
	int triangleWidth() const		{ return (tricapwid); }
	int triangleLength() const		{ return (tricaphei); }
	int overlaySize() const			{ return (overlay); }

private:
	drawbyte joincapwind;
	drawbyte overlay;
	drawbyte tricapwid;
	drawbyte tricaphei;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTSTYLE -- Basic text style description			//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextStyle					// in TEXT object
{							// 'ref' only in TRANSTEXT
public:
	DrawTextStyle();

	bool read(DrawReader &rdr);
	operator QString() const;

	Draw::fontid ref() const		{ return (fontref); }
	Draw::textopt flags() const 		{ return (static_cast<Draw::textopt>(flag)); }
	short lineSpacing() const		{ return (spacing); }

private:
	Draw::fontid fontref;
	drawbyte flag;
	short spacing;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTFLAGS -- Additional text style description		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextFlags					// in TRANSTEXT object
{
public:
	DrawTextFlags();

	bool read(DrawReader &rdr);
	operator QString() const;

	Draw::textopt flags() const 		{ return (static_cast<Draw::textopt>(flag)); }

private:
	drawbyte flag;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Namespace DRAWUTIL -- Utility routines				//
//									//
//////////////////////////////////////////////////////////////////////////

namespace DrawUtil
{
	extern QColor toQColor(Draw::coltyp c);
	extern bool isTransparent(Draw::coltyp c);
	extern QByteArray &substituteRiscosSpecialChars(QByteArray &s);
}

#endif							// DRAWTYPES_H
