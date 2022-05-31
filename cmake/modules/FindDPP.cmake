# This module defines
# DPP_FOUND: if false, do not try to link to DPP
# DPP_LIBRARY_DIR: path to the DPP libraries
# DPP_LIBRARY: path to the DPP library file
# DPP_INCLUDE_DIR: where to find dpp.h

message("<FindDPP.cmake>")

SET(DPP_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local/opt
	/usr/local
	/usr/local/lib/
		/usr/local/include
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${DPP_PATH}
	${DPP_ROOT_DIR}
)

FIND_PATH(DPP_INCLUDE_DIR dpp.h
	HINTS
	$ENV{DPP_PATH}
	$ENV{DPP_ROOT_DIR}
	PATH_SUFFIXES include/dpp include
	PATHS ${DPP_SEARCH_PATHS}
)

FIND_LIBRARY(DPP_LIBRARY
	NAMES dpp
	HINTS
	$ENV{DPP_PATH}
	$ENV{DPP_ROOT_DIR}
	PATH_SUFFIXES lib64 lib
	PATHS ${DPP_SEARCH_PATHS}
)

message("DPP_INCLUDE_DIR: ${DPP_INCLUDE_DIR}; DPP_LIBRARY: ${DPP_LIBRARY}")

get_filename_component(DPP_LIBRARY_DIR ${DPP_LIBRARY} DIRECTORY)

message("</FindDPP.cmake>")

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(DPP REQUIRED_VARS DPP_LIBRARY DPP_LIBRARY_DIR DPP_INCLUDE_DIR)