/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	15-Jan-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Configuration file location and reading.				//
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

#ifndef FILES_H
#define FILES_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QStringList;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class FILEREADER --	Locate, read and parse a configuration file	//
//									//
//////////////////////////////////////////////////////////////////////////

class FileReader
{
public:
	FileReader(const QString &basename);
	~FileReader();

	bool isValid() const				{ return (ok); }
	int lineNumber() const				{ return (lineno); }

	const QString getLine();
	const QStringList getParsedLine();

	void reportError(const QString &desc,QWidget *parent = NULL) const;

protected:
	bool tryFile(const QString &basename = QString::null);

private:
	bool ok;
	QString name;
	QString path;
	QFile f;
	QTextStream str;
	int lineno;
};

#endif							// FILES_H
