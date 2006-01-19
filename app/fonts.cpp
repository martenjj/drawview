//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Library					//
//  SCCS:	<%Z% %M% %I%>					//
//  Edit:	06-Dec-05						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  Font lookup and substitutions.					//
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

#undef DEBUG_FONTS

//////////////////////////////////////////////////////////////////////////
//									//
//  Include files							//
//									//
//////////////////////////////////////////////////////////////////////////

#include "global.h"

#include <qmap.h>
#include <qfont.h>
#include <qfontinfo.h>
#include <qalgorithms.h>
#include <qstringlist.h>

#include "coord.h"
#include "files.h"

#include "fonts.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Printable font descriptions						//
//									//
//////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_FONTS

static QString describeFont(const QFont &ff)
{
	QString s("[fam '%1' pt %2 sty %3 wgt %4 str %5]");
	return (s.arg(ff.family()).arg(ff.pointSize()).arg(ff.style())
		.arg(ff.weight()).arg(ff.stretch()));
}


static QString describeInfo(const QFontInfo &fi)
{
	QString s("[fam '%1' pt %2 sty %3 wgt %4]");
	return (s.arg(fi.family()).arg(fi.pointSize()).arg(fi.style()).arg(fi.weight()));
}

#endif							// DEBUG_FONTS

//////////////////////////////////////////////////////////////////////////
//									//
//  Class FONTNAMEMAPENTRY -- The Qt font family, style and weight	//
//  deduced from a RiscOS font name.					//
//									//
//////////////////////////////////////////////////////////////////////////

class FontNameMapEntry
{
public:
	FontNameMapEntry(QString name);

	const QString &family() const		{ return (fam); }
	QFont::Style style() const		{ return (sty); }
	QFont::Weight weight() const		{ return (wei); }

private:
	QString fam;
	QFont::Style sty;
	QFont::Weight wei;
};


