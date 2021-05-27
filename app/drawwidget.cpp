//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Library					//
//  Edit:	27-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  A widget that displays a Draw diagram.  Zoom is handled here;	//
//  scrolling is handled by the parent.					//
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

#undef DEBUG_PAINTING

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qwidget.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpalette.h>
#include <qevent.h>
#include <qdatetime.h>
#include <qrect.h>
#include <qprinter.h>
#include <qguiapplication.h>

#include "diagram.h"
#include "coord.h"
#include "paintoptions.h"

#include "drawwidget.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWWIDGET -- Widget to display a Draw diagram		//
//									//
//////////////////////////////////////////////////////////////////////////

DrawWidget::DrawWidget(QWidget *parent)
	: QWidget(parent)
{
	debugmsg(0) << funcinfo;

	diag = NULL;					// no diagram set yet
	setFixedSize(500,400);				// pick a default size
	zoom = 1.0;					// the only reasonable default
	cumulativeDelta = 0;				// accumulated mouse wheel angle
	dragStart = QPoint();				// point where drag started

	QPalette pal = palette();			// set widget background colour
	pal.setColor(QPalette::Background,Qt::white);
	setPalette(pal);
	setAutoFillBackground(true);			// drawing background is that
}


void DrawWidget::setDiagram(const DrawDiagram *dia)
{
	diag = dia;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Zoom and overall size						//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawWidget::setZoom(double scale)
{
	debugmsg(0) << funcinfo << "to " << scale;
	zoom = scale;
}


void DrawWidget::setDrawingSize(unsigned int width,unsigned int height)
{
	width = DrawCoord::scaleBy(width,zoom);
	height = DrawCoord::scaleBy(height,zoom);
	debugmsg(0) << funcinfo << "w=" << width << " h=" << height;
	setFixedSize(width,height);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Painting and printing						//
//									//
//  The latter is here because it's so similar to widget painting.	//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawWidget::paintEvent(QPaintEvent *pe)
{
#ifdef DEBUG_PAINTING
	debugmsg(0) << funcinfo << "start, rect=" << pe->rect();
	QTime start = QTime::currentTime();
#endif
	QWidget::paintEvent(pe);			// parent event first

	QPainter p;
	p.begin(this);					// start painting
	p.setClipRect(pe->rect());			// clip to event area

	if (diag!=NULL)					// render the diagram
	{
		if (paintopts.flags() & PaintOptions::AntiAlias) p.setRenderHint(QPainter::Antialiasing);

		DrawCoord::begin(this,zoom);		// set for coordinate conversions
		paintopts.setClippingBox(pe->rect());	// must come after that
		diag->draw(p,&paintopts);		// draw the diagram
		DrawCoord::end();			// finished with conversions
	}

	p.setBrush(Qt::NoBrush);			// finally outside border
	p.setPen(Qt::black);
	p.drawRect(this->rect().left(),this->rect().top(),
		   this->rect().width()-1,this->rect().height()-1);
	p.end();					// finished with painting

#ifdef DEBUG_PAINTING
	debugmsg(0) << funcinfo << "done, took " << start.msecsTo(QTime::currentTime()) << "ms" << Qt::endl;
#endif
}


void DrawWidget::printDiagram(QPaintDevice *dev)
{
	debugmsg(0) << funcinfo << "start";
	QTime start = QTime::currentTime();

	QPainter p;
	p.begin(dev);					// begin painting

	PaintOptions opts = paintopts;			// don't need clipping here
	opts.setFlags(PaintOptions::EnableClipping,false);

	DrawCoord::begin(p.device());			// set for coordinate conversions
	diag->draw(p,&opts);				// draw the diagram
	DrawCoord::end();				// finished with conversions

	p.end();					// finished with painting

	debugmsg(0) << funcinfo << "done, took " << start.msecsTo(QTime::currentTime()) << "ms" << Qt::endl;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Mouse operations							//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawWidget::wheelEvent(QWheelEvent *we)
{
    if (!(we->modifiers() & Qt::ControlModifier))	// Ctrl-wheel does zooming
    {
        we->ignore();					// otherwise default wheel action
        return;
    }

    // Based on KMixDockWidget::trayWheelEvent() in kmix/gui/kmixdockwidget.cpp

    int inc = 0;
    const int delta = we->angleDelta().y();
    cumulativeDelta += delta;

    while (cumulativeDelta>=120)
    {
        cumulativeDelta -= 120;
        ++inc;
    }
    while (cumulativeDelta <= -120)
    {
        cumulativeDelta += 120;
        ++inc;
    }

    while (inc>0)
    {
        emit wheelZoom(delta<0 ? -1 : +1);
        --inc;
    }

    we->accept();
}


void DrawWidget::mousePressEvent(QMouseEvent *ev)
{
	if (ev->button()!=Qt::LeftButton)
	{
		ev->ignore();
		return;
	}

	dragStart = ev->pos();
	QGuiApplication::setOverrideCursor(Qt::OpenHandCursor);
	ev->accept();
}


void DrawWidget::mouseMoveEvent(QMouseEvent *ev)
{
	if (dragStart.isNull())
	{
		ev->ignore();
		return;
	}

	const int dx = ev->pos().x()-dragStart.x();
	const int dy = ev->pos().y()-dragStart.y();

	emit dragScroll(dx, dy);
	ev->accept();
}


void DrawWidget::mouseReleaseEvent(QMouseEvent *ev)
{
	dragStart = QPoint();
	QGuiApplication::restoreOverrideCursor();
	ev->accept();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Access to the diagram						//
//									//
//////////////////////////////////////////////////////////////////////////

QList<QByteArray> DrawWidget::libraryObjectNames() const
{
	return (diag->libraryObjectNames());
}
