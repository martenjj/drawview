/////////////////////////////////////////////////// -*- mode:c++; -*- ////
//									//
//  Project:	DrawView - Objects					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Text area and column objects.					//
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

#include <qtextstream.h>
#include <qlist.h>
#include <qalgorithms.h>
#include <qtextdocument.h>
#include <qtextcursor.h>
#include <qtextlayout.h>
#include <qtextobject.h>
#include <qabstracttextdocumentlayout.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qpainter.h>
#include <qbrush.h>
#include <qvariant.h>

#include "reader.h"
#include "coord.h"
#include "paintoptions.h"
#include "fonts.h"

#include "textarea.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Class TEXTAREADOCUMENT -- Formatted document generated from the	//
//  input text.								//
//									//
//  Persistent properties (line spacing, paragraph spacing, left and	//
//  right margins) are attached to the text as user properties, so	//
//  that they can be read out while drawing.				//
//									//
//////////////////////////////////////////////////////////////////////////

class TextAreaDocument : public QTextDocument
{
public:
	TextAreaDocument(DrawReader &rd,const QByteArray &txt,QColor col);
	virtual ~TextAreaDocument();

	enum						// user text properties
	{
		LeadingProperty = QTextFormat::UserProperty+0,
		ParagraphProperty = QTextFormat::UserProperty+1,
		LeftMarginProperty = QTextFormat::UserProperty+2,
		RightMarginProperty = QTextFormat::UserProperty+3
	};

protected:
	void setError(const QString &msg,bool fatal = false) const;
	void skipSequence(bool nlonly = false);

	bool sequenceA();
	bool sequenceBC(char what);
	bool sequenceD();
	bool sequenceF();
	bool sequenceLP(char what);
	bool sequenceM();
	bool sequenceU();
	bool sequenceV();
	bool sequenceNumber(char first);

private:
	DrawReader *rdr;

	QString text;
	int ptr;
	QTextCursor *curs;
	FontReferenceMap fontmap;

