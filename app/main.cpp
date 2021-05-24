//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Application					//
//  Edit:	24-May-21						//
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
#include <qcommandlineparser.h>
#include <qurl.h>
#include <qdir.h>

#include <kaboutdata.h>
#include <klocalizedstring.h>

#include "drawview.h"
#include "version.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  MAIN -- Initialise the application, then create viewer(s) to load	//
//  files specified on the command line.				//
//									//
//////////////////////////////////////////////////////////////////////////

int main(int argc,char *argv[])
{
    KAboutData aboutData("drawview",			// componentName
                         i18n("DrawView"),		// displayName
#ifdef VCS_HAVE_VERSION
                         ( VERSION " (" VCS_TYPE_STRING " " VCS_REVISION_STRING ")" ),
#else
                         VERSION,			// version
#endif
                         i18n("RiscOS Draw file viewer"),
                         KAboutLicense::GPL_V2,
                         i18n("Copyright (c) 2006-2021 Jonathan Marten"),
                         "",				// otherText
                         PACKAGE_URL,			// homePageAddress
                         PACKAGE_BUGREPORT);		// bugsEmailAddress
    aboutData.addAuthor(i18n("Jonathan Marten"),
                        "",
                        PACKAGE_BUGREPORT,
                        "http://www.keelhaul.me.uk");

    QApplication app(argc, argv);
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.setApplicationDescription(aboutData.shortDescription());

    parser.addPositionalArgument("file", i18n("File to load"), i18n("[file...]"));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    DrawView *v = nullptr;
    const QStringList args = parser.positionalArguments();
    for (int i = 0; i<args.count(); ++i)		// load project or data files
    {
        // Parsing file arguments as URLs, as recommended at
        // http://marc.info/?l=kde-core-devel&m=141359279227385&w=2
        const QUrl u = QUrl::fromUserInput(args[i], QDir::currentPath(), QUrl::AssumeLocalFile);
        if (!u.isValid())
        {
            warnmsg() << "Invalid file URL" << u;
            continue;
        }

        v = new DrawView(u.toLocalFile());
        if (!v->isValid()) v->deleteLater();
        else v->show();
    }

    if (v==nullptr)					// no files were loaded
    {
        v = new DrawView(QString());
        v->show();
    }

    app.setQuitOnLastWindowClosed(true);		// can do this now
    return (app.exec());
}  
