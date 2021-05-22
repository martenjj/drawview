/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Draw diagram:  a collection of objects, with associated font	//
//  mapping and possible loading/verify errors. 			//
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

#include <errno.h>
#include <string.h>

#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qprocess.h>
#include <qdir.h>
#include <qapplication.h>
#include <qbytearray.h>
#include <qbuffer.h>
#include <qrect.h>
#include <qalgorithms.h>

#include "object.h"
#include "reader.h"
#include "error.h"
#include "fonts.h"
#include "coord.h"
#include "paintoptions.h"

#include "diagram.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Access to external decompression filter				//
//									//
//  The filter is searched for in the following locations, relative	//
//  to the directory containing the executable ("HERE"):		//
//									//
//    HERE/../libexec			(final installed location)	//
//    HERE/../lib			(old install compatibility)	//
//    HERE/../filter			(in-place during development)	//
//    current directory			(last resort)			//
//									//
//  If the filter cannot be found, then it will not be possible to	//
//  load any compressed files.						//
//									//
//////////////////////////////////////////////////////////////////////////

static const char *filtername = "drawfilter";
static QString decompfilter = "";


static void locateDecompFilter()
{
	if (!decompfilter.isNull()) return;		// have got it already

	QStringList paths;
	QString here = qApp->applicationDirPath();
	paths.append(here+"/../libexec");
	paths.append(here+"/../lib");
	paths.append(here+"/../filter");
	paths.append(QDir::currentPath());

	for (QStringList::const_iterator it = paths.begin(); it!=paths.end(); ++it)
	{
		QFile exe((*it)+"/"+filtername);
		if (exe.exists())
		{
			QFileInfo exinf(exe);
			if (exinf.isExecutable())
			{
				decompfilter = exinf.absoluteFilePath();
				debugmsg(0) << funcinfo << "found filter " << decompfilter;
				return;
			}
		}
	}

	warnmsg() << funcinfo << "no filter found!" << decompfilter;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWDIAGRAM -- Draw diagram handling, as a linear collection	//
//  of objects.								//
//									//
//  The diagram created may be blank (if 'file' is null), or loaded	//
//  from a file.  If there is a file, the first word of it is read to	//
//  determine whether it is a plain Draw file or a compressed one.	//
//  If it is not compressed, reading from it continues with the rest	//
//  of the file.							//
//									//
//  If the file is compressed, the decompression filter (located	//
//  above) is started as a subprocess to read from the file.  For	//
//  this reason, the input file must be a plain file (i.e. not a	//
//  device - unfortunately this excludes standard input).  The output	//
//  of the subprocess is read in one big chunk and then reading		//
//  starts again with that from a buffer.  Trying to do this the	//
//  obvious way - reading the output of the subprocess directly -	//
//  appears to return an end-of-file at a too early (and random)	//
//  place, I suspect that this would only work properly if the		//
//  application is running the Qt event loop while reading.  Even	//
//  decompressed, Draw files are fairly small - at least, by today's	//
//  standards, anyway - so the buffer space should not be a serious	//
//  problem.  Since the subprocess completes here, we get better	//
//  diagnostics from it too.						//
//									//
//////////////////////////////////////////////////////////////////////////

DrawDiagram::DrawDiagram(const QString &file)
{
	debugmsg(0) << funcinfo << "file=" << file;

	fontmap = NULL;					// not allocated yet

	if (!file.isNull())				// load from a file
	{
		QFile ff(file);
		QFileInfo fi(ff);

		if (!ff.exists())
		{
			errors.add(QString("File '%1' not found").arg(fi.absoluteFilePath()));
			return;
		}
		if (!fi.isReadable() || fi.isDir())
		{
			errors.add(QString("Not a readable file '%1'").arg(fi.absoluteFilePath()));
			return;
		}

		if (!ff.open(QIODevice::ReadOnly))
		{
			errors.add(QString("File '%1' %2").arg(fi.absoluteFilePath()).arg(strerror(errno)));
			return;
		}

		QByteArray data;
		QBuffer *buf = NULL;

		DrawReader *rd = new DrawReader(&ff, &errors);
		if (rd->getStatus()!=DrawReader::Ok)	// create reader to read from file
		{
			errors.add("Reader initialisation failed");
			ff.close();
			return;
		}

		QByteArray title(sizeof(drawword),'\0');
		rd->setExpectedSize(title.size());	// read first word of file
		if (!rd->getByte(title.data(), title.size()))
		{					// what, a problem already?
			errors.add("Empty file");
			ff.close();
			return;
		}

		debugmsg(0) << funcinfo << "got title " << title;
		if (title=="DrwC")			// see if compressed file
		{
			delete rd;			// if so won't use it here
			ff.close();			// or this file either

			if (!fi.isFile())		// must be a plain file now
			{
				errors.add(QString("Not a plain file '%1'").arg(fi.absoluteFilePath()));
				return;
			}

			locateDecompFilter();		// will run filter instead
			if (decompfilter.isNull())
			{
				errors.add("Cannot locate decompression filter");
				return;
			}

			QProcess proc;
			proc.start(decompfilter,QStringList(file));
			if (!proc.waitForStarted())	// start filter subprocess
			{
				//debugmsg(0) << funcinfo << "failed to start " << proc.error() << " exit " << proc.exitCode();
				errors.add(QString("Decompression process failed to start, error %1").arg(proc.error()));
				return;
			}
			debugmsg(0) << "started process pid " << proc.processId();

			if (!proc.waitForFinished())	// let it run to finish
			{
				//debugmsg(0) << funcinfo << "failed to finish " << proc.error() << " exit " << proc.exitCode();
				errors.add(QString("Decompression process failed to finish, error %1").arg(proc.error()));
				return;
			}

			const int status = proc.exitCode();
			const QString se = proc.readAllStandardError();
			//debugmsg(0) << "  finished with exit " << status;
			//if (!se.isEmpty()) debugmsg(0) << "  standard error: '" << se << "'";

			if (status!=0)
			{
				errors.add(QString("Decompression failed, status %1").arg(status));
				if (!se.isEmpty()) errors.add(se);
				return;
			}
			if (!se.isEmpty()) errors.add(se,Draw::errorWARNING);

			title.clear();			// need to read this again
			data = proc.readAllStandardOutput();
							// grab all the decompressed data
			//debugmsg(0) << "read data " << data.size();
			proc.close();			// finished with the process

			buf = new QBuffer(&data);	// create device reading buffer
			buf->open(QIODevice::ReadOnly);	// create reader using buffer
			rd = new DrawReader(buf, &errors);
		}
							// read rest of file
		if (rd->getStatus()==DrawReader::Ok) readDrawFile(*rd,title);
		else errors.add("Reader initialisation failed");

		delete rd;				// finished with reader
		if (buf!=NULL) delete buf;		// finished with buffer
		data.clear();				// finished with the data
	}

	debugmsg(0) << funcinfo << "done valid=" << errors.level() << Qt::endl;
}


DrawDiagram::~DrawDiagram()
{
	qDeleteAll(objects);
	debugmsg(0) << funcinfo << "done";
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Reading from file							//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawDiagram::readDrawFile(DrawReader &rdr,QByteArray title)
{							// from 'struct draw_fileheader'
	drawuint majorstamp,minorstamp;
	drawbyte progident[12+1];

	rdr.setExpectedSize(40);			// size of file header

	debugmsg(0) << funcinfo << "\nReading Draw file..................................................\n";

	if (title.isEmpty())				// haven't got this already
	{
		title.resize(4);			// read 'char title[4]'
		if (!rdr.getByte(title.data(), 4)) return (false);
	}

	debugmsg(0) << "  title=" << title;
	if (title=="Draw")				// only possible option now
	{
		debugmsg(0) << "  little endian, uncompressed";
	}
	else if (title=="warD")				// stream should sort this out
	{
		debugmsg(0) << "  big endian, uncompressed";
		rdr.addError("Wrong endian");
		return (false);
	}
	else if (title=="DrwC")				// shouldn't get these by now
	{
		debugmsg(0) << "  little endian, compressed";
		rdr.addError("Compressed Draw files not supported");
		return (false);
	}
	else if (title=="CwrD")				// really going wrong here
	{
		debugmsg(0) << "  big endian, compressed";
		rdr.addError("Wrong endian");
		rdr.addError("Compressed Draw files not supported");
		return (false);
	}
	else						// title not recognised at all
	{
		rdr.addError("Not a Draw file");
		return (false);
	}
							// read 'int majorstamp,minorstamp'
	if (!rdr.getWord(&majorstamp) || !rdr.getWord(&minorstamp)) return (false);
	debugmsg(0) << "  header major " << majorstamp << " minor " << minorstamp;
	if (majorstamp>Draw::fileVERSION)
	{
		rdr.addError(QString("File version %1 not supported").arg(majorstamp));
		return (false);
	}

	if (!rdr.getByte(progident,12)) return (false);	// read 'char progident[12]'
	progident[12] = '\0';
	debugmsg(0) << "  creator '" << progident << "'";

	if (!bbox.read(rdr)) return (false);		// read 'draw_box bbox'
	debugmsg(0) << "  bbox=" << bbox;

	for (;;)					// until end of file
	{						// create and read object
		DrawObject *obj = DrawObjectManager::create(rdr,this);
		if (obj==NULL)				// creation failed, look at why
		{					// normal end of file
			if (rdr.getStatus()==DrawReader::Finished) break;
			else return (false);		// some other problem
		}

		objects.append(obj);			// add object to diagram
	}

	debugmsg(0) << funcinfo << "read " << objects.size() << " objects";
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Error checking and reporting					//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawDiagram::isValid() const
{
	return (errors.level()<Draw::errorFATAL);
}


QString DrawDiagram::drawError() const
{
	QString msg = "";

	int count = 0;
	for (QList<const DrawError *>::const_iterator it = errors.list()->begin();
	     it!=errors.list()->end(); ++it)
	{
		msg += "\n";
		msg += (*it)->message();

		++count;
		if (count>20)				// don't show too many
		{
			msg += "\n(more suppressed)";
			break;
		}
	}
	return (msg);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Draw onto a painter							//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawDiagram::draw(QPainter &p,const PaintOptions *opts) const
{
	for (QList<DrawObject *>::const_iterator it = objects.begin();
	     it!=objects.end(); ++it)			// draw each object in order
	{
		(*it)->draw(p,this,opts);
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Dump to a text stream						//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawDiagram::dump(QTextStream &str) const
{
	const QString line(78,'-');

	str << Qt::endl << line << Qt::endl << Qt::endl << Qt::uppercasedigits;
	str << "Diagram: objects " << objects.size() << " bbox " << bbox;
	str << Qt::endl;

	QString in2 = "|  ";
	for (QList<DrawObject *>::const_iterator it = objects.begin();
	     it!=objects.end(); ++it)
	{
		if ((*it)==objects.last()) in2 = "   ";
		(*it)->dump(str,"",in2);
	}

	str << Qt::endl << line << Qt::endl << Qt::endl;
}
