#########################################################################
#  Project:		DrawView - Desktop Integration			#
#  Edit:		24-May-21					#
#########################################################################

project(drawview)

include(ECMInstallIcons)

set(drawview_ICONS
  16-mimetypes-file810.png
  16-mimetypes-fileAFF.png
  16-mimetypes-fileBBB.png
  16-mimetypes-fileC4B.png
  16-mimetypes-fileC56.png
  16-mimetypes-fileCC3.png
  32-mimetypes-file810.png
  32-mimetypes-fileAFF.png
  32-mimetypes-fileBBB.png
  32-mimetypes-fileC4B.png
  32-mimetypes-fileC56.png
  32-mimetypes-fileCC3.png
  16-apps-drawview.png
  32-apps-drawview.png
)

set(drawview_MIMETYPES
  x-compressdraw.desktop
  x-riscosdraw.desktop
  x-riscosgridpro.desktop
  x-riscoslibrary.desktop
  x-riscosposter.desktop
  x-riscosvector.desktop
)

set(drawview_MIMEXML
  drawview-mime.xml
)

set(drawview_APPLICS
  drawview.desktop
)

ecm_install_icons(ICONS ${drawview_ICONS} DESTINATION ${KDE_INSTALL_ICONDIR})

install(FILES drawviewui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/drawview)
install(FILES ${drawview_APPLICS} DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES ${drawview_MIMEXML} DESTINATION ${KDE_INSTALL_MIMEDIR})