FontNameMapEntry::FontNameMapEntry(QString name)
{
	QStringList names = name.toLower().split('.');	// split up name into components
	fam = names.takeFirst();			// take family name as it is

	sty = QFont::StyleNormal;			// look for possible style names
	if (names.contains("italic")) sty = QFont::StyleItalic;
	else if (names.contains("oblique")) sty = QFont::StyleOblique;

	wei = QFont::Normal;				// look for possible weight names
	if (names.contains("light")) wei = QFont::Light;
	else if (names.contains("demi")) wei = QFont::DemiBold;
	else if (names.contains("bold")) wei = QFont::Bold;
	else if (names.contains("black") || names.contains("heavy")) wei = QFont::Black;
	else if (names.contains("ultra") || names.contains("ultrabold")) wei = QFont::Black;

#ifdef DEBUG_FONTS
	QFont font(fam,12);				// do a lookup for display
	font.setStyle(sty);
	font.setWeight(wei);
	QFontInfo fi(font);

	debugmsg(0) << funcinfo
		    << "for '" << name << "'"
		    << " fam='" << fam << "' sty=" << sty << " wei=" << wei
		    << " requested " << describeFont(font)
		    << " using " << describeInfo(fi);
#endif							// DEBUG_FONTS
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Internal variables							//
//									//
//////////////////////////////////////////////////////////////////////////

static int systemFontAdjust = 100;			// width adjustment

//////////////////////////////////////////////////////////////////////////
//									//
//  Class FONTNAMEMAP -- Mapping from a RiscOS font name to a Qt font	//
//  family, style and weight.						//
//									//
//  The format of the configuration file ("fontmap") is:		//
//									//
//    riscosname    fontname[,fontname...]    [adjusth    [adjustv]]	//
//									//
//  'riscosname' is the name that is expected to be found in a font	//
//  table or text area object.						//
//									//
//  'fontname' is the Qt font family name that is equivalent to that.	//
//  Multiple names may be given, each separated by commas.  A font	//
//  name containing spaces may be quoted.				//
//									//
//  The 'adjust' factors, if present, modify the standard width or	//
//  height of this font to better fit the RiscOS font metrics.  At	//
//  present this is only implemented for the "system" font width.	//
//									//
//  The 'riscosname' may be one of these special names:			//
//									//
//    SYSTEM	Font equivalent for the RiscOS system font.  This	//
//		should be a fixed-width font.				//
//									//
//    DEFAULT	Font to be used when no equivalent is defined.		//
//									//
//  Matching of font names (including the special values above) is	//
//  case insensitive.							//
//									//
//////////////////////////////////////////////////////////////////////////

FontNameMap::FontNameMap()
{
	FileReader fr("fontmap");
	if (!fr.isValid())
	{
		fr.reportError("font map");
		return;
	}

	QStringList fields;
	while ((fields = fr.getParsedLine()).count()>0)
	{
// TODO: adjustment factors for all fonts
		QString fontname = fields[0].toLower();
#ifdef DEBUG_FONTS
		debugmsg(0) << "  substitute '" << fontname << "' with '" << fields[1] << "'";
#endif							// DEBUG_FONTS
		QFont::insertSubstitutions(fontname,fields[1].split(','));

		if (fontname=="system")
		{
			const int adjust = fields[2].toInt();
			if (adjust>0) systemFontAdjust = adjust;
		}
	}
}


FontNameMap::~FontNameMap()
{
	qDeleteAll(list);
}


FontNameMap *FontNameMap::self()
{
	static FontNameMap *mapInstance = new FontNameMap();
	return (mapInstance);
}


const FontNameMapEntry *FontNameMap::findFont(const QString &name)
{
#ifdef DEBUG_FONTS
	debugmsg(0) << funcinfo << "for '" << name << "'";
#endif							// DEBUG_FONTS
	FontNameMapEntry *ent = list[name];
	if (ent==NULL)
	{
		ent = new FontNameMapEntry(name);
		list[name] = ent;
	}

	return (ent);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  Structure FONTREFMAPENTRY -- The RiscOS font name and its mapping	//
//  corresponding to an object font reference.				//
//									//
//  The 'size' and 'width' stored here is only used for text areas,	//
//  where they are explicitly defined for the font reference.		//
//									//
//////////////////////////////////////////////////////////////////////////

struct FontRefMapEntry
{
	FontRefMapEntry(const QString name)	{ riscosname = name; size = width = 0; m = NULL; }
	QString riscosname;
	int size;
	int width;
	const FontNameMapEntry *m;
};

//////////////////////////////////////////////////////////////////////////
//									//
//  Class FONTREFERENCEMAP -- Mapping from a drawing font reference	//
//  to a RiscOS font name, and therefore a Qt font.  There is one of	//
//  these mappings for all text objects in a drawing, and a separate	//
//  mapping for each text area object.					//
//									//
//////////////////////////////////////////////////////////////////////////

FontReferenceMap::FontReferenceMap()
{
	debugmsg(0) << funcinfo;
}


FontReferenceMap::~FontReferenceMap()
{
	qDeleteAll(list);
}


void FontReferenceMap::add(Draw::fontid ref,const char *name,int size,int width)
{
	debugmsg(0) << funcinfo << "add ref " << ref << " -> '" << name << "' size=" << size << " width=" << width;
	list[ref] = new FontRefMapEntry(name);
	if (size>0) list[ref]->size = size;
	if (width>0) list[ref]->width = width;
}


bool FontReferenceMap::isKnown(Draw::fontid ref) const
{
	return (list.contains(ref));
}


QFont FontReferenceMap::findFont(Draw::fontid ref,int size,int width) const
{
	const bool forobj = !(size==-1 && width==-1);	// whether for text object or area
	QString family = "default";			// fallback font name
	QFont::Style style = QFont::StyleNormal;
	QFont::Weight weight = QFont::Normal;

	FontRefMapEntry *ent = NULL;
	if (list.contains(ref))				// font reference is known
	{
		ent = list[ref];
		if (ent->m==NULL) ent->m = FontNameMap::self()->findFont(ent->riscosname);
		const FontNameMapEntry *map = ent->m;	// find or create mapping

		family = map->family();			// get details from mapping
		style = map->style();
		weight = map->weight();
	}
							// use as stored if not specified
	if (size==-1) size = (ent==NULL ? 0 : ent->size);
	if (width==-1) width = (ent==NULL ? 0 : ent->width);
	if (size==0) size = 12*640;			// should never happen now
	if (width==0) width = size;			// unspecified means natural size

	QFont font(family,(forobj ? DrawCoord::fontToPoint(size) : size));
	if (width!=size) font.setStretch((width*100)/size);
	if (ref==0)					// adjust "System Font"
	{
		font.setStyleHint(QFont::TypeWriter,static_cast<QFont::StyleStrategy>(QFont::PreferDefault+QFont::PreferMatch));
		font.setStretch(systemFontAdjust);
	}						// outline scales better
	else font.setStyleHint(QFont::AnyStyle,QFont::ForceOutline);

	font.setStyle(style);				// create and return font
	font.setWeight(weight);
	return (font);
}


void FontReferenceMap::dump(QTextStream &str,const QString &indent) const
{
	for (QMap<Draw::fontid,FontRefMapEntry *>::const_iterator it = list.begin();
	     it!=list.end(); ++it)
	{
		if (it.key()==0) continue;
		const FontRefMapEntry *ent = it.value();

		str << indent << "ref " << it.key() << " -> " << ent->riscosname;
		if (ent->m!=NULL)		// has been resolved
		{
			str << " = family \"" << ent->m->family() << "\""
			    << " style " << ent->m->style() << " weight " << ent->m->weight();
		}
		str << endl;

	}
}
