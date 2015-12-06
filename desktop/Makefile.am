############################################## -*- mode:makefile; -*- ####
##									##
##  Project:	DrawView - Desktop integration				##
##  Edit:	06-Dec-15						##
##									##
##########################################################################
##									##
##  Automake template makefile for the DrawView desktop integration,	##
##  for both KDE and freedesktop standards.				##
##									##
##  TODO: No freedesktop (and therefore presumably Gnome) support for	##
##  MIME types yet.							##
##									##
##########################################################################
##									##
##  Copyright (c) 2006 Jonathan Marten <jjm@keelhaul.demon.co.uk>	##
##  Home & download:  http://www.keelhaul.demon.co.uk/acorn/drawview/	##
##									##
##  This program is free software; you can redistribute it and/or	##
##  modify it under the terms of the GNU General Public License as	##
##  published by the Free Software Foundation; either version 2 of	##
##  the License, or (at your option) any later version.			##
##									##
##  It is distributed in the hope that it will be useful, but		##
##  WITHOUT ANY WARRANTY; without even the implied warranty of		##
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	##
##  GNU General Public License for more details.			##
##									##
##  You should have received a copy of the GNU General Public		##
##  License along with this program; see the file COPYING for further	##
##  details.  If not, write to the Free Software Foundation, Inc.,	##
##  59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.		##
##									##
##########################################################################

MIMEICONS =		hi16-mime-file810.png hi16-mime-fileAFF.png		\
			hi16-mime-fileBBB.png hi16-mime-fileC4B.png		\
			hi16-mime-fileC56.png hi16-mime-fileCC3.png 		\
			hi32-mime-file810.png hi32-mime-fileAFF.png		\
			hi32-mime-fileBBB.png hi32-mime-fileC4B.png		\
			hi32-mime-fileC56.png hi32-mime-fileCC3.png

APPICONS =		hi16-app-drawview.png hi32-app-drawview.png

MIMETYPES =		x-compressdraw.desktop x-riscosdraw.desktop		\
			x-riscosgridpro.desktop x-riscoslibrary.desktop		\
			x-riscosposter.desktop x-riscosvector.desktop

MIMEXML=		drawview-mime.xml

APPLICS =		drawview.desktop

KDEICONS =		@kdeicons@
KDEMIME =		@kdemime@
KDEMIMEDB =		@kdemimedb@
KDEAPPS =		@kdeapps@

DESKTOPAPPS =		@desktopapps@
DESKTOPICONS =		@desktopicons@


EXTRA_DIST =		$(MIMEICONS) $(APPICONS) $(MIMETYPES) $(APPLICS) $(MIMEXML)


