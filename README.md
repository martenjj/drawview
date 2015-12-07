README file for DrawView
========================

DrawView is an application that allows Draw files created by the
Acorn/RiscOS application Draw, and other compatible applications, to
be displayed on a Linux or other Unix system.  It is written in C++
and released under the GNU GPL.

DrawView requires the Qt 4 libraries (version 4.1.0 or later),
available from www.qt.io or included in many Linux distributions.
Qt/X11 is free for use and development of open-source applications.

The file is displayed in a desktop window which can be scrolled and
zoomed as required.  The drawing can be exported in PDF or PostScript
format for printing or use in other applications.


The file formats supported, and their original Acorn file types, are:

| File format               | Acorn file type | Applications              |
| ------------------------- | --------------- | ------------------------- |
| Plain Draw file           | DrawFile (AFF)  | Draw, many others         |
| Draw file with extra data | DrawFile (AFF)  | DrawPlus                  |
| Compressed Draw           | CmprDraw (C4B)  | Chameleon, Poster, Vector |
| Compressed with extras    | VecFile  (C56)  | Vector                    |
| Draw with tagged objects  | Poster   (CC3)  | Poster                    |

All types of standard Draw objects, and the additional ones provided
by Vector, are supported and displayed.  There are some features that
it is not possible to reproduce exactly, due to limitations in the Qt
library or the underlying platform, but the results obtained should be
as close as it is possible to achieve.  See the home page for further
information.

Best results for text display are obtained if a good selection of
TrueType fonts are installed.  The mapping between the original Acorn
font names and available fonts is configurable.


The source download (with some example files) and further information
is available from GitHub at:

  https://github.com/martenjj/drawview

More information and documentation is available from the home page:

   http://www.keelhaul.me.uk/acorn/drawview/

To build and install the application, you will need GNU autotools
installed on your system.  This may seem like an addition to the usual
'download, untar, configure, make, install' process, but it is not a
good idea to keep autotools-generated files in a VCS such as git.  See
http://www.gnu.org/software/automake/manual/html_node/CVS.html for more
information on why.

    # Clone the source repository from GitHub
    git clone https://github.com/martenjj/drawview.git
    cd drawview
    # Generate the required configuration files and scripts
    autoreconf -i
    # Now follow the standard build steps, see the INSTALL file
    # for more information.
    ./configure [options...]
    make
    make install
    drawview [file...]


This is a work in progress; further facilities and improvements are
planned.  Reasonable suggestions, bug or problem reports are welcome.