	QTextBlockFormat blkfmt;
	QTextCharFormat charfmt;
	int vmove;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Text area parsing							//
//									//
//////////////////////////////////////////////////////////////////////////

TextAreaDocument::TextAreaDocument(DrawReader &rd,const QByteArray &txt,QColor col)
	: QTextDocument()
{
	debugmsg(0) << funcinfo;

	rdr = &rd;
	text = txt;
	curs = new QTextCursor(this);			// text cursor within document

	// Basic document splitting:
	//
	// \n\n ->	paragraph break = end block, null block, new block
	// \\n  -> line break = end block, new block
	// \n   -> equivalent to space

	if (text.right(1)[0]!='\n') text += '\n';	// must end with newline
	ptr = 0;					// parsing position in text

	// Convert any control sequence terminated by newline into the same
	// sequence terminated with "/"
	//	text.replace(QRegExp("(\\\\\\D[^/\\\\\\n]+)/*\\n+"),"\\1/");
	//
	// Ensure that "\A" (alignment) code is always terminated with "/" too
	//	text.replace(QRegExp("(\\\\A.)([^/])"),"\\1/\\2");
	//
	// Replace any other unescaped single newline with a space
	//	text.replace(QRegExp("([^\\\\\\n])\\n([^\\n])"),"\\1 \\2");

	// Tab is equivalent to space
	text.replace('\t',' ');

	// Any other control character is ignored
	text.replace(QRegExp("[\\0000-\\0011\\0013-\\0037\\0177-\\0237]"),"");

	vmove = 0;					// cumulative vertical move

	blkfmt.setAlignment(Qt::AlignLeft);		// set up block format
	curs->setBlockFormat(blkfmt);

	charfmt.setForeground(QBrush(col));		// set up character format
	charfmt.setProperty(TextAreaDocument::LeadingProperty,QVariant(0));
	charfmt.setProperty(TextAreaDocument::ParagraphProperty,QVariant(0));
	curs->setCharFormat(charfmt);

	QRegExp r4("^\\\\!\\s*(\\d+)[/\\n]");		// parse "!" sequence
	if (r4.indexIn(text)<0 || r4.cap(1).toInt()!=1)
	{
		setError("Bad text area header");
		return;
	}
	ptr = r4.cap(0).length();			// skip past first code

	QRegExp r0("^(.*)([\\\\\\n])");			// search up to code or newline
	r0.setMinimal(true);
	QRegExp r1("^\\n+");				// parse sequence of newlines

	for (;;)					// until end of text
	{
		int i = r0.indexIn(text,ptr,QRegExp::CaretAtOffset);
		if (i==-1) break;			// end of text

		QString lead = r0.cap(1);		// leading text before match
		ptr += lead.length();			// step up to '\' or NL

		char tm = text.at(ptr++).toLatin1();	// backslash or NL character
		if (tm=='\n')				// this is a newline
		{					// output text up to that
			if (!lead.isEmpty()) curs->insertText(lead);

			i = r1.indexIn(text,ptr,QRegExp::CaretAtOffset);
			if (i>0)			// sequence of newlines
			{
				QString ns = r1.cap(0);	// see exactly how many
				for (int j = 0; j<ns.length(); ++j)
				{			// insert paragraph breaks
					curs->insertBlock();
				}
				ptr += ns.length();
			}
			else curs->insertText(" ");	// single newline
		}
		else					// backslash start of code
		{					// output text up to that
			if (!lead.isEmpty()) curs->insertText(lead);

			char nx = text.at(ptr++).toLatin1();
			switch (nx)			// the code character
			{
case '\n':			curs->insertBlock();	// hard newline
				break;

case '!':			setError("Version (\\!) code not allowed here");
				skipSequence();
				continue;

case ';':			skipSequence(true);	// comment
				break;

case 'A':			if (!sequenceA()) continue;
				break;

case 'B':
case 'C':			if (!sequenceBC(nx)) continue;
				break;

case 'D':			if (!sequenceD()) continue;
				break;

case 'F':			if (!sequenceF()) continue;
				break;

case 'L':
case 'P':			if (!sequenceLP(nx)) continue;
				break;

case 'M':			if (!sequenceM()) continue;
				break;

case 'U':			if (!sequenceU()) continue;
				break;

case 'V':			if (!sequenceV()) continue;
				break;

case '0':
case '1':
case '2':
case '3':
case '4':
case '5':
case '6':
case '7':
case '8':
case '9':			if (!sequenceNumber(nx)) continue;
				break;

case '-':			break;			// soft hyphen

case '\\':			curs->insertText(QString(nx));
				break;			// literal backslash

default:			setError(QString("Unknown code '%1'").arg(nx));
				skipSequence();
				break;
			}
		}
	}
}


TextAreaDocument::~TextAreaDocument()
{
	if (curs!=NULL) delete curs;
}


void TextAreaDocument::setError(const QString &msg,bool fatal) const
{
	const int lineno = text.left(ptr).count('\n');
	rdr->addError((msg+QString(" at line %1").arg(lineno+1)),
		      (fatal ? Draw::errorFATAL : Draw::errorWARNING));
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Text area control sequences						//
//									//
//  See RiscOS PRM pages 4-464 et seq for control sequence formats.	//
//									//
//  The Qt text document and painting system does not support all of	//
//  the effects that would be necessary to fully implement RiscOS text	//
//  areas.  The incompatibilities and restructions are:			//
//									//
//    Background (\B)		Qt doesn't support background colour	//
//				hinting, so the 'B' code is ignored.	//
//									//
//    Columns (\D)		The 'D' code is parsed but ignored,	//
//				rendering always uses the number of	//
//				column objects.				//
//									//
//    Underline (\U)		Qt doesn't support variable underline	//
//				thickness or positioning.  All		//
//				underlining appears as standard.	//
//									//
//    Vertical move (\V)	Qt doesn't support arbitrary vertical	//
//				moves.  The cumulative vertical offset	//
//				is tracked through all 'V' codes.	//
//				Zero gives normal text; any positive	//
//				value (move up) gives a superscript,	//
//				while any negative value (move down)	//
//				gives a subscript.			//
//									//
//    Soft hyphen (\-)		Not supported for text layout,		//
//				ignored.				//
//									//
//////////////////////////////////////////////////////////////////////////

void TextAreaDocument::skipSequence(bool nlonly)
{
	for (;;)
	{
		if (ptr>=text.length()) break;
		char ch = text.at(ptr++).toLatin1();
		if (ch=='\n') break;
		if (ch=='/' && !nlonly) break;
	}
}


bool TextAreaDocument::sequenceA()
{
	QRegExp reg("^([LRCD])/?");
	reg.setCaseSensitivity(Qt::CaseInsensitive);

	if (reg.indexIn(text,ptr,QRegExp::CaretAtOffset)<0)
	{
		setError("Unrecognised align (\\A) code");
		skipSequence();
		return (false);
	}

	if (!curs->atBlockStart()) curs->insertBlock();	// forces a line break
	switch (reg.cap(1).at(0).toUpper().toLatin1())
	{
case 'L':	blkfmt.setAlignment(Qt::AlignLeft);    break;
case 'R':	blkfmt.setAlignment(Qt::AlignRight);   break;
case 'C':	blkfmt.setAlignment(Qt::AlignHCenter); break;
case 'D':	blkfmt.setAlignment(Qt::AlignJustify); break;
	}
	curs->setBlockFormat(blkfmt);

	ptr += reg.cap(0).length();
	return (true);
}


bool TextAreaDocument::sequenceBC(char what)
{
	QRegExp reg("^\\s*(\\d+)\\s+(\\d+)\\s+(\\d+)[/\\n]");

	if (reg.indexIn(text,ptr,QRegExp::CaretAtOffset)<0)
	{
		setError("Unrecognised colour (\\B/\\C) sequence");
		skipSequence();
		return (false);
	}

	int colr = reg.cap(1).toInt();
	int colg = reg.cap(2).toInt();
	int colb = reg.cap(3).toInt();
	if (what=='C') charfmt.setForeground(QBrush(QColor(colr,colg,colb)));
	curs->setCharFormat(charfmt);

	ptr += reg.cap(0).length();
	return (true);
}


bool TextAreaDocument::sequenceD()
{
	QRegExp reg("^\\s*(\\d+)[/\\n]");

	if (reg.indexIn(text,ptr,QRegExp::CaretAtOffset)<0)
	{
		setError("Unrecognised columns (\\D) sequence");
		skipSequence();
		return (false);
	}

	ptr += reg.cap(0).length();
	return (true);
}


bool TextAreaDocument::sequenceF()
{
	QRegExp reg("^\\s*(\\d\\d?)\\s+(\\S+)\\s+(\\d+)(?:\\s+(\\d+))?[/\\n]");

	if (reg.indexIn(text,ptr,QRegExp::CaretAtOffset)<0)
	{
		setError("Unrecognised font (\\F) sequence");
		skipSequence();
		return (false);
	}

	fontmap.add(reg.cap(1).toInt(),reg.cap(2).toLatin1(),reg.cap(3).toInt(),reg.cap(4).toInt());

	ptr += reg.cap(0).length();
	return (true);
}


bool TextAreaDocument::sequenceLP(char what)
{
	QRegExp reg("^\\s*(\\d+)[/\\n]");

	if (reg.indexIn(text,ptr,QRegExp::CaretAtOffset)<0)
	{
		setError("Unrecognised leading/paragraph (\\L/\\P) sequence");
		skipSequence();
		return (false);
	}

	charfmt.setProperty((what=='L' ? TextAreaDocument::LeadingProperty : TextAreaDocument::ParagraphProperty),
			    QVariant(reg.cap(1).toInt()));
	curs->setCharFormat(charfmt);

	ptr += reg.cap(0).length();
	return (true);
}


bool TextAreaDocument::sequenceM()
{
	QRegExp reg("^\\s*(\\d+)\\s+(\\d+)/?");

	if (reg.indexIn(text,ptr,QRegExp::CaretAtOffset)<0)
	{
		setError("Unrecognised margin (\\M) sequence");
		skipSequence();
		return (false);
	}

	charfmt.setProperty(TextAreaDocument::LeftMarginProperty,QVariant(reg.cap(1).toInt()));
	charfmt.setProperty(TextAreaDocument::RightMarginProperty,QVariant(reg.cap(2).toInt()));
	curs->setCharFormat(charfmt);

	ptr += reg.cap(0).length();
	return (true);
}


bool TextAreaDocument::sequenceU()
{
	QRegExp reg1("^\\s*(-?\\d+)\\s+(\\d+)/?");
	QRegExp reg2("^\\s*\\./?");

	if (reg2.indexIn(text,ptr,QRegExp::CaretAtOffset)>=0)
	{
		charfmt.setFontUnderline(false);
		ptr += reg2.cap(0).length();
	}
	else if (reg1.indexIn(text,ptr,QRegExp::CaretAtOffset)>=0)
	{
		int thickness = reg1.cap(2).toInt();
		charfmt.setFontUnderline(thickness!=0);
		ptr += reg1.cap(0).length();
	}
	else
	{
		setError("Unrecognised underline (\\U) sequence");
		skipSequence();
		return (false);
	}

	curs->setCharFormat(charfmt);
	return (true);
}

bool TextAreaDocument::sequenceV()
{
	QRegExp reg("^\\s*(-?\\d+)/?");

	if (reg.indexIn(text,ptr,QRegExp::CaretAtOffset)<0)
	{
		setError("Unrecognised vertical move (\\V) sequence");
		skipSequence();
		return (false);
	}

	vmove += reg.cap(1).toInt();			// accumulate total move

	QTextCharFormat::VerticalAlignment aln;		// select appropriate format
	if (vmove<0) aln = QTextCharFormat::AlignSubScript;
	else if (vmove>0) aln = QTextCharFormat::AlignSuperScript;
	else aln = QTextCharFormat::AlignNormal;
	charfmt.setVerticalAlignment(aln);
	curs->setCharFormat(charfmt);

	ptr += reg.cap(0).length();
	return (true);
}


bool TextAreaDocument::sequenceNumber(char first)
{
	QRegExp reg("^(\\d?)/?");			// maximum 2 digits (same in 'F')

	if (reg.indexIn(text,ptr,QRegExp::CaretAtOffset)<0)
	{
		setError("Unrecognised font number (\\digit) sequence");
		skipSequence();
		return (false);
	}

	Draw::fontid ref = (QString(first)+reg.cap(1)).toInt();

	QFont f = fontmap.findFont(ref);
	charfmt.setFont(f);
	curs->setCharFormat(charfmt);

	ptr += reg.cap(0).length();
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTAREAOBJECT -- Text area and its formatted document.	//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTextAreaObject::DrawTextAreaObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
	document = NULL;
}


DrawTextAreaObject::~DrawTextAreaObject()
{
	qDeleteAll(columns);
	if (document!=NULL) delete document;
}


bool DrawTextAreaObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_textareahdr'
	if (!DrawObject::build(rdr,diag)) return (false);

	for (;;)					// read 'draw_textcolstr column[]'
	{
		rdr.save();
		DrawObject *obj = DrawObjectManager::create(rdr,diag,DrawObjectManager::ExpectEndMark);
		rdr.restore();
		if (obj==NULL) return (false);

		if (obj->type()==Draw::objENDMARK)	// have read 'int endmark'
		{					// end of text columns
			delete obj;			// don't need that any more
			break;				// finished reading columns
		}

		if (obj->type()!=Draw::objTEXTCOL)	// shouldn't be anything else
		{
			rdr.addError(QString("Text area contains non-column object %1").arg(obj->type()),Draw::errorWARNING);
			continue;
		}

		columns.append(static_cast<DrawTextColumnObject *>(obj));
	}
							// from 'struct draw_textareaend'
	if (!rdr.getWord(nullptr) ||			// read 'int blank1'
	    !rdr.getWord(nullptr)) return (false);	// read 'int blank2'

	if (!rdr.getWord(&textcolour) ||		// read 'draw_coltyp textcolour'
	    !rdr.getWord(&backcolour)) return (false);	// read 'draw_coltyp background'

	if (!rdr.getString(&text)) return (false);	// read 'char text[]'
	DrawUtil::substituteRiscosSpecialChars(text);

	if (columns.count()<1) rdr.addError("Text area contains no columns");
	else
	{						// parse text into document
		document = new TextAreaDocument(rdr,text,DrawUtil::toQColor(textcolour));
	}

	return (rdr.discardWordAlign());
}


void DrawTextAreaObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
	const QString indent = indent1+indent2;

