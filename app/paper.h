/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Application					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	23-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Paper size lookup and settings.					//
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

#ifndef PAPER_H
#define PAPER_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qprinter.h>

#include "drawtypes.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Namespace PAPERUTIL -- Paper size utility routines.			//
//									//
//////////////////////////////////////////////////////////////////////////

namespace PaperUtil
{
	extern bool guessSize(const DrawBox *box,
			      QPageSize *size, QPageLayout::Orientation *orient);
	extern void getSize(const QPageSize &size, QPageLayout::Orientation orient,
                            unsigned int *width, unsigned int *height,
                            QPrinter::Unit unit = QPrinter::DevicePixel);
}

#endif							// PAPER_H
