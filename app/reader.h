/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	24-Apr-06						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  File reader, expected size tracking and error checking.		//
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

#ifndef READER_H
#define READER_H

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include <qstack.h>

//////////////////////////////////////////////////////////////////////////
//									//
//  Referenced classes and types					//
//									//
//////////////////////////////////////////////////////////////////////////

class QIODevice;
class QDataStream;
class QByteArray;

class DrawError;
class DrawErrorList;

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWREADER -- Abstract base class for reading.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawReader
{
public:
	enum status { Ok, Finished, Eof, Ioerr, Short, Invalid };

	virtual ~DrawReader();

	void save();
	void restore();

	void setExpectedSize(drawuint bytes)		{ expsize = bytes; }
	drawuint sizeRemaining() const			{ return (expsize); }
	drawuint currentPosition() const		{ return (curpos); }
	void noteCurrentObject()			{ objstart = curpos; }
	drawuint currentObject() const			{ return (objstart); }

	DrawReader::status getStatus() const		{ return (errstat); }
	DrawErrorList *getErrorList() const		{ return (errlist); }

	virtual bool getWord(drawword *wp,bool expecteof = false) = 0;
	virtual bool getByte(drawbyte *cp,drawuint len = 1) = 0;
	virtual bool getString(QByteArray *bp,unsigned char terminator = '\0') = 0;
	virtual bool discardWordAlign() = 0;
	virtual bool discardRest() = 0;

	void addError(const QString &msg,Draw::error lvl = Draw::errorFATAL);

protected:
	DrawReader(DrawErrorList *errl = NULL);

	bool DrawReader::setError(DrawReader::status st,int num = 0,QString msg = QString::null);

	drawuint curpos;
	drawuint expsize;
	QStack<drawuint> savestack;

private:
	DrawReader::status errstat;
	QString errmessage;
	drawuint errloc;
	DrawErrorList *ourerrs;
	DrawErrorList *errlist;

	drawuint objstart;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWFILEREADER -- Reader that reads from a file.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawFileReader : public DrawReader
{
public:
	DrawFileReader(QIODevice *dev,DrawErrorList *errl = NULL);
	~DrawFileReader();

	bool getWord(drawword *wp,bool expecteof = false);
	bool getByte(drawbyte *cp,drawuint len = 1);
	bool getString(QByteArray *bp,unsigned char terminator = '\0');
	bool discardWordAlign();
	bool discardRest();

private:
	QDataStream *str;
};

#endif							// READER_H
