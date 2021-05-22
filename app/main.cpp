//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Application					//
//  Edit:	22-May-21						//
//									//
//////////////////////////////////////////////////////////////////////////
//									//
//  The main program for the application.  It doesn't do much...	//
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
#include <qfile.h>
#include <qicon.h>
#include <qmessagebox.h>

#include "drawview.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Locating the application icon					//
//									//
//////////////////////////////////////////////////////////////////////////

static bool tryIcon(const QString &where)
{
	if (where.isNull()) return (false);

	QString f(where+QString("/hicolor/32x32/apps/%1.png").arg(PACKAGE));
	if (!QFile(f).exists()) return (false);

	qApp->setWindowIcon(QIcon(f));
	return (true);
}

//////////////////////////////////////////////////////////////////////////
//									//
//  MAIN -- Initialise the application, then create viewer(s) to load	//
//  files specified on the command line.				//
//									//
//////////////////////////////////////////////////////////////////////////

int main(int argc,char *argv[])
{
	QApplication app(argc,argv);

	app.setApplicationName(PACKAGE_NAME);
#ifdef DESKTOPICONS
	tryIcon(DESKTOPICONS) ||
#endif
            tryIcon(KDEICONS);

	app.setQuitOnLastWindowClosed(false);		// not during startup
 	if (!app.isSessionRestored())
	{
		bool ok = false;
		const QStringList args = app.arguments();
		const int n = args.count()-1;		// not including program name
		if (n==0)
		{
			(void) new DrawView(QString());
			ok = true;
		}
		else
		{
			for (int i = 1; i<=n; ++i)
			{
				QString file = args[i];
                                debugmsg(0) << file;
				if (file[0]=='-' && file[1]=='\0') file = "/dev/stdin";
				DrawView *v = new DrawView(file);
				if (v->isValid()) ok = true;
				else delete v;
			}
		}

		if (!ok) app.exit(EXIT_FAILURE);
	}

	app.setQuitOnLastWindowClosed(true);		// can do this now
	return (app.exec());
}  
