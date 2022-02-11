//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Application					//
//  Edit:	11-Feb-22						//
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
#include <qtextstream.h>
#include <qfile.h>
#include <qregexp.h>
#include <qaction.h>
#include <qgridlayout.h>
#include <qscrollarea.h>
#include <qscrollbar.h>
#include <qfiledialog.h>
#include <qalgorithms.h>
#include <qprinter.h>
#include <qprintdialog.h>
#include <qpagesetupdialog.h>
#include <qsvggenerator.h>
#include <qstatusbar.h>

#include <klocalizedstring.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
#include <kselectaction.h>
#include <kmessagebox.h>

#include "drawwidget.h"
#include "diagram.h"
#include "paper.h"

#include "drawview.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWVIEW -- Viewer main window				//
//									//
//////////////////////////////////////////////////////////////////////////

DrawView::DrawView(const QString &file)
	: KXmlGuiWindow(NULL)
{
	debugmsg(0) << funcinfo << "file=" << file;

	mDiagram = NULL;				// no diagram yet

	setAcceptDrops(false);				// not accepted just anywhere
	setAttribute(Qt::WA_DeleteOnClose);		// close me and I'll go away

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
	connect(wDrawing, &DrawWidget::wheelZoom, this, &DrawView::slotChangeZoom);
	connect(wDrawing, &DrawWidget::dragScroll, this, &DrawView::slotDragScroll);
	lay->addWidget(wDrawing,0,0,Qt::AlignCenter);	// centered within border area

	setCentralWidget(wScroller);			// content of main window
	setupActions();					// menu and shortcut actions
	setupGUI(KXmlGuiWindow::Default);		// create and merge GUI
	setAutoSaveSettings();
							// we don't have a status bar
	QAction *act = actionCollection()->action("options_show_statusbar");
	if (act!=nullptr) act->setVisible(false);
	statusBar()->setVisible(false);
							// default page settings
	setDrawingSize(QPageSize(QPageSize::A4), QPageLayout::Landscape);

	if (!file.isNull())				// file to load specified
	{
		if (!loadFile(file)) deleteLater();	// load it, give up if error
	}

	show();						// show us as window
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

	KActionCollection *ac = actionCollection();
	KStandardAction::quit(this, SLOT(close()), ac);

	KStandardAction::open(this, SLOT(fileOpen()), ac);

	QAction *act = KStandardAction::save(this, SLOT(fileExport()), ac);
	act->setText(i18n("Export..."));
	act->setEnabled(false);

	act = ac->addAction("file_dump");
	act->setText(i18n("Dump"));
	act->setIcon(QIcon::fromTheme("view-list-text"));
	act->setEnabled(false);
	connect(act, SIGNAL(triggered()), SLOT(fileDump()));

	act = ac->addAction("settings_page_size");
	act->setText(i18n("Page Size..."));
	act->setIcon(QIcon::fromTheme("document-properties"));
	connect(act, SIGNAL(triggered()), SLOT(optionsPageSize()));

	act = KStandardAction::preferences(this, SLOT(optionsPreferences()), ac);
	act->setEnabled(false);

	act = KStandardAction::zoomIn(this, [this]() { slotChangeZoom(+1); }, ac);
	act = KStandardAction::zoomOut(this, [this]() { slotChangeZoom(-1); }, ac);
	act = KStandardAction::actualSize(this, [this]() { slotChangeZoom(0); }, ac);

	mZoomActs = new KSelectAction(QIcon::fromTheme("zoom"), i18n("Zoom"), this);
	for (int i = 0; i<nzooms; ++i)
	{
		const int z = zooms[i];
		act = new QAction(QString("%1%").arg(z), this);
		act->setCheckable(true);
		act->setData(z);
		connect(act, &QAction::triggered, this, [this, act]() { slotZoomSelected(act); });
		mZoomActs->addAction(act);
		if (z==100) mZoomActs->setCurrentAction(act);
	}
	ac->addAction("view_zoom", mZoomActs);

	act = ac->addAction("view_clipping");
	act->setText(i18n("Clipping"));
	act->setCheckable(true);
	act->setChecked(wDrawing->paintOptions()->flags() & PaintOptions::EnableClipping);
	connect(act,SIGNAL(triggered(bool)),this,SLOT(slotToggleClipping(bool)));

	act = ac->addAction("view_antialiasing");
	act->setText(i18n("Antialiasing"));
	act->setCheckable(true);
	act->setChecked(wDrawing->paintOptions()->flags() & PaintOptions::AntiAlias);
	connect(act,SIGNAL(triggered(bool)),this,SLOT(slotToggleAntiAlias(bool)));

	act = ac->addAction("view_bounding_boxes");
	act->setText(i18n("Bounding boxes"));
	act->setCheckable(true);
	act->setChecked(wDrawing->paintOptions()->flags() & PaintOptions::DisplayBoundingBoxes);
	connect(act,SIGNAL(triggered(bool)),this,SLOT(slotToggleBoxes(bool)));

	act = ac->addAction("view_skeleton_objects");
	act->setText(i18n("Skeleton objects"));
	act->setCheckable(true);
	act->setChecked(wDrawing->paintOptions()->flags() & PaintOptions::DisplaySkeletonObjects);
	connect(act,SIGNAL(triggered(bool)),this,SLOT(slotToggleSkeletons(bool)));

	mLibraryNameActs = new KSelectAction(QIcon::fromTheme("folder-images"), i18n("Library Object"), this);
	ac->addAction("view_library_name", mLibraryNameActs);
	mLibraryNameActs->setEnabled(false);		// until a library is loaded
}

