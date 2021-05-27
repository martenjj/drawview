/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Application					//
//  Edit:	26-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Main "document" window for the viewer.				//
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

#ifndef DRAWVIEW_H
#define DRAWVIEW_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <kxmlguiwindow.h>
#include <qprinter.h>

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QScrollArea;

class KSelectAction;

class DrawWidget;
class DrawDiagram;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWVIEW -- Viewer main window				//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawView : public KXmlGuiWindow
{
	Q_OBJECT

public:
	DrawView(const QString &file = QString());
	virtual ~DrawView();

	bool isValid() const			{ return (mDiagram!=NULL); }

protected:
	void drawingSizeChanged();
	void setDrawingSize(const QPageSize &size, QPageLayout::Orientation orient);

protected slots:
	void fileOpen();
	void fileDump();
	void fileExport();
	void optionsPreferences();
	void optionsPageSize();

	void slotToggleClipping(bool on);
	void slotToggleAntiAlias(bool on);
	void slotToggleBoxes(bool on);
	void slotToggleSkeletons(bool on);
	void slotZoomSelected(QAction *act);

private slots:
	void slotChangeZoom(int incr);
	void slotDragScroll(int dx, int dy);

private:
	bool loadFile(const QString &file);
	void setupActions();

	QScrollArea *wScroller;
	QWidget *wFrame;
	KSelectAction *mZoomActs;
	KSelectAction *mLibraryNameActs;

	DrawWidget *wDrawing;
	DrawDiagram *mDiagram;

	QString mDocname;
	QPageSize mPagesize;
	QPageLayout::Orientation mOrient;
};


#endif							// DRAWVIEW_H
