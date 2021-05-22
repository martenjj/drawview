/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Configuration file location, reading and parsing.			//
//									//
//  The configuration files are all in a very simple format.  Blank	//
//  lines and comment lines (starting with '#') are ignored; all	//
//  other lines are split into whitespace-separated fields with double	//
//  quote ('"') enclosing fields containing whitespace.  A list of	//
//  fields is returned to the caller in string form; any further	//
//  processing (e.g. conversion to numbers) is done there.  Syntax	//
//  error reporting is also performed by the caller, although the	//
//  current line number is tracked here.				//
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

#include <string.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif

#include <qapplication.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtextstream.h>
#include <qregexp.h>
#include <qmessagebox.h>

#include "files.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class FILEREADER --	Locate, read and parse a configuration file	//
//									//
//  The file is searched for in the following locations, in order:	//
//									//
//    name exactly as given						//
//    BINDIR/../share/APPNAME/		(app-specific data directory)	//
//    BINDIR/../share/			(common data directory)		//
//    ../share/				(uninstalled, for testing)	//
//    current directory			(last resort)			//
//									//
//////////////////////////////////////////////////////////////////////////

FileReader::FileReader(const QString &basename)
{
	name = basename;				// save for report later
	path.clear();					// file not found yet

	ok = tryFile() ||
	     tryFile(qApp->applicationDirPath()+"/../share/"+PACKAGE) ||
	     tryFile(qApp->applicationDirPath()+"/../share") ||
	     tryFile("../share") ||
	     tryFile(".");
	if (!ok)
	{
		warnmsg() << funcinfo << "cannot find data file '" << name << "'";
		return;
	}

	ok = f.open(QIODevice::ReadOnly);
	if (!ok)
	{
		warnmsg() << funcinfo << "cannot read '" << path << "', " << strerror(errno);
		return;
	}

	str.setDevice(&f);				// associate with stream
	str.reset();
	lineno = 0;
}


FileReader::~FileReader()
{
	f.close();					// finished with file
}


bool FileReader::tryFile(const QString &where)
{
	f.setFileName(where+(!where.isNull() ? "/" : "")+name);
	if (!f.exists()) return (false);		// make path, see if file exists

	QFileInfo fi(f);				// check readable regular file
	if (!fi.isFile() || !fi.isReadable()) return (false);

	path = fi.absoluteFilePath();			// note full path to file
	debugmsg(0) << funcinfo << "using " << path;
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Read a line from the file, as an unparsed string			//
//									//
//////////////////////////////////////////////////////////////////////////

const QString FileReader::getLine()
{
	for (;;)					// until got a line, anyway
	{						// check for end of file
		if (str.atEnd()) return (QString());
		QString line = str.readLine();
		++lineno;				// read line and count it

		if (line.length()==0) continue;		// ignore blank lines
		if (line.startsWith('#')) continue;	// ignore comment lines
		return (line);				// got a valid line
	}
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Read a line from the file, as a string list of fields		//
//									//
//////////////////////////////////////////////////////////////////////////

const QStringList FileReader::getParsedLine()
{
	QStringList fields;				// list of fields read

	QString line = getLine();			// get a line from file
	if (line.isNull()) return (fields);		// end of file, empty result

	const QRegExp ws("^\\s+");			// match whitespace at start
	QString field;					// current field being built
	bool quote = false;				// within a quoted string

	while (!line.isEmpty())				// until end of line
	{
		if (!quote && line.indexOf(ws)==0)	// white space, end of field
		{					// save the current field
			if (!field.isNull()) fields.append(field);
			line = line.replace(ws,"");	// trim whitespace from start
			field.clear();			// clear to start next
			continue;
		}

		if (line.startsWith('"'))		// start/end of qouted string
		{
			quote = !quote;			// change quote state
			line = line.mid(1);		// drop the quote character
			continue;
		}

		field += line.left(1);			// take character from line
		line = line.mid(1);			// strip it from remaining
	}

	if (!field.isNull()) fields.append(field);	// save the last field
	return (fields);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Report an error							//
//									//
//////////////////////////////////////////////////////////////////////////

void FileReader::reportError(const QString &desc,QWidget *parent) const
{
	QMessageBox::critical(parent,("Error - "+qApp->applicationName()),
			     QString("Cannot locate %2 data file '%1'").arg(name).arg(desc),
			     QMessageBox::Ignore,QMessageBox::NoButton);
}
