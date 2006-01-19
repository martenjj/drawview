/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Application					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	17-Jan-06						//
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


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include "paper.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Conversion from Qt reported page size to Draw units			//
//									//
//////////////////////////////////////////////////////////////////////////

#define mmToDraw(mm)	((mm)*1814)

//////////////////////////////////////////////////////////////////////////
//									//
//  Paper sizes considered when guessing the size of a drawing		//
//									//
//////////////////////////////////////////////////////////////////////////

static const QPrinter::PageSize guessablePageSizes[] =
{
	QPrinter::A5,
	QPrinter::A4,
	QPrinter::A3,
	QPrinter::A2,
	QPrinter::A1,
	QPrinter::A0,
	QPrinter::B0
};
static const int nGuessablePageSizes = sizeof(guessablePageSizes)/sizeof(QPrinter::PageSize);

//////////////////////////////////////////////////////////////////////////
//									//
//  GUESSSIZE -- Select the best fitting paper size and orientation	//
//  (not just the first that fits) to display the drawing.		//
//									//
//////////////////////////////////////////////////////////////////////////

bool PaperUtil::guessSize(const DrawBox *box,
			  QPrinter::PageSize *size,QPrinter::Orientation *orient)
{
	if (!box->isValid()) return (false);		// bounds not available

	const drawuint right = box->x1();
	const drawuint top = box->y1();

	debugmsg(0) << funcinfo << "for drawing right=" << right << " top=" << top;

	QPrinter::PageSize bestps = QPrinter::A4;
	QPrinter::Orientation bestor = QPrinter::Landscape;
	int bestfit = INT_MAX;

	QPrinter pr(QPrinter::HighResolution);
	pr.setOrientation(QPrinter::Portrait);
	pr.setFullPage(true);				// don't want any margins

	for (int i = 0; i<nGuessablePageSizes; ++i)
	{
		QPrinter::PageSize ps = guessablePageSizes[i];
		pr.setPageSize(ps);
		unsigned int w = mmToDraw(pr.widthMM());
		unsigned int h = mmToDraw(pr.heightMM());

		if (right<w && top<h)			// fits portrait orientation
		{
			int fit = (w-right)+(h-top);
			if (fit<bestfit)
			{
				debugmsg(0) << "  best fit portrait ps=" << ps << " w=" << w << " h=" << h;
				bestfit = fit;
				bestps = ps;
				bestor = QPrinter::Portrait;
			}
		}

		if (right<h && top<w)			// fits landscape orientation
		{
			int fit = (h-right)+(w-top);
			if (fit<bestfit)
			{
				debugmsg(0) << "  best fit landscape ps=" << ps << " w=" << w << " h=" << h;
				bestfit = fit;
				bestps = ps;
				bestor = QPrinter::Landscape;
			}
		}
	}

	if (bestfit==INT_MAX) return (false);		// nothing suitable found

	*size = bestps;
	*orient = bestor;
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  GETSIZE -- Read the dimensions for a selected paper size and	//
//  orientation.							//
//									//
//////////////////////////////////////////////////////////////////////////

void PaperUtil::getSize(QPrinter::PageSize size,QPrinter::Orientation orient,
			unsigned int *width,unsigned int *height)
{
	QPrinter pr(QPrinter::ScreenResolution);
	pr.setPageSize(size);
	pr.setOrientation(orient);
	pr.setFullPage(true);				// don't want any margins

	debugmsg(0) << funcinfo
		    << "for size=" << size << " orient=" << orient
		    << " -> paperrect=" << pr.paperRect()
		    << " pagerect=" << pr.pageRect()
		    << " width=" << pr.width() << " height=" << pr.height()
		    << " res=" << pr.resolution();

	*width = pr.paperRect().width();
	*height = pr.paperRect().height();
}
