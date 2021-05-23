#########################################################################
#  Project:		DrawView - Decompression filter			#
#  Edit:		22-May-21					#
#########################################################################

project(drawview)

#########################################################################
#									#
#  Building the target							#
#									#
#########################################################################

set(drawfilter_SRCS
  main.c
  lzw.c
  riscoscompat.c
  ../utils/cmdutils.c
  ../riscos/draw/decompress.c
)

add_definitions(-DDRAWFILTER)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../riscos)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../utils)

add_executable(drawfilter ${drawfilter_SRCS})
target_link_libraries(drawfilter
  dl
)

#########################################################################
#									#
#  Installation								#
#									#
#########################################################################

if (INSTALL_BINARIES)
  install(TARGETS drawfilter DESTINATION ${KDE_INSTALL_LIBEXECDIR})
endif (INSTALL_BINARIES)
