//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Application					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Main "document" window for the viewer.  Handles scrolling and	//
//  zooming of the drawing view widget, and the menus and dialogues.	//
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

#include <qapplication.h>
#include <qmainwindow.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qregexp.h>
#include <qwidget.h>
#include <qaction.h>
#include <qactiongroup.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <qframe.h>
#include <qgridlayout.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qalgorithms.h>
#include <qlist.h>
#include <qpoint.h>
#include <qprinter.h>
#include <qprintdialog.h>
#include <qpagesetupdialog.h>
#include <qsvggenerator.h>
#include <qdesktopwidget.h>

#include "drawwidget.h"
#include "diagram.h"
#include "paper.h"

#include "drawview.h"
#include "drawview.moc"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWVIEW -- Viewer main window				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawView::DrawView(const QString &file)
	: QMainWindow(NULL)
{
	debugmsg(0) << funcinfo << "file=" << file;

	mDiagram = NULL;				// no diagram yet

	setAcceptDrops(false);				// not accepted just anywhere
	setAttribute(Qt::WA_DeleteOnClose);		// close me and I'll go away
	setWindowTitle(qApp->applicationName());

	wScroller = new QScrollArea(this);		// main scrolling area
	wScroller->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	wScroller->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	wFrame = new QWidget(this);			// border around drawing
	wFrame->setCursor(Qt::ForbiddenCursor);		// nothing happens here
	wScroller->setWidget(wFrame);			// set scroll area widget
	wScroller->setWidgetResizable(true);		// adjust to fill window

	QGridLayout *lay = new QGridLayout(wFrame);	// layout for that border
	lay->setSpacing(0);				// minimal spacing outside
	lay->setMargin(2);				// minimal spacing inside

	wDrawing = new DrawWidget(wFrame);		// the actual drawing
	wDrawing->setCursor(Qt::CrossCursor);
	lay->addWidget(wDrawing,0,0,Qt::AlignCenter);	// centered within border area

	setCentralWidget(wScroller);			// content of main window
	setupActions();					// menu and shortcut actions
							// default page settings
	setDrawingSize(QPageSize(QPageSize::A4), QPageLayout::Landscape);

	if (!file.isNull())				// file to load specified
	{
		if (!loadFile(file)) return;		// load it, give up if error
	}

	this->show();					// show us as window
	debugmsg(0) << funcinfo << "done" << Qt::endl;
}


