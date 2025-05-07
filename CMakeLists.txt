#########################################################################
#  Project:		DrawView					#
#  Edit:		07-May-25					#
#########################################################################
#									#
#									#
#########################################################################

project(drawview)
cmake_minimum_required(VERSION 3.6)

if (NOT DEFINED QT_MAJOR_VERSION)
  set(QT_MAJOR_VERSION 6)
endif ()

if (QT_MAJOR_VERSION EQUAL 6)
  set(QT_MIN_VERSION "6.5.0")
  set(KF_MIN_VERSION "6.1.0")
else ()
  if (QT_MAJOR_VERSION EQUAL 5)
    set(QT_MIN_VERSION "5.10.0")
    set(KF_MIN_VERSION "5.68.0")
  else ()
    message(FATAL_ERROR "Expecting to build for either Qt5 or Qt6")
  endif ()
endif ()

set(QV ${QT_MAJOR_VERSION})
set(PN ${CMAKE_PROJECT_NAME})

set(VERSION "${QV}.3.0")
message(STATUS "Configuring for project '${CMAKE_PROJECT_NAME}' version ${VERSION}")

#########################################################################
#									#
#  ECM paths and definitions						#
#									#
#########################################################################

find_package(ECM ${ECM_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(ECMQtDeclareLoggingCategory)

#########################################################################
#									#
#  Options								#
#									#
#########################################################################

option(INSTALL_BINARIES "Install the binaries and libraries, turn off for development in place" ON)
option(INSTALL_EXAMPLES "Install the example files" ON)

#########################################################################
#									#
#  VCS version tracking							#
#									#
#########################################################################

add_custom_target(vcsversion
  ALL
  COMMENT "Checking VCS type and version"
  VERBATIM
  COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/genversion.sh ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${VERSION}
)

#########################################################################
#									#
#  Subdirectories							#
#									#
#########################################################################

add_subdirectory(desktop)
add_subdirectory(filter)
add_subdirectory(app)
add_subdirectory(share)
add_subdirectory(examples)

#########################################################################
#									#
#  Information at end of cmake run					#
#									#
#########################################################################

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