	str << indent << qSetPadChar('0')
	    << "textcol " << Qt::hex << qSetFieldWidth(8) << textcolour << qSetFieldWidth(0) << Qt::dec
	    << " backcol " << Qt::hex << qSetFieldWidth(8) << backcolour << qSetFieldWidth(0) << Qt::dec
	    << Qt::endl;

	for (QList<DrawTextColumnObject *>::const_iterator it = columns.begin();
	     it!=columns.end(); ++it)
	{
		(*it)->dump(str,(indent1+indent2),"|  ");
	}

	QByteArray tx(text);				// because 'replace()' modifies
	str << indent << "|" << Qt::endl;
	str << indent
	    << "+- Text length " << tx.size()
	    << Qt::endl;
	str << indent << "   " << tx.replace('\n',("\n"+indent+"   ")) << Qt::endl;
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Text area drawing.							//
//									//
//  Text is formatted into the columns until either the text is		//
//  finished or the bottom of the last column is reached.  If the end	//
//  of the columns is reached but there is still some text remaining,	//
//  it is allowed to overflow the column for a maximum of two more	//
//  lines.  This is to at least allow the text to be visible, even if	//
//  it overflows the layout slightly - which is very possible given	//
//  the difference in font metrics between RiscOS and the running	//
//  system.  The comments at the top of 'textobjects.cpp' regarding	//
//  font compatibility also apply here.					//
//									//
//  The various tweaking factors noted below should really be put in	//
//  a configuration file (or, at least, defined as constants).		//
//									//
//////////////////////////////////////////////////////////////////////////

void DrawTextAreaObject::getCharProperties(const QTextCursor *cur,int *lp,int *pp,int *lm,int *rm) const
{
	const QTextCharFormat fmt = cur->charFormat();

	*lp = DrawCoord::pointToUnscaledV(fmt.property(TextAreaDocument::LeadingProperty).toInt());
	*pp = DrawCoord::pointToUnscaledV(fmt.property(TextAreaDocument::ParagraphProperty).toInt());
	*lm = DrawCoord::pointToUnscaledV(fmt.property(TextAreaDocument::LeftMarginProperty).toInt());
	*rm = DrawCoord::pointToUnscaledV(fmt.property(TextAreaDocument::RightMarginProperty).toInt());
}


bool DrawTextAreaObject::draw(QPainter &p,const DrawDiagram *diag,const PaintOptions *opts) const
{
	if (!DrawObject::draw(p,diag,opts)) return (false);

	if (document==NULL || document->isEmpty()) return (false);
							// nothing to draw
	p.save();					// save global state
	QFont f("times",12);				// initial default font
	p.setFont(f);					// to get metrics on painter
	const int lh = p.fontMetrics().height();	// default line height
	const int ps = p.fontMetrics().ascent();	// default paragraph spacing
	// TODO: tweak to some factor of 'lineSpacing()'

	p.setPen(!DrawUtil::isTransparent(textcolour) ? DrawUtil::toQColor(textcolour) : Qt::black);

	document->documentLayout()->setPaintDevice(p.device());

	QList<DrawTextColumnObject *>::const_iterator it = columns.begin();
							// start at first column
	if (opts->flags() & PaintOptions::DisplayBoundingBoxes)
	{						// display column bounding boxes
		while (it!=columns.end()) (*it++)->draw(p,diag,opts);
		it = columns.begin();			// reset back to start
	}

	const DrawTextColumnObject *col = NULL;		// current text column
							// expect 'might be used uninitialized' for...
	int width;					// width of text
	int bottom;					// limit of text column
	int x,y;					// position of current line

	QTextBlock blk = document->begin();		// start at first text block

	int leading = 0;				// current text leading
	int paraspc = 0;				// current paragraph spacing
	int leftmargin = 0;				// current margins
	int rightmargin = 0;

	int overlines = 0;				// lines allowed to overflow

	QTextLayout *lay = NULL;
	QTextCursor *cur = NULL;

	p.translate(DrawCoord::toPixelX(mBbox.x0()),	// origin to left of area
		    DrawCoord::toPixelY(mBbox.y1()));
	p.scale(DrawCoord::zoom(),DrawCoord::zoom());	// scale for display zoom

	for (;;)					// until end of text or columns
	{
		if (!blk.isValid()) break;		// no more text blocks

		if (col==NULL)				// set up for new column
		{
			col = (*it);			// find current column

			width = DrawCoord::toUnscaledH(col->bbox()->x1()-col->bbox()->x0());
			x = DrawCoord::toUnscaledH(col->bbox()->x0()-mBbox.x0());
			y = DrawCoord::toUnscaledV(mBbox.y1()-col->bbox()->y1());
			bottom = DrawCoord::toUnscaledV(mBbox.y1()-col->bbox()->y0());
							// calculate text size of column
		}

		// TODO: tweak to some factor of 'lh'
		if (y>(bottom-lh))			// overflows bottom of column
		{
			// TODO: tweak this limit
			if (overlines>2) break;		// too many overflow lines

			if (overlines==0)		// not in overflowing mode
			{
				++it;			// step to next column
				if (it==columns.end())	// see if end of columns
				{
					overlines = 1;	// start counting overflow lines
				}
			}
			else ++overlines;		// count up overflowing lines

			if (overlines==0)		// still not in overflowing mode
			{
				col = NULL;		// set up next time round
				continue;
			}
		}

		if (lay==NULL)				// set up for new block layout
		{
			lay = new QTextLayout(blk);
			lay->setCacheEnabled(true);	// will use immediately afterwards
			lay->setFont(f);
			lay->setTextOption(QTextOption(blk.blockFormat().alignment()));
			lay->beginLayout();

			cur = new QTextCursor(blk);	// cursor at start of block
			getCharProperties(cur,&leading,&paraspc,&leftmargin,&rightmargin);
		}

		QTextLine line = lay->createLine();
		if (!line.isValid())			// reached end of text block
		{
			lay->endLayout();		// finish and draw layout
			if (lay->lineCount()>0) lay->draw(&p,QPoint(0,0));

			delete lay; lay = NULL;		// recreate next time round
			delete cur; cur = NULL;

			for (;;)			// step to next non-empty block
			{
				blk = blk.next();	// move on to next block
				if (blk.length()!=1) break;
				y += (paraspc==0 ? ps : paraspc);
							// empty block, new paragraph
			}				// and go on to next
			continue;
		}

		line.setLineWidth(width-(leftmargin+rightmargin));
		line.setPosition(QPoint(x+leftmargin,y));

		cur->movePosition(QTextCursor::NextCharacter,QTextCursor::MoveAnchor,line.textLength());
		getCharProperties(cur,&leading,&paraspc,&leftmargin,&rightmargin);

		// TODO: tweak 'leading' to a bit less
		y += (leading==0 ? static_cast<int>(line.height()) : leading);
	}

	if (lay!=NULL)					// finish last layout
	{
		lay->endLayout();
		if (lay->lineCount()>0) lay->draw(&p,QPoint(0,0));
		delete lay;
		delete cur;
	}

	p.restore();					// finally restore global state
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTAREAOBJECTCREATOR - New object creation		//
//  Class DRAWTEXTAREAOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextAreaObjectCreator : public DrawObjectCreator
{
	friend class DrawTextAreaObjectSetup;
public:
	DrawTextAreaObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawTextAreaObject(flag,layer));
	}
};


