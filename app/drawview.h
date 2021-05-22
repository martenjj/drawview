/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Application					//
//  Edit:	22-May-21						//
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

#include <qmainwindow.h>
#include <qprinter.h>

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QWidget;
class QScrollArea;
class QAction;
class QActionGroup;

class DrawWidget;
class DrawDiagram;
//class PaperMenus;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWVIEW -- Viewer main window				//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawView : public QMainWindow
{
	Q_OBJECT

public:
	DrawView(const QString &file = QString());
	~DrawView();

	bool isValid() const			{ return (mDiagram!=NULL); }

protected:
	void drawingSizeChanged();
	void setDrawingSize(QPrinter::PageSize size,QPrinter::Orientation orient);

protected slots:
	void fileOpen();
	void fileDump();
	void fileExport();
	void optionsPreferences();
	void optionsPageSize();
	void aboutMe();

	void slotFileMenuAboutToShow();

	void slotToggleClipping(bool on);
	void slotToggleAntiAlias(bool on);
	void slotToggleBoxes(bool on);
	void slotToggleSkeletons(bool on);
	void slotZoomSelected(QAction *act);
	void slotZoomIn();
	void slotZoomOut();

private:
	bool loadFile(const QString &file);
	void setupActions();
	void changeZoom(int incr);

	QScrollArea *wScroller;
	QWidget *wFrame;
	QActionGroup *mZoomacts;
	QAction *aExport;
	QAction *aDump;

	DrawWidget *wDrawing;
	DrawDiagram *mDiagram;

	QString mDocname;
	QPrinter::PageSize mPagesize;
	QPrinter::Orientation mOrient;
};


#endif							// DRAWVIEW_H
