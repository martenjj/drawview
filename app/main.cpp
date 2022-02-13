//////////////////////////////////////////////////////////////////////////
//									//
//  Project:	DrawView - Application					//
//  Edit:	13-Feb-22						//
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

#include <iostream>

#include "global.h"

#include <qapplication.h>
#include <qcommandlineparser.h>
#include <qurl.h>
#include <qdir.h>
#include <qfileinfo.h>

#include <kaboutdata.h>
#include <klocalizedstring.h>

#include "drawview.h"
#include "version.h"

//////////////////////////////////////////////////////////////////////////
//									//
//  Error reporting							//
//									//
//////////////////////////////////////////////////////////////////////////

bool guiMode = true;					// running as a GUI application

void reportError(const QString &msg, bool fatal)
{
    std::cerr << qPrintable(QCoreApplication::applicationName())
              << " (" << qPrintable(fatal ? i18n("ERROR") : i18n("WARNING")) << "): "
              << qPrintable(msg)
              << std::endl;
    if (fatal) std::exit(EXIT_FAILURE);
}


static bool isValidFormat(const QString &fmt)
{
    return (fmt=="svg" || fmt=="pdf" || fmt=="ps");
}

//////////////////////////////////////////////////////////////////////////
//									//
//  MAIN -- Initialise the application, then create viewer(s) to load	//
//  files specified on the command line.				//
//									//
//////////////////////////////////////////////////////////////////////////

