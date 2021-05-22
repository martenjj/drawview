/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  File reader, expected size tracking and error checking.		//
// 								 	//
//  Although the purpose of the DrawView application was simply to	//
//  read and display regular files, there is the possibility that	//
//  in the future there may be a need to read other data sources	//
//  (e.g. a memory buffer).  To accommodate this, all Draw file data	//
//  is read via a subclass of a 'DrawReader' which encapsulates all	//
//  of the operations and error checking needed.			//
//									//
//  Having said that, in the current implementation only files are	//
//  read.  Reading the output of the decompression filter process via	//
//  a pipe is not reliable (see the reference below for probably why),	//
//  and so decompressed data is read into a memory buffer and		//
//  accessed via a 'QBuffer' in the same way as any other I/O device.	//
//  So at the moment the only data source implemented is the		//
//  'DrawFileReader'.							//
//									//
//  If this eventually turns out to be the only sort of	reader needed,	//
//  the two classes could be recombined into one.			//
//									//
//  Error checking on each I/O operation is implemented here.  In	//
//  addition, there is the possibility that even without physical I/O	//
//  errors the file may be too short and end unexpectedly part of	//
//  the way through an object.  To save all of the object builders	//
//  from having to check each and every read operation for all		//
//  possible problems, the expected size of an object being read	//
//  (not known, of course, until the object tag and size have been	//
//  read) is tracked here.						//
//									//
//  To handle I/O errors (detected here) and object format/data errors	//
//  (detected by the object readers) consistently, an 'DrawErrorList'	//
//  error list is kept here - in the current DrawView implentation	//
//  this is allocated and owned by the caller ('DrawDiagram').  All	//
//  errors detected during reading are recorded in this list via our	//
//  'AddError'.								//
//									//
//  See http://lists.kde.org/?l=kde-core-devel&m=113316044112484&w=2	//
//  for the reason why error and EOF detection on a 'QDataStream' is	//
//  not particularly reliable.						//
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

#define CHECK_QIOERR

#undef DEBUG_READER

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qstring.h>
#include <qiodevice.h>
#include <qdatastream.h>
#include <qbytearray.h>
#include <qstack.h>

#include "error.h"

