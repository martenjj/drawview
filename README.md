README file for DrawView
========================

DrawView is an application that allows Draw files created by the
Acorn/RiscOS application Draw, and other compatible applications, to
be displayed on a Linux or other Unix system.  It is written in C++
using the Qt and KDE Frameworks libraries, and released under the GNU
GPL.

The file is displayed in a desktop window which can be scrolled and
zoomed as required.  The drawing can be exported in PDF, SVG or
PostScript format for printing or use in other applications.

------------------------------------------------------------------------

The file formats supported, and their original Acorn file types, are:

| File format               | Acorn file type | Applications              |
| ------------------------- | --------------- | ------------------------- |
| Plain Draw file           | DrawFile (AFF)  | Draw, many others         |
| Draw file with extra data | DrawFile (AFF)  | DrawPlus                  |
| Compressed Draw           | CmprDraw (C4B)  | Chameleon, Poster, Vector |
| Compressed with extras    | VecFile  (C56)  | Vector                    |
| Draw with tagged objects  | Poster   (CC3)  | Poster                    |
| Object library            | DrawLib  (810)  | DrawPlus, Vector          |

All types of standard Draw objects, and the additional ones provided
by Vector, are supported and displayed.  There are some features that
it is not possible to reproduce exactly, due to limitations in the Qt
library or the underlying platform, but the results obtained should be
as close as it is possible to achieve.  See the home page for further
information.

Libraries are initially displayed with all of the included objects
superimposed.  The "Library Object" selector (in the "View" menu) can
be used to display or export a single object.

The best results for text display are obtained if a good selection of
scalable fonts are installed.  The mapping between the original Acorn
font names and available fonts is configurable.

------------------------------------------------------------------------

DrawView requires the Qt 5 (version 5.10 or later, available from
www.qt.io) and the KDE Frameworks (version 5.68 or later, available
from www.kde.org) libraries.  These are already included in many Linux
distributions.  Qt/X11 is free for use and development of open-source
applications.  The 'qt4' branch works with the older Qt 4, while the
'qt5' branch works with Qt 5 alone (no need for Frameworks).

The source download (with some example files) and further information
is available from GitHub at:

  https://github.com/martenjj/drawview

More information and documentation is available from the home page:

   http://www.keelhaul.me.uk/acorn/drawview/

To build and install the application, you will need CMake to configure
and generate build information.  Assuming that you have this and the
Qt and Frameworks libraries as above, build the application as
follows:

    # Clone the source repository from GitHub
    git clone https://github.com/martenjj/drawview.git
    cd drawview
    git checkout frameworks5
    # Generate the required configuration files and scripts
    mkdir BUILD
    cd BUILD
    cmake ..
    # Now follow the standard build steps.
    make
    make install
    drawview [file...]

The Qt-only branch ('qt5') still uses GNU autotools.  To build this
version, do:

    # Clone the source repository from GitHub
    git clone https://github.com/martenjj/drawview.git
    cd drawview
    git checkout qt5
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