//////////////////////////////////////////////////////////////////////////
//									//
//  "File" menu operations						//
//									//
//////////////////////////////////////////////////////////////////////////

static QString drawErrorDisplay(const QString &prefix, const DrawErrorList *errors)
{
	QString details;

	int count = 0;
	for (const DrawError *error : *errors->list())
	{
		if (!details.isEmpty()) details += "<br/>";
		details += error->message();

		++count;
		if (count>20)				// don't show too many
		{
			details += "<br/>";
			details += "(more suppressed)";
			break;
		}
	}

	QString msg = prefix;
	msg.replace("###", details);			// insert or blank details
	return (msg);
}


bool DrawView::loadFile(const QString &file)
{
	if (file.isEmpty()) return (false);
	QString infile = (file=="-") ? "/dev/stdin" : file;
	QFile qf(infile);				// stdin is not actually supported

	DrawDiagram *dg = new DrawDiagram(infile);
	if (!dg->isValid())
	{
		KMessageBox::sorry(this,
				   drawErrorDisplay(xi18nc("@info with placeholder at end",
							   "Error loading drawing file<nl/><filename>%1</filename><nl/><nl/><emphasis>###</emphasis>",
							   infile),
						    dg->drawErrorList()));
		delete dg;
		return (false);
	}

	if (mDiagram!=NULL) delete mDiagram;		// have a diagram already?
	mDiagram = dg;					// note the new one
	wDrawing->setDiagram(mDiagram);			// tell display about it

	// Clear the current library object name.  If there are library
	// objects then the object name combo box will be filled below
	// and its currentTextChanged() signal will set the name for
	// rendering.
	wDrawing->paintOptions()->setLibraryName(QByteArray());

	mLibraryNameActs->clear();
	const QList<QByteArray> libraryNames = wDrawing->libraryObjectNames();
	if (!libraryNames.isEmpty())
	{
		QAction *act = new QAction(i18nc("@action:inmenu", "(All)"), this);
		act->setCheckable(true);
		mLibraryNameActs->addAction(act);

		for (const QByteArray &libraryName : libraryNames)
		{
			act = new QAction(QString::fromLatin1(libraryName), this);
			act->setCheckable(true);
			act->setData(libraryName);
			mLibraryNameActs->addAction(act);
		}

		QList<QAction *> addedActs = mLibraryNameActs->actions();
		for (QAction *act1 : addedActs)
		{
			connect(act1, &QAction::triggered, this, [this, act1]()
			{
				wDrawing->paintOptions()->setLibraryName(act1->data().toByteArray());
				wDrawing->update();
			});
		}

		mLibraryNameActs->setCurrentItem(0);
		mLibraryNameActs->setEnabled(true);
	}
	else
	{
		mLibraryNameActs->setEnabled(false);
	}

	QPageSize size;
	QPageLayout::Orientation orient;		// guess a page size
	if (PaperUtil::guessSize(mDiagram->boundingBox(),&size,&orient)) setDrawingSize(size,orient);

	mDocname = infile.remove(QRegExp("^.*/"));	// save for possible printing
	setWindowTitle(mDocname);

	if (!mDiagram->drawErrorList()->list()->isEmpty())
	{

		KMessageBox::information(this,
				   drawErrorDisplay(xi18nc("@info with placeholder at end",
							   "Problem in drawing file<nl/><filename>%1</filename><nl/><nl/><emphasis>###</emphasis>",
							   infile),
						    dg->drawErrorList()));
	}

	QAction *act = actionCollection()->action("file_save");
	if (act!=nullptr) act->setEnabled(true);
	act = actionCollection()->action("file_dump");
	if (act!=nullptr) act->setEnabled(true);

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
	d.setOption(QFileDialog::DontConfirmOverwrite, false);
	d.setDirectory(expdir);				// use previous directory

	if (!d.exec()) return;				// run the dialogue

	expdir = d.directory();				// remember selected directory
	QString expfile = d.selectedFiles().value(0);
	if (expfile.isNull())
	{
		KMessageBox::error(this, i18n("No file name entered"));
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
	KMessageBox::information(this, xi18nc("@info", "Exported %1 to file<nl/><filename>%2</filename>",
					      formatName, expfile));
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
	pd.setWindowTitle("Page Size");
	if (!pd.exec()) return;

	setDrawingSize(pr.pageLayout().pageSize(), pr.pageLayout().orientation());
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
	wDrawing->update();
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


void DrawView::slotChangeZoom(int incr)
{
	const QList<QAction *> acts = mZoomActs->actions();
	int i = mZoomActs->currentItem();

	if (incr==0 || i==-1)
	{
		QAction *act = mZoomActs->action("100%");
		if (act!=nullptr)
		{
			mZoomActs->setCurrentAction(act);
			i = mZoomActs->currentItem();
		}
	}

	if (i==-1) return;
	i += incr;
	if (i>=0 && i<acts.size()) acts.at(i)->activate(QAction::Trigger);
}


void DrawView::slotDragScroll(int dx, int dy)
{
	wScroller->horizontalScrollBar()->setValue(wScroller->horizontalScrollBar()->value()-dx);
	wScroller->verticalScrollBar()->setValue(wScroller->verticalScrollBar()->value()-dy);
}
