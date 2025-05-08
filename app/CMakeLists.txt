#########################################################################
#  Project:		DrawView - Application				#
#  Edit:		08-May-25					#
#########################################################################

project(drawview)

#########################################################################
#									#
#  Dependencies								#
#									#
#########################################################################

find_package(Qt${QV} ${QT_MIN_VERSION} REQUIRED COMPONENTS Core Gui Widgets PrintSupport Svg DBus)
find_package(KF${QV} ${KF_MIN_VERSION} REQUIRED COMPONENTS I18n CoreAddons XmlGui Auth Crash IconThemes)

#########################################################################
#									#
#  Rigourousness							#
#									#
#########################################################################

add_definitions("-DQT_USE_FAST_CONCATENATION")
add_definitions("-DQT_USE_FAST_OPERATOR_PLUS")
add_definitions("-DQT_NO_CAST_FROM_BYTEARRAY")
add_definitions("-DQT_NO_URL_CAST_FROM_STRING")
add_definitions("-DQT_NO_CAST_TO_ASCII")
add_definitions("-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT")
add_definitions("-DQT_DEPRECATED_WARNINGS")

remove_definitions("-DQT_NO_CAST_FROM_ASCII")
remove_definitions("-DQT_NO_SIGNALS_SLOTS_KEYWORDS")
remove_definitions("-DQT_NO_KEYWORDS")

#########################################################################
#									#
#  Logging								#
#									#
#########################################################################

ecm_qt_declare_logging_category(drawview_LOG_SRCS
  HEADER "drawview_debug.h"
  IDENTIFIER "DRAWVIEW_LOG"
  CATEGORY_NAME "drawview"
  EXPORT drawviewlogging
  DESCRIPTION "application (DrawView)")

#########################################################################
#									#
#  Building the target							#
#									#
#########################################################################

set(drawview_SRCS
  main.cpp
  drawview.cpp
  paper.cpp
  drawwidget.cpp
  diagram.cpp
  object.cpp
  coord.cpp
  reader.cpp
  error.cpp
  misc.cpp
  fonts.cpp
  files.cpp
  paintoptions.cpp
  spritereader.cpp
  palette.cpp
  unknownobject.cpp
  pathobjects.cpp
  groupobject.cpp
  taggedobject.cpp
  libraryobject.cpp
  textobjects.cpp
  spriteobjects.cpp
  textarea.cpp
  replicates.cpp
  radiate.cpp
  ${drawview_LOG_SRCS}
)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

# These version conditionals are assumed to be comulative
add_definitions("-DQT4")
if (${QV} GREATER_EQUAL 5)
  add_definitions("-DQT5")
  add_definitions("-DKF5")
endif ()
if (${QV} GREATER_EQUAL 6)
  add_definitions("-DQT6")
  add_definitions("-DKF6")
endif ()

add_definitions("-DHAVE_VERSION_H")
add_definitions("-DHAVE_MATH_H")

add_definitions("-DPACKAGE=\"${CMAKE_PROJECT_NAME}\"")
add_definitions("-DPACKAGE_VERSION=\"${VERSION}\"")
add_definitions("-DPACKAGE_NAME=\"DrawView\"")
add_definitions("-DPACKAGE_URL=\"http://www.keelhaul.me.uk/acorn/drawview\"")
add_definitions("-DPACKAGE_BUGREPORT=\"jjm@keelhaul.me.uk\"")
add_definitions("-DDRAWFILTER_DIR=\"${KDE_INSTALL_FULL_LIBEXECDIR}\"")
add_definitions("-DPROJECT_NAME=\"${PN}\"")

add_executable(drawview ${drawview_SRCS})
add_dependencies(drawview vcsversion)
target_link_libraries(drawview
  Qt${QV}::Core
  Qt${QV}::Gui
  Qt${QV}::Widgets
  Qt${QV}::PrintSupport
  Qt${QV}::Svg
  KF${QV}::I18n
  KF${QV}::CoreAddons
  KF${QV}::XmlGui
  KF${QV}::Crash
  KF${QV}::IconThemes
)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

if (INSTALL_BINARIES)
  install(TARGETS drawview ${INSTALL_TARGETS_DEFAULT_ARGS})
endif (INSTALL_BINARIES)

ecm_qt_install_logging_categories(
  EXPORT drawviewlogging
  FILE drawview.categories
  DESTINATION "${KDE_INSTALL_LOGGINGCATEGORIESDIR}")
