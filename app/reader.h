/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	27-May-21						//
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
//  Class DRAWREADER -- Reader that operates on any QIODevice.		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawReader
{
public:
	enum status { Ok, Finished, Eof, Ioerr, Short, Invalid };

	explicit DrawReader(QIODevice *dev, DrawErrorList *errl = nullptr);
	~DrawReader();

	void save();
	void restore();

	void setExpectedSize(drawuint bytes)		{ expsize = bytes; }
	drawuint sizeRemaining() const			{ return (expsize); }
	drawuint currentPosition() const		{ return (curpos); }
	void noteCurrentObject()			{ objstart = curpos; }
	drawuint currentObject() const			{ return (objstart); }

	DrawReader::status getStatus() const		{ return (errstat); }
	DrawErrorList *getErrorList() const		{ return (errlist); }

	bool getWord(drawword *wp, bool expecteof = false);
	bool getByte(drawbyte *bp, drawuint len = 1);
	bool getString(QByteArray *bp, unsigned char terminator = '\0');
	bool discardWordAlign();
	bool discardRest();

	// Compatibility overload to be able to pass the result of
	// QByteArray::data(), which returns a 'char *'
	bool getByte(char *cp, drawuint len = 1);

	// Compatibility overload to be able to pass a pointer to
	// an arbitrary type, which must be word-aligned.
	bool getWord(void *vp, bool expecteof = false);

	// Compatibility overload to be able to pass a null pointer.
	bool getWord(std::nullptr_t np, bool expecteof = false);

	void addError(const QString &msg,Draw::error lvl = Draw::errorFATAL);
	void endOfFile();

protected:
	DrawReader(DrawErrorList *errl = NULL);

	bool setError(DrawReader::status st, int num = 0, QString msg = "");

	drawuint curpos;
	drawuint expsize;
	QStack<drawuint> savestack;

private:
	QDataStream *str;
	DrawReader::status errstat;
	QString errmessage;
	drawuint errloc;
	DrawErrorList *ourerrs;
	DrawErrorList *errlist;

	drawuint objstart;
};

#endif							// READER_H
