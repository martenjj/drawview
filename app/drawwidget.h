/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  A widget that displays a Draw diagram.				//
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

#ifndef DRAWWIDGET_H
#define DRAWWIDGET_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qwidget.h>

#include "paintoptions.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QPaintDevice;

class DrawDiagram;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWWIDGET -- Widget to display a Draw diagram		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawWidget : public QWidget
{
	Q_OBJECT

public:
	DrawWidget(QWidget *parent = NULL);

	void setDiagram(const DrawDiagram *dia);
	void setDrawingSize(unsigned int width,unsigned int height);
	void setZoom(double scale = 1.0);
	PaintOptions *paintOptions()			{ return (&paintopts); }

	void printDiagram(QPaintDevice *dev);

protected:
	void paintEvent(QPaintEvent *pe) override;

private:
	const DrawDiagram *diag;
	PaintOptions paintopts;
	double zoom;
};


#endif							// DRAWWIDGET_H