#include "reader.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Error tracking							//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawReader::addError(const QString &msg,Draw::error lvl)
{
	debugmsg(0) << funcinfo << "level " << lvl << " '" << msg << "'";

	if (errlist==NULL)				// no error list set up
	{
		if (ourerrs==NULL) ourerrs = new DrawErrorList;
		errlist = ourerrs;			// allocate our own copy
		debugmsg(0) << funcinfo << "allocated internal list";
	}

	errlist->add(msg,lvl,objstart);			// add error to the list
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Internally detected I/O error, or end of file.			//
//									//
//  There are three cases where an "end of file" may be encountered:	//
//									//
//    Finished	An EOF detected on the stream, but in a place where	//
//		it is expected - at the start of a new top-level	//
//		object.  This means that the end of the Draw file has	//
//		been reached; it is not an error.			//
//									//
//    Eof	An EOF detected on the stream elsewhere, either part	//
//		of the way through an object or at an object boundary	//
//		which is not top-level.					//
//									//
//    Short	Intending to read more data than the expected size	//
//		would indicate - e.g. an object tag that is too small	//
//		for the minimum size of an object header.  This		//
//		indicates an error in the structure of the Draw file	//
//		rather than an I/O error, but it is more easily		//
//		detected here.						//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawReader::setError(DrawReader::status st,int num,QString msg)
{
	debugmsg(0) << funcinfo << "setting status " << st << " = " << num << "+'" << msg << "'";

	errstat = st;
	errloc = curpos;

	QString errstr;
	switch (st)
	{
case Ok:	return (true);
case Finished:	return (false);
case Eof:	errstr = "Unexpected end of file";				break;
case Invalid:	errstr = "Invalid reader state";				break;
case Ioerr:	errstr = QString("QIO status %1, %2").arg(num).arg(msg);	break;
case Short:	errstr = QString("Expected %1 unavailable").arg(num);		break;
default:	errstr = QString("Unknown error %1").arg(st);			break;
	}

	if (errloc>0) errstr += QString(", at %1(0x%2)").arg(errloc).arg(errloc,0,16);
	addError(errstr);
	return (false);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Recursion, for reading subobjects of containing objects.		//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawReader::save()
{
#ifdef DEBUG_READER
	debugmsg(0) << Qt::endl << funcinfo << "stacking cp=0x" << Qt::hex << curpos << Qt::dec << " exp=" << expsize;
#endif
	savestack.push(curpos);				// save current position
	savestack.push(expsize);			// save expected size
	expsize = 0;					// needs to be set again
}

void DrawReader::restore()
{
	drawuint oxs = savestack.pop();			// restore expected size
	drawuint ocp = savestack.pop();			// restore old position
	expsize = oxs-(curpos-ocp);			// calculate new expected size
#ifdef DEBUG_READER
	debugmsg(0) << funcinfo << "popped cp=0x" << Qt::hex << ocp << Qt::dec << " xs=" << oxs << "; new cp=0x" << Qt::hex << curpos << Qt::dec << " xs=" << expsize;
#endif
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWREADER -- Reader that reads from any QIODevice, which	//
//  therefore can read from a file or any other data source such as a	//
//  memory buffer.							//
//									//
//////////////////////////////////////////////////////////////////////////

DrawReader::DrawReader(QIODevice *dev,DrawErrorList *errl)
{
	debugmsg(0) << funcinfo;

	errstat = DrawReader::Invalid;			// no status set yet
	curpos = expsize = objstart = 0;		// clear position and counters

	ourerrs = NULL;					// no error list allocated yet
	errlist = errl;					// save caller's list if any

	str = new QDataStream(dev);
	str->setByteOrder(QDataStream::LittleEndian);	// always this in Draw files
	if (str->status()==QDataStream::Ok) setError(DrawReader::Ok);
}


DrawReader::~DrawReader()
{
	debugmsg(0) << funcinfo;
	str->device()->close();
	delete str;
	if (ourerrs!=NULL) delete ourerrs;		// we allocated it, so destroy
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Checking macros 							//
//									//
//////////////////////////////////////////////////////////////////////////

#define checkStreamEof() do { if (str->atEnd()) return (setError(DrawReader::Eof)); } while (0)
#define checkStreamShort(n) do { if (expsize<n) return (setError(DrawReader::Short,n)); } while (0)

#ifdef CHECK_QIOERR
#define checkStreamStatus() do { if (str->status()!=QDataStream::Ok) return (setError(DrawReader::Ioerr,str->status(),str->device()->errorString())); } while (0)
#else
#define checkStreamStatus() do { } while (0)
#endif

//////////////////////////////////////////////////////////////////////////
//									//
//  Read a 4-byte word, possibly with an EOF expected			//
//									//
//  Unaligned reads are allowed - this was to support the possibility	//
//  of reading and decoding compressed files on the fly.  As it turned	//
//  out, doing this was impossibly complicated and the RiscOS decoding	//
//  code was used instead (run as a filter in a separate process).	//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawReader::getWord(drawword *wp, bool expecteof)
{
#ifdef DEBUG_READER
	debugmsg(0) << funcinfo << "expsize=" << expsize << " curpos=0x" << Qt::hex << curpos << Qt::dec;
#endif
	if (!savestack.isEmpty()) expecteof = false;	// only at top level
	if (str->atEnd())				// end of file, but is it to
	{						// be expected here?
		if (expecteof) return (setError(DrawReader::Finished));
		else return (setError(DrawReader::Eof));
	}
	checkStreamShort(4);

	drawword w;
	if ((curpos % sizeof(drawword))==0)		// currently word aligned
	{
		*str >> w;
#ifdef DEBUG_READER
		debugmsg(0) << "  aligned read got " << w << "(0x" << Qt::hex << w << Qt::dec << ")";
#endif
	}
	else
	{
		w = 0;
		for (unsigned int i = 0; i<8*sizeof(drawword); i += 8)
		{
			quint8 c; *str >> c;
			w |= c<<i;
		}
#ifdef DEBUG_READER
		debugmsg(0) << "  unaligned read got " << w << "(0x" << Qt::hex << w << Qt::dec << ")";
#endif
	}

	if (str->status()==QDataStream::ReadPastEnd) return (setError(DrawReader::Eof));
	checkStreamStatus();

	if (wp!=NULL) *wp = w;				// store result read
	expsize -= sizeof(drawword);			// count down expected size
	curpos += sizeof(drawword);			// count up current position
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Read a counted sequence of bytes					//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawReader::getByte(drawbyte *cp,drawuint len)
{
#ifdef DEBUG_READER
	debugmsg(0) << funcinfo << "expsize=" << expsize << " curpos=" << Qt::hex << curpos << Qt::dec << " len=" << len;
#endif
	checkStreamEof();
	checkStreamShort(len);

	while (len>0)
	{
		quint8 c; *str >> c;			// no alignment requirements
		checkStreamStatus();

		if (cp!=NULL) *cp++ = c;		// save in output buffer
		--expsize;
		++curpos;
		--len;
	}

	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Read a string up to a terminator					//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawReader::getString(QByteArray *b,unsigned char terminator)
{
#ifdef DEBUG_READER
	debugmsg(0) << funcinfo << "expsize=" << expsize << " curpos=" << Qt::hex << curpos << Qt::dec;
#endif
	b->clear();
	quint8 c;
	for (;;)
	{
		checkStreamEof();
		checkStreamShort(1);

		*str >> c;				// no alignment requirements
		checkStreamStatus();

		++curpos;
		--expsize;

		if (c!='\0') b->append(c);		// append to output string
		if (c<=terminator) break;
	}

	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Discard up to the next word boundary				//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawReader::discardWordAlign()
{
#ifdef DEBUG_READER
	debugmsg(0) << funcinfo << "expsize=" << expsize << " curpos=" << Qt::hex << curpos << Qt::dec;
#endif
	qint8 c;
	while ((curpos % 4)!=0)
	{
		checkStreamEof();
		checkStreamShort(1);

		*str >> c;				// no alignment requirements
		checkStreamStatus();

		++curpos;
		--expsize;
	}

	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Discard all of the current expected data				//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawReader::discardRest()
{
#ifdef DEBUG_READER
	debugmsg(0) << funcinfo << "expsize=" << expsize << " curpos=" << Qt::hex << curpos << Qt::dec;
#endif
	qint8 c;
	while (expsize>0)
	{
		checkStreamEof();

		*str >> c;				// no alignment requirements
		checkStreamStatus();

		++curpos;
		--expsize;
	}

	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Compatibility overloads - keep all the pointer ugliness in one	//
//  place here.								//
//									//
//////////////////////////////////////////////////////////////////////////

bool DrawReader::getByte(char *cp, drawuint len)
{
	return (getByte(static_cast<drawbyte *>(static_cast<void *>(cp)), len));
}

bool DrawReader::getWord(void *vp, bool expecteof)
{
	return (getWord(static_cast<drawword *>(vp), expecteof));
}

bool DrawReader::getWord(std::nullptr_t np, bool expecteof)
{
	return (getWord(static_cast<drawword *>(np), expecteof));
}