DrawView::~DrawView()
{
	if (mDiagram!=NULL) delete mDiagram;
	debugmsg(0) << funcinfo << "done" << Qt::endl;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Menus and keystroke actions						//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawView::setupActions()
{
	debugmsg(0) << funcinfo;

	static const int zooms[] = { 10, 20, 33, 50, 75, 100, 125, 150, 200, 330, 500, 1000 };
	static const int nzooms = sizeof(zooms)/sizeof(int);

        QMenu *mfile = menuBar()->addMenu("&File");
        QMenu *mview = menuBar()->addMenu("&View");
        QMenu *msett = menuBar()->addMenu("&Settings");
        menuBar()->addSeparator();
        QMenu *mhelp = menuBar()->addMenu("&Help");

	QAction *act = new QAction("&Open...",this);
	act->setShortcut(Qt::CTRL+Qt::Key_O);
	connect(act,SIGNAL(triggered()),this,SLOT(fileOpen()));
	mfile->addAction(act);

	aExport = new QAction("Ex&port...",this);
	//act->setShortcut(Qt::CTRL+Qt::Key_P);
	connect(aExport,SIGNAL(triggered()),this,SLOT(fileExport()));
	mfile->addAction(aExport);

	aDump = new QAction("&Dump",this);
	connect(aDump,SIGNAL(triggered()),this,SLOT(fileDump()));
	mfile->addAction(aDump);
	mfile->addSeparator();

	act = new QAction("&Close",this);
	act->setShortcut(Qt::CTRL+Qt::Key_W);
	connect(act,SIGNAL(triggered()),this,SLOT(close()));
	mfile->addAction(act);

	act = new QAction("&Quit",this);
	act->setShortcut(Qt::CTRL+Qt::Key_Q);
	connect(act,SIGNAL(triggered()),qApp,SLOT(quit()));
	mfile->addAction(act);

	act = new QAction("P&age Size...",this);
	connect(act,SIGNAL(triggered()),this,SLOT(optionsPageSize()));
	msett->addAction(act);

	act = new QAction("&Preferences...",this);
	act->setEnabled(false);
	connect(act,SIGNAL(triggered()),this,SLOT(optionsPreferences()));
	msett->addAction(act);

	QMenu *mzoom = new QMenu("&Zoom");

	act = new QAction("&In",this);
	act->setShortcut(Qt::CTRL+Qt::Key_Up);
	connect(act,SIGNAL(triggered()),this,SLOT(slotZoomIn()));
	mzoom->addAction(act);

	act = new QAction("&Out",this);
	act->setShortcut(Qt::CTRL+Qt::Key_Down);
	connect(act,SIGNAL(triggered()),this,SLOT(slotZoomOut()));
	mzoom->addAction(act);

	mzoom->addSeparator();

	mZoomacts = new QActionGroup(this);
	connect(mZoomacts,SIGNAL(triggered(QAction *)),this,SLOT(slotZoomSelected(QAction *)));
	for (int i = 0; i<nzooms; ++i)
	{
		const int z = zooms[i];
		act = new QAction(QString("%1%").arg(z), this);
		act->setCheckable(true);
		act->setData(z);
		mZoomacts->addAction(act);
		mzoom->addAction(act);
		if (z==100)
		{
			act->setShortcut(Qt::CTRL+Qt::Key_1);
			act->setChecked(true);
		}
	}

	mview->addMenu(mzoom);
	mview->addSeparator();

	act = new QAction("&Clipping",this);
	act->setCheckable(true);
	act->setChecked(wDrawing->paintOptions()->flags() & PaintOptions::EnableClipping);
	connect(act,SIGNAL(triggered(bool)),this,SLOT(slotToggleClipping(bool)));
	mview->addAction(act);

	act = new QAction("&Antialiasing",this);
	act->setCheckable(true);
	act->setChecked(wDrawing->paintOptions()->flags() & PaintOptions::AntiAlias);
	connect(act,SIGNAL(triggered(bool)),this,SLOT(slotToggleAntiAlias(bool)));
	mview->addAction(act);

	act = new QAction("&Bounding boxes",this);
	act->setCheckable(true);
	act->setChecked(wDrawing->paintOptions()->flags() & PaintOptions::DisplayBoundingBoxes);
	connect(act,SIGNAL(triggered(bool)),this,SLOT(slotToggleBoxes(bool)));
	mview->addAction(act);

	act = new QAction("S&keleton objects",this);
	act->setCheckable(true);
	act->setChecked(wDrawing->paintOptions()->flags() & PaintOptions::DisplaySkeletonObjects);
	connect(act,SIGNAL(triggered(bool)),this,SLOT(slotToggleSkeletons(bool)));
	mview->addAction(act);

	act = new QAction(QString("&About %1...").arg(qApp->applicationName()),this);
	connect(act,SIGNAL(triggered()),this,SLOT(aboutMe()));
	mhelp->addAction(act);

	act = new QAction("About &Qt...",this);
	connect(act,SIGNAL(triggered()),qApp,SLOT(aboutQt()));
	mhelp->addAction(act);

	connect(mfile,SIGNAL(aboutToShow()),this,SLOT(slotFileMenuAboutToShow()));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  "File" menu operations						//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawView::slotFileMenuAboutToShow()
{
	aExport->setEnabled(mDiagram!=NULL);
	aDump->setEnabled(mDiagram!=NULL);
}


bool DrawView::loadFile(const QString &file)
{
	if (file.isEmpty()) return (false);
	QString infile = (file=="-") ? "/dev/stdin" : file;
	QFile qf(infile);				// stdin is not actually supported

	DrawDiagram *dg = new DrawDiagram(infile);
	if (!dg->isValid())
	{
		QMessageBox::critical(NULL,("Error - "+qApp->applicationName()),
				      QString("Error loading drawing file '%2'\n%1").arg(dg->drawError()).arg(infile),
				      QMessageBox::Cancel,QMessageBox::NoButton,QMessageBox::NoButton);
		delete dg;
		return (false);
	}

	if (mDiagram!=NULL) delete mDiagram;		// have a diagram already?
	mDiagram = dg;					// note the new one
	wDrawing->setDiagram(mDiagram);			// tell display about it

	QPageSize size;
	QPageLayout::Orientation orient;		// guess a page size
	if (PaperUtil::guessSize(mDiagram->boundingBox(),&size,&orient)) setDrawingSize(size,orient);

	mDocname = infile.remove(QRegExp("^.*/"));	// save for possible printing
	setWindowTitle(QString("%2 - %1").arg(qApp->applicationName()).arg(mDocname));

	if (!mDiagram->drawError().isEmpty())
	{
		QMessageBox::information(NULL,("Message - "+qApp->applicationName()),
					 QString("Problem in drawing file '%2'\n%1").arg(mDiagram->drawError()).arg(infile),
					 QMessageBox::Ignore,QMessageBox::NoButton,QMessageBox::NoButton);
	}

	return (true);
}


void DrawView::fileDump()
{
	if (mDiagram==NULL) return;
	QTextStream str(stdout,QIODevice::WriteOnly);
	mDiagram->dump(str);
}


void DrawView::fileOpen()
{
	QStringList openfilters;
	openfilters.append("All drawing files (*.aff *.vec *.c56 *.c4b *.cc3)");
	openfilters.append("Draw files (*.aff)");
	openfilters.append("Vector files (*.vec *.c56)");
	openfilters.append("Compressed Draw files (*.c4b)");
	openfilters.append("Poster files (*.cc3)");
	openfilters.append("All files (*)");

	static QDir opendir = QDir::current();		// start in current directory

	QFileDialog d(this, "Open File", "", openfilters.join(";;"));
	d.setFileMode(QFileDialog::ExistingFile);
	d.setAcceptMode(QFileDialog::AcceptOpen);
	d.setDirectory(opendir);			// use previous directory

	if (!d.exec()) return;				// run the dialogue

	opendir = d.directory();			// remember selected directory
	QString newfile = d.selectedFiles()[0];

	if (newfile.isNull()) return;
	if (mDiagram==NULL) loadFile(newfile);
	else
	{
		(void) new DrawView(newfile);
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Export (which is really printing)					//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawView::fileExport()
{
	if (mDiagram==NULL) return;			// nothing to print

	QStringList filters;
	filters.append("Portable Document Format (*.pdf)");
	filters.append("PostScript (*.ps)");
	filters.append("Scalable Vector Graphics (*.svg)");

	static QDir expdir = QDir::current();		// start in current directory

	QFileDialog d(this, "Export To File", "", filters.join(";;"));
	d.setFileMode(QFileDialog::AnyFile);
	d.setAcceptMode(QFileDialog::AcceptSave);
	d.setLabelText(QFileDialog::LookIn,"Save in:");
	d.setLabelText(QFileDialog::Accept,"Export");
	d.setConfirmOverwrite(true);
	d.setDirectory(expdir);				// use previous directory

	if (!d.exec()) return;				// run the dialogue

	expdir = d.directory();				// remember selected directory
	QString expfile = d.selectedFiles().value(0);
	if (expfile.isNull())
	{
		QMessageBox::warning(this,("Error - "+qApp->applicationName()),
				     "No file name entered",
				     QMessageBox::Cancel,QMessageBox::NoButton,QMessageBox::NoButton);
		return;
	}

	const QString sf = d.selectedNameFilter();	// see which filter was used
	QString ext = "";
	QRegExp rx("\\(\\*\\.(\\w+)");
	if (sf.indexOf(rx)>=0) ext = rx.cap(1);		// extract extension from that
	if (!ext.isNull() && !expfile.endsWith("."+ext)) expfile += "."+ext;

	QPaintDevice *device;
	QString formatName;
	if (ext=="svg")
	{
		formatName = "SVG";

		QSvgGenerator *gen = new QSvgGenerator;

		gen->setFileName(expfile);
		gen->setTitle(expfile);
		gen->setDescription(tr("Generated by %1").arg(qApp->applicationName()));

		// The setSize() below sets the SVG page size, which is output
		// in millimetres.  However, coordinates in the generated SVG
		// are output without units (i.e. in user units).  In order
		// to get the correct object sizes, the SVG resolution needs
		// to be set to the same as the current screen resolution.
		int xdpi = wDrawing->logicalDpiX();
		int ydpi = wDrawing->logicalDpiY();
		gen->setResolution(qRound((xdpi+ydpi)/2.0));

		// Although the API documentation doesn't actually give any
		// specifics, QSvgGenerator::setSize() expects the page size
		// in pixels at the generator's resolution (which is in
		// dots per inch).  The size is output in the SVG file in
		// millimetres:
		//
		//   <svg width="726.017mm" height="513.644mm" xmlns= ...>
		//
		// This needs to happen after the setResolution() above.
		unsigned int printWidth;
		unsigned int printHeight;
		PaperUtil::getSize(mPagesize, mOrient, &printWidth, &printHeight, QPrinter::Millimeter);
		double res = gen->resolution()/25.4;
		gen->setSize(QSize(qRound(printWidth*res), qRound(printHeight*res)));

		device = gen;				// use this paint device
	}
	else
	{
		QPrinter::OutputFormat format = (ext=="pdf" ? QPrinter::PdfFormat : QPrinter::NativeFormat);
		formatName = (format==QPrinter::PdfFormat ? "PDF" : "PostScript");

		QPrinter *pr = new QPrinter(QPrinter::HighResolution);
		pr->setOutputFormat(format);		// configure printer for output
		pr->setOutputFileName(expfile);

		pr->setFullPage(true);			// must do these settings in
		pr->setPageOrientation(mOrient);	// this order, otherwise Qt 4.1.0
		pr->setPageSize(mPagesize);		// gets the output orientation
							// wrong - Qt bug 99066
		device = pr;				// use this paint device
	}

	wDrawing->printDiagram(device);			// do the print/export
	delete device;					// finished with device
							// let user know it worked
	QMessageBox::information(this,("Success - "+qApp->applicationName()),
				 QString("<qt>Exported <b>%1</b> to file<br><b>%2</b>")
				         .arg(formatName).arg(expfile),
				 QMessageBox::Ok,QMessageBox::NoButton,QMessageBox::NoButton);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  "Settings" menu operations						//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawView::optionsPageSize()
{
	QPrinter pr;
	pr.setPageSize(mPagesize);
	pr.setPageOrientation(mOrient);
	pr.setOutputFormat(QPrinter::PdfFormat);	// allow all known paper sizes

	QPageSetupDialog pd(&pr,this);
	pd.setWindowTitle("Page Size - "+qApp->applicationName());
	if (pd.exec()) setDrawingSize(pr.pageLayout().pageSize(), pr.pageLayout().orientation());
}


void DrawView::optionsPreferences()
{
	debugmsg(0) << funcinfo;
	// TODO: does the application need one?
}


void DrawView::drawingSizeChanged()
{
	unsigned int width,height;
	PaperUtil::getSize(mPagesize,mOrient,&width,&height);

	debugmsg(0) << funcinfo << "size=" << mPagesize << " w=" << width << " h=" << height << " o=" << mOrient;

	wDrawing->setDrawingSize(width,height);		// set widget size
	wFrame->adjustSize();				// adjust frame/scroll to suit
}


void DrawView::setDrawingSize(const QPageSize &size, QPageLayout::Orientation orient)
{
	mPagesize = size;
	mOrient = orient;
	drawingSizeChanged();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  "View" menu operations						//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawView::slotToggleClipping(bool on)
{
	wDrawing->paintOptions()->setFlags(PaintOptions::EnableClipping,on);
}


void DrawView::slotToggleAntiAlias(bool on)
{
	wDrawing->paintOptions()->setFlags(PaintOptions::AntiAlias,on);
	wDrawing->update();
}


void DrawView::slotToggleBoxes(bool on)
{
	wDrawing->paintOptions()->setFlags(PaintOptions::DisplayBoundingBoxes,on);
	wDrawing->update();
}


void DrawView::slotToggleSkeletons(bool on)
{
	wDrawing->paintOptions()->setFlags(PaintOptions::DisplaySkeletonObjects,on);
	wDrawing->update();
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Zoom selection							//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawView::slotZoomSelected(QAction *act)
{
//TODO: can use QScrollArea::ensureVisible (with extended margins)?
	int zoom = act->data().toInt();
	if (zoom==0) return;

	QWidget *v = wScroller->viewport();
	QPoint oldcent = wDrawing->mapFrom(v,QPoint(v->width(),v->height())/2);
	int xpc = (oldcent.x()*100)/wDrawing->width();	// old centre of window
	int ypc = (oldcent.y()*100)/wDrawing->height();	// as a proportion of it

	wDrawing->setZoom(zoom/100.0);			// tell widget its new scale
	drawingSizeChanged();				// resize it and myself

	QPoint newcent = wDrawing->mapFrom(v,QPoint(v->width(),v->height())/2);
	int wantx = (wDrawing->width()*xpc)/100;	// new centre of window
	int wanty = (wDrawing->height()*ypc)/100;	// want this point centered

	wScroller->horizontalScrollBar()->setValue(wScroller->horizontalScrollBar()->value()+(wantx-newcent.x()));
	wScroller->verticalScrollBar()->setValue(wScroller->verticalScrollBar()->value()+(wanty-newcent.y()));
}


void DrawView::changeZoom(int incr)
{
	const QList<QAction *> acts = mZoomacts->actions();
	int i = acts.indexOf(mZoomacts->checkedAction());
	if (i==-1) return;

	i += incr;
	if (i>=0 && i<acts.size()) acts.at(i)->activate(QAction::Trigger);
}


void DrawView::slotZoomIn()
{
	changeZoom(+1);
}


void DrawView::slotZoomOut()
{
	changeZoom(-1);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  "Help" menu operations						//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawView::aboutMe()
{
	QMessageBox::about(this,QString("About %1").arg(qApp->applicationName()),
			   QString("<qt>RiscOS Draw file viewer using Qt 5, version <b>%1</b><p>Home and download page:<br><b>%3</b><p>Report bugs or suggestions to:<br><b>%2</b><p>Released under the <b>GNU GPL</b>;<br>see http://www.gnu.org/licenses/gpl.html")
			   .arg(PACKAGE_VERSION).arg(PACKAGE_BUGREPORT).arg(PACKAGE_URL));
}
