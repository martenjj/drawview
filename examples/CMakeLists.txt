#########################################################################
#  Project:		DrawView - Examples				#
#  Edit:		22-May-21					#
#########################################################################

project(drawview)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

set(drawview_EXAMPLES
  README
  Ttnkhamun.c56
  Metro.c56
  6ominoes.aff
  Viking.c56
  Backdrop.aff
  Barcelona.c56
  brainVmind.c56
  compass.c56
  Spitfire.c4b
  Hurricane.c4b
  Coupons.c56
  impossible.aff
  MultiPlay.c4b
  GraphicEq.c4b
  AllCaps.vec
  AllJoins.vec
)

if (INSTALL_EXAMPLES)
  install(FILES ${drawview_EXAMPLES} DESTINATION ${KDE_INSTALL_DATADIR}/drawview/examples)
endif (INSTALL_EXAMPLES)
