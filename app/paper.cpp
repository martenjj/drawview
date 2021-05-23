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


//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include "paper.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Conversion from Qt reported page size to Draw units			//
//									//
//////////////////////////////////////////////////////////////////////////

#define mmToDraw(mm)	((mm)*1814)

#ifndef INT_MAX
#define INT_MAX (1<<(sizeof(int)*8-1))
#endif

//////////////////////////////////////////////////////////////////////////
//									//
//  Paper sizes considered when guessing the size of a drawing		//
//									//
//////////////////////////////////////////////////////////////////////////

static const QPageSize::PageSizeId guessablePageSizes[] =
{
	QPageSize::A5,
	QPageSize::A4,
	QPageSize::A3,
	QPageSize::A2,
	QPageSize::A1,
	QPageSize::A0,
	QPageSize::B0
};
static const int nGuessablePageSizes = sizeof(guessablePageSizes)/sizeof(QPageSize::PageSizeId);

//////////////////////////////////////////////////////////////////////////
//									//
//  GUESSSIZE -- Select the best fitting paper size and orientation	//
//  (not just the first that fits) to display the drawing.		//
//									//
//////////////////////////////////////////////////////////////////////////

bool PaperUtil::guessSize(const DrawBox *box,
			  QPageSize *size, QPageLayout::Orientation *orient)
{
	if (!box->isValid()) return (false);		// bounds not available

	const drawuint right = box->x1();
	const drawuint top = box->y1();

	debugmsg(0) << funcinfo << "for drawing right=" << right << " top=" << top;

	QPageSize bestps(QPageSize::A4);
	QPageLayout::Orientation bestor = QPageLayout::Landscape;
	int bestfit = INT_MAX;

	QPrinter pr(QPrinter::HighResolution);
	pr.setPageOrientation(QPageLayout::Portrait);
	pr.setFullPage(true);				// don't want any margins

	for (int i = 0; i<nGuessablePageSizes; ++i)
	{
		QPageSize ps = QPageSize(guessablePageSizes[i]);
		pr.setPageSize(ps);
		unsigned int w = mmToDraw(pr.widthMM());
		unsigned int h = mmToDraw(pr.heightMM());

		if (right<w && top<h)			// fits portrait orientation
		{
			int fit = (w-right)+(h-top);
			if (fit<bestfit)
			{
				debugmsg(0) << "  best fit portrait ps=" << ps.name() << " w=" << w << " h=" << h;
				bestfit = fit;
				bestps = ps;
				bestor = QPageLayout::Portrait;
			}
		}

		if (right<h && top<w)			// fits landscape orientation
		{
			int fit = (h-right)+(w-top);
			if (fit<bestfit)
			{
				debugmsg(0) << "  best fit landscape ps=" << ps.name() << " w=" << w << " h=" << h;
				bestfit = fit;
				bestps = ps;
				bestor = QPageLayout::Landscape;
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

void PaperUtil::getSize(const QPageSize &size, QPageLayout::Orientation orient,
                        unsigned int *width, unsigned int *height, QPrinter::Unit unit)
{
	QPrinter pr(QPrinter::ScreenResolution);
	pr.setPageSize(size);
	pr.setPageOrientation(orient);
	pr.setFullPage(true);				// don't want any margins

	debugmsg(0) << funcinfo
		    << "for size=" << size << " orient=" << orient
		    << " -> paperrect=" << pr.paperRect(unit)
		    << " res=" << pr.resolution();

	*width = qRound(pr.paperRect(unit).width());
	*height = qRound(pr.paperRect(unit).height());
}