int main(int argc,char *argv[])
{
    KAboutData aboutData(PACKAGE,			// componentName
                         i18n(PACKAGE_NAME),		// displayName
#ifdef VCS_HAVE_VERSION
                         ( VERSION " (" VCS_TYPE_STRING " " VCS_REVISION_STRING ")" ),
#else
                         VERSION,			// version
#endif
                         i18n("RiscOS Draw file viewer"),
                         KAboutLicense::GPL_V2,
                         i18n("Copyright (c) 2006-2022 Jonathan Marten"),
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

    parser.addPositionalArgument("file", i18n("Drawing file to load"), i18n("[file...]"));

    QCommandLineOption targetDirectoryOption(QStringList() << "e" << "export",
                                             i18n("Export input file as <file> or files into <directory>"),
                                             i18n("file|directory"));
    parser.addOption(targetDirectoryOption);

    QCommandLineOption targetFormatOption(QStringList() << "f" << "format",
                                          i18n("Format to export as (SVG, PDF, PS), default SVG"),
                                          i18n("format"));
    parser.addOption(targetFormatOption);

    QCommandLineOption targetOverwriteOption(QStringList() << "o" << "overwrite",
                                          i18n("Overwrite already existing export files"));
    parser.addOption(targetOverwriteOption);

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    QString exportArg = parser.value(targetDirectoryOption);
    const QString exportFmt = parser.value(targetFormatOption).toLower();
    const bool exportOverwrite = parser.isSet(targetOverwriteOption);
    const QStringList args = parser.positionalArguments();

    if (!exportFmt.isEmpty())				// export format specified
    {
        if (exportArg.isEmpty()) reportError(i18n("Export format ('-f') ignored without export destination ('-e')"), false);
        if (!isValidFormat(exportFmt)) reportError(i18n("Unrecognised export format ('-f') \"%1\"", exportFmt));
    }

    bool dirExport = false;				// exporting to a directory
    if (!exportArg.isEmpty())				// batch export mode
    {
        if (args.isEmpty()) reportError(i18n("No files specified for export ('-e')"));

        QFileInfo fi(exportArg);
        dirExport = (fi.exists() && fi.isDir());

        if (!dirExport && args.count()>1) reportError(i18n("Export ('-e') for multiple files must be to a directory"));

        if (dirExport && !exportArg.endsWith('/')) exportArg += '/';
        qDebug() << "export to" << exportArg << "dir?" << dirExport << "overwrite?" << exportOverwrite;

        guiMode = false;				// note not running with GUI
    }
    else
    {
        if (exportOverwrite) reportError(i18n("Export overwrite ('-o') ignored without export destination ('-e')"), false);
    }

    for (const QString &arg : args)			// load drawing files
    {
        // Parsing file arguments as URLs, as recommended at
        // http://marc.info/?l=kde-core-devel&m=141359279227385&w=2
        const QUrl u = QUrl::fromUserInput(arg, QDir::currentPath(), QUrl::AssumeLocalFile);
        if (!u.isValid() || !u.isLocalFile())
        {
            reportError(i18n("Invalid URL or not local file, '%1'", arg), false);
            continue;
        }

        // Create a drawing from the input file
        const QString inPath = u.toLocalFile();
        DrawView *v = new DrawView(inPath);
        if (!v->isValid())
        {
            // There was a fatal error loading the drawing.  Nothing
            // more can be done.
            v->deleteLater();
            continue;
        }

        // If doing batch export mode, derive the output file name for
        // this input file.
        if (!exportArg.isEmpty())
        {
            // The specified export format.  If it is not explicitly
            // specified then a file extension may set it, so do not
            // apply the default yet.
            QString exportAs = exportFmt;

            QString exportPath;
            if (dirExport)
            {
                // Exporting to a directory.  We know that 'exportArg'
                // ends with a slash here, so a file name can simply be
                // appended to it.  First get the base name of the
                // input file.
                const int idx = inPath.lastIndexOf('/');
                QString exportFile = inPath.mid(idx+1);

                // Leave the input file name exactly as it is, not
                // trying to remove any extension, because there is no
                // standardisation of Acorn drawing file names.
                // Simply append the format extension, taking the default
                // format as SVG if it is not specified.
                if (exportAs.isEmpty()) exportAs = "svg";
                exportPath = exportArg+exportFile+'.'+exportAs;
            }
            else					// export is to a file
            {
                // Exporting to a file.  Get the base name of the
                // specified output file.
                const int idx = exportArg.lastIndexOf('/');
                QString exportFile = exportArg.mid(idx+1);

                // Then see if it has a recognised extension.
                const int idx2 = exportFile.lastIndexOf('.');
                if (idx2!=-1)
                {
                    // Get the file extension, which can be used as the
                    // output format if it is valid and no explicit output
                    // format has been specified.
                    QString fmt = exportFile.mid(idx2+1).toLower();
                    if (exportAs.isEmpty() && isValidFormat(fmt))
                    {
                        // The file extension is present and valid.  Use it
                        // as the output format.
                        exportAs = fmt;
                        exportPath = exportArg;
                    }
                }

                // If The file extension was present and valid, then the
                // output path should have been set by now.
                if (exportPath.isEmpty())
                {
                    // The output file had no extension, or not one that was
                    // recognised.  Simply append the format extension, taking
                    // the default format as SVG if it is not specified.
                    if (exportAs.isEmpty()) exportAs = "svg";
                    exportPath = exportArg+'.'+exportAs;
                }
            }

            qDebug() << "export to" << exportPath << "format" << exportAs;

            // The export is to 'exportPath' with the format 'exportAs'.
            // See if the output file already exists.
            if (QFile::exists(exportPath) && !exportOverwrite)
            {
                reportError(i18n("Export file '%1' already exists, not overwriting (force with '-o')", exportPath), false);
            }
            else
            {
                // Now ready to do the export.
                v->fileExportTo(exportAs, exportPath);
            }

            v->deleteLater();				// finished with the drawing
            continue;
        }

        v->show();					// not export, normal display
    }

    if (!exportArg.isEmpty()) exit(EXIT_SUCCESS);	// export finished, all done
    if (args.isEmpty())					// no arguments, just a window
    {
        DrawView *v = new DrawView(QString());
        v->show();
    }

    app.setQuitOnLastWindowClosed(true);		// can do this now
    return (app.exec());
}  