class DrawTextAreaObjectSetup
{
private:
	DrawTextAreaObjectSetup();
	static DrawTextAreaObjectSetup instance;
};


DrawTextAreaObjectSetup DrawTextAreaObjectSetup::instance;


DrawTextAreaObjectSetup::DrawTextAreaObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objTEXTAREA,new DrawTextAreaObjectCreator);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTCOLUMNOBJECT -- Text column contained within a text	//
//  area.								//
//									//
//////////////////////////////////////////////////////////////////////////

DrawTextColumnObject::DrawTextColumnObject(Draw::objflags flag,int layer)
	: DrawObject(flag,layer)
{
}

bool DrawTextColumnObject::build(DrawReader &rdr,DrawDiagram *diag)
{							// from 'struct draw_textcolstr'
	return (DrawObject::build(rdr,diag));		// no additional data
}

void DrawTextColumnObject::dump(QTextStream &str,const QString &indent1,const QString &indent2) const
{
	DrawObject::dump(str,indent1,indent2);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Class DRAWTEXTCOLUMNOBJECTCREATOR - New object creation		//
//  Class DRAWTEXTCOLUMNOBJECTSETUP - Initialisation for above		//
//									//
//////////////////////////////////////////////////////////////////////////

class DrawTextColumnObjectCreator : public DrawObjectCreator
{
	friend class DrawTextColumnObjectSetup;
public:
	DrawTextColumnObject *create(Draw::objflags flag,int layer = 0) override
	{
		return (new DrawTextColumnObject(flag,layer));
	}
};


class DrawTextColumnObjectSetup
{
private:
	DrawTextColumnObjectSetup();
	static DrawTextColumnObjectSetup instance;
};


DrawTextColumnObjectSetup DrawTextColumnObjectSetup::instance;


DrawTextColumnObjectSetup::DrawTextColumnObjectSetup()
{
	DrawObjectManager::registerCreator(Draw::objTEXTCOL,new DrawTextColumnObjectCreator);
}