install-data-local:	$(MIMEICONS) $(APPICONS) $(MIMETYPES) $(APPLICS) $(MIMEXML)
			p1=`basename $(prefix)`;								\
			d2=`dirname $(prefix)`;									\
			p2=`basename $${d2}`;									\
			if [ \( "$${p1}" = "_inst" \) -a \( "$${p2}" = "$(PACKAGE)-$(VERSION)" \) ];		\
			then											\
				echo "make of 'install-data-local' suppressed for 'distcheck'";			\
			else											\
				if test "$(KDEICONS)" != "";							\
				then										\
					for i in $(MIMEICONS);							\
					do									\
						s=`echo "$${i}" | sed -e 's/^hi//;s/-mime-.*$$//'`;		\
						d="$(DESTDIR)$(KDEICONS)/hicolor/$${s}x$${s}/mimetypes";	\
						$(INSTALL) -d $${d};						\
						b=`echo "$${i}" | sed -e 's/^hi..-mime-//'`;			\
						$(INSTALL) $(srcdir)/$${i} $${d}/$${b};				\
					done;									\
					for i in $(APPICONS);							\
					do									\
						s=`echo "$${i}" | sed -e 's/^hi//;s/-app-.*$$//'`;		\
						d="$(DESTDIR)$(KDEICONS)/hicolor/$${s}x$${s}/apps";		\
						$(INSTALL) -d $${d};						\
						b=`echo "$${i}" | sed -e 's/^hi..-app-//'`;			\
						$(INSTALL) $(srcdir)/$${i} $${d}/$${b};				\
					done;									\
				else										\
					true;									\
				fi;										\
				if test "$(KDEMIME)" != "";							\
				then										\
					d="$(DESTDIR)$(KDEMIME)/application";					\
					$(INSTALL) -d $${d};							\
					for i in $(MIMETYPES);							\
					do									\
						$(INSTALL) $(srcdir)/$${i} $${d};				\
					done;									\
				else										\
					true;									\
				fi;										\
				if test "$(KDEMIMEDB)" != "";							\
				then										\
					d="$(DESTDIR)$(KDEMIMEDB)";						\
					$(INSTALL) -d $${d};							\
					$(INSTALL) -m 644 $(srcdir)/$(MIMEXML) $${d};				\
				else										\
					true;									\
				fi;										\
				if test "$(KDEAPPS)" != "";							\
				then										\
					d="$(DESTDIR)$(KDEAPPS)";						\
					$(INSTALL) -d $${d};							\
					for i in $(APPLICS);							\
					do									\
						$(INSTALL) $(srcdir)/$${i} $${d};				\
					done;									\
				else										\
					true;									\
				fi;										\
				if test "$(DESKTOPICONS)" != "";						\
				then										\
					for i in $(APPICONS);							\
					do									\
						s=`echo "$${i}" | sed -e 's/^hi//;s/-app-.*$$//'`;		\
						d="$(DESTDIR)$(DESKTOPICONS)/hicolor/$${s}x$${s}/apps";		\
						$(INSTALL) -d $${d};						\
						b=`echo "$${i}" | sed -e 's/^hi..-app-//'`;			\
						$(INSTALL) $(srcdir)/$${i} $${d}/$${b};				\
					done;									\
				else										\
					true;									\
				fi;										\
				if test "$(DESKTOPAPPS)" != "";							\
				then										\
					d="$(DESTDIR)$(DESKTOPAPPS)";						\
					$(INSTALL) -d $${d};							\
					for i in $(APPLICS);							\
					do									\
						$(INSTALL) $(srcdir)/$${i} $${d};				\
					done;									\
				else										\
					true;									\
				fi;	\
			fi


uninstall-local:;	if test "$(KDEICONS)" != "";							\
			then										\
				for i in $(MIMEICONS);							\
				do									\
					s=`echo "$${i}" | sed -e 's/^hi//;s/-mime-.*$$//'`;		\
					d="$(DESTDIR)$(KDEICONS)/hicolor/$${s}x$${s}/mimetypes";	\
					b=`echo "$${i}" | sed -e 's/^hi..-mime-//'`;			\
					$(RM) $${d}/$${b};						\
				done;									\
				for i in $(APPICONS);							\
				do									\
					s=`echo "$${i}" | sed -e 's/^hi//;s/-app-.*$$//'`;		\
					d="$(DESTDIR)$(KDEICONS)/hicolor/$${s}x$${s}/apps";		\
					b=`echo "$${i}" | sed -e 's/^hi..-app-//'`;			\
					$(RM) $${d}/$${b};						\
				done;									\
			else										\
				true;									\
			fi;										\
			if test "$(KDEMIME)" != "";							\
			then										\
				d="$(DESTDIR)$(KDEMIME)/application";					\
				for i in $(MIMETYPES);							\
				do									\
					$(RM) $${d}/$${i};						\
				done;									\
			else										\
				true;									\
			fi;										\
			if test "$(KDEAPPS)" != "";							\
			then										\
				d="$(DESTDIR)$(KDEAPPS)";						\
				for i in $(APPLICS);							\
				do									\
					$(RM) $${d}/$${i};						\
				done;									\
			else										\
				true;									\
			fi;										\
			if test "$(DESKTOPICONS)" != "";						\
			then										\
				for i in $(APPICONS);							\
				do									\
					s=`echo "$${i}" | sed -e 's/^hi//;s/-app-.*$$//'`;		\
					d="$(DESTDIR)$(DESKTOPICONS)/hicolor/$${s}x$${s}/apps";		\
					b=`echo "$${i}" | sed -e 's/^hi..-app-//'`;			\
					$(RM) $${d}/$${b};						\
				done;									\
			else										\
				true;									\
			fi;										\
			if test "$(DESKTOPAPPS)" != "";							\
			then										\
				d="$(DESTDIR)$(DESKTOPAPPS)";						\
				for i in $(APPLICS);							\
				do									\
					$(RM) $${d}/$${i};						\
				done;									\
			else										\
				true;									\
			fi
