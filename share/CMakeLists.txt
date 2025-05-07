#########################################################################
#  Project:		DrawView - Support files			#
#  Edit:		07-May-25					#
#########################################################################

project(drawview)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

install(FILES fontmap DESTINATION ${KDE_INSTALL_DATADIR}/${PN}/share)
install(FILES modes DESTINATION ${KDE_INSTALL_DATADIR}/${PN}/share)
install(FILES palette DESTINATION ${KDE_INSTALL_DATADIR}/${PN}/share)
