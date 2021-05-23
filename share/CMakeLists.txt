#########################################################################
#  Project:		DrawView - Support files			#
#  Edit:		22-May-21					#
#########################################################################

project(drawview)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

install(FILES fontmap DESTINATION ${KDE_INSTALL_DATADIR}/drawview/share)
install(FILES modes DESTINATION ${KDE_INSTALL_DATADIR}/drawview/share)
install(FILES palette DESTINATION ${KDE_INSTALL_DATADIR}/drawview/share)
