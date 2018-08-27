# - Try to find MariaDB.
# Once done this will define:
# MARIADB_FOUND			- If false, do not try to use MariaDB.
# MARIADB_INCLUDE_DIRS	- Where to find mysql.h, etc.
# MARIADB_LIBRARIES		- The libraries to link against.
# MARIADB_VERSION_STRING	- Version in a string of MariaDB.
#
# Created by RenatoUtsch based on eAthena implementation.
#
# Please note that this module only supports Windows and Linux officially, but
# should work on all UNIX-like operational systems too.
#

#=============================================================================
# Copyright 2012 RenatoUtsch
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

if( WIN32 )
	find_path( MARIADB_INCLUDE_DIR
		NAMES "mysql.h"
		PATHS "$ENV{PROGRAMFILES}/MariaDB/*/include"
			  "$ENV{PROGRAMFILES(x86)}/MariaDB/*/include"
			  "$ENV{SYSTEMDRIVE}/MariaDB/*/include" )
	
	find_library( MARIADB_LIBRARY
		NAMES "mariadb"
		PATHS "$ENV{PROGRAMFILES}/MariaDB/*/lib"
			  "$ENV{PROGRAMFILES(x86)}/MariaDB/*/lib"
			  "$ENV{SYSTEMDRIVE}/MariaDB/*/lib" )
else()
	find_path( MARIADB_INCLUDE_DIR
		NAMES "mysql.h"
		PATHS "/usr/include/mariadb"
			  "/usr/local/include/mariadb"
			  "/usr/mysql/include/mariadb" )
	
	find_library( MARIADB_LIBRARY
		NAMES "mariadb"
		PATHS "/lib/mysql"
			  "/lib64/mysql"
			  "/usr/lib/mysql"
			  "/usr/lib64/mysql"
			  "/usr/local/lib/mysql"
			  "/usr/local/lib64/mysql"
			  "/usr/mysql/lib/mysql"
			  "/usr/mysql/lib64/mysql" )
endif()

if( MARIADB_INCLUDE_DIR AND EXISTS "${MARIADB_INCLUDE_DIR}/mariadb_version.h" )
	file( STRINGS "${MARIADB_INCLUDE_DIR}/mariadb_version.h"
		MARIADB_VERSION_H REGEX "^#define[ \t]+MARIADB_CLIENT_VERSION_STR[ \t]+\"[^\"]+\".*$" )
	string( REGEX REPLACE
		"^.*MARIADB_CLIENT_VERSION_STR[ \t]+\"([^\"]+)\".*$" "\\1" MARIADB_VERSION_STRING
		"${MARIADB_VERSION_H}" )    
endif()

# handle the QUIETLY and REQUIRED arguments and set MARIADB_FOUND to TRUE if
# all listed variables are TRUE
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( MARIADB
	REQUIRED_VARS	MARIADB_LIBRARY MARIADB_INCLUDE_DIR
	VERSION_VAR		MARIADB_VERSION_STRING )

set( MARIADB_INCLUDE_DIRS ${MARIADB_INCLUDE_DIR} )
set( MARIADB_LIBRARIES ${MARIADB_LIBRARY} )

mark_as_advanced( MARIADB_INCLUDE_DIR MARIADB_LIBRARY )
