# This module defines
# SPDLOG_FOUND: if false, do not try to link to SPDLOG
# SPDLOG_LIBRARY_DIR: path to the SPDLOG libraries
# SPDLOG_LIBRARY: path to the SPDLOG library file
# SPDLOG_INCLUDE_DIR: where to find SPDLOG.h

message("<FindSpdLog.cmake>")

SET(SPDLOG_SEARCH_PATHS
	~/Library/Frameworks
	/Library/Frameworks
	/usr/local/opt
	/usr/local
	/usr/local/lib
	/usr/local/include
	/usr
	/sw # Fink
	/opt/local # DarwinPorts
	/opt/csw # Blastwave
	/opt
	${SPDLOG_PATH}
	${SPDLOG_ROOT_DIR}
)

FIND_PATH(SPDLOG_INCLUDE_DIR spdlog.h
	HINTS
	$ENV{SPDLOG_PATH}
	$ENV{SPDLOG_ROOT_DIR}
	PATH_SUFFIXES include/spdlog include
	PATHS ${SPDLOG_SEARCH_PATHS}
)

FIND_LIBRARY(SPDLOG_LIBRARY
	NAMES spdlog
	HINTS
	$ENV{SPDLOG_PATH}
	$ENV{SPDLOG_ROOT_DIR}
	PATH_SUFFIXES lib64 lib
	PATHS ${SPDLOG_SEARCH_PATHS}
)

message("SPDLOG_INCLUDE_DIR: ${SPDLOG_INCLUDE_DIR}; SPDLOG_LIBRARY: ${SPDLOG_LIBRARY}")

get_filename_component(SPDLOG_LIBRARY_DIR ${SPDLOG_LIBRARY} DIRECTORY)

message("</FindSpdLog.cmake>")

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(SpdLog REQUIRED_VARS SPDLOG_LIBRARY SPDLOG_LIBRARY_DIR SPDLOG_INCLUDE_DIR)