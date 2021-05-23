#########################################################################
#  Project:		DrawView					#
#  Edit:		23-May-21					#
#########################################################################
#									#
#									#
#########################################################################

project(drawview)
set(VERSION "2.00")
message(STATUS "Configuring for DrawView version ${VERSION}")

#########################################################################
#									#
#  ECM paths and definitions						#
#									#
#########################################################################

set(QT_MIN_VERSION "5.10.0")
set(KF5_MIN_VERSION "5.68.0")
set(ECM_MIN_VERSION "5.68.0")

find_package(ECM ${ECM_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMSetupVersion)
include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings)
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

add_subdirectory(utils)
add_subdirectory(desktop)
add_subdirectory(filter)
add_subdirectory(app)
add_subdirectory(po)
add_subdirectory(share)
add_subdirectory(examples)

#########################################################################
#									#
#  Information at end of cmake run					#
#									#
#########################################################################

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
