#	Licensed to the Apache Software Foundation (ASF) under one
#	or more contributor license agreements.  See the NOTICE file
#	distributed with this work for additional information
#	regarding copyright ownership.  The ASF licenses this file
#	to you under the Apache License, Version 2.0 (the
#	"License"); you may not use this file except in compliance
#	with the License.  You may obtain a copy of the License at
#
#	http://www.apache.org/licenses/LICENSE-2.0
#
#	Unless required by applicable law or agreed to in writing,
#	software distributed under the License is distributed on an
#	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#	KIND, either express or implied.  See the License for the
#	specific language governing permissions and limitations
#	under the License.
#
cmake_minimum_required(VERSION 3.0)
project("ModuleDevelopmentKit")

option(WITH_SQLITE "Enable SQLite support" ON)
option(WITH_MARIADB "Enable MariaDB support" ON)

# Add definitions for w32 build that dosen't support _WIN32 macro
if (WIN32)
	add_definitions("-DWIN32 -D__WIN32__")
endif()

# Set -fPIC to allow compiling mdk shared library with libuv
if (NOT WIN32)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()

# Set CMake output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# LibUV inclusions
add_subdirectory(libuv)
include_directories("libuv/include")

# Main MDK inclusions
include_directories("include/MDK")

# SQLite inclusion
if (WITH_SQLITE)  ##*************************************************8
	add_subdirectory(sqlite3)
	add_definitions(-D__SQLITE__)
	include_directories(sqlite3)
endif()

# Mariadb inclusion
if (WITH_MARIADB)
	set(WITH_UNIT_TESTS OFF) # Remove useless tests from mariadb build
	add_subdirectory(mariadb-connector-c)
	include_directories("mariadb-connector-c/include")
	include_directories("${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c/include") # mariadb config
	add_definitions(-D__MARIADB__)
endif()

set(SOURCES
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/Database.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/MDK_Definitions.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/mdkstdint.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/ModuleEntryPoint.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/Utility.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/Query.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/TemplateClient.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/TemplateServer.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/TemplateSocket.h
	${CMAKE_CURRENT_LIST_DIR}/Database.cpp
	${CMAKE_CURRENT_LIST_DIR}/Utility.cpp
	${CMAKE_CURRENT_LIST_DIR}/Query.cpp
	${CMAKE_CURRENT_LIST_DIR}/TemplateClient.cpp
	${CMAKE_CURRENT_LIST_DIR}/TemplateServer.cpp
	${CMAKE_CURRENT_LIST_DIR}/TemplateSocket.cpp
)

if (WIN32)
	set(SOURCES
		${SOURCES}
		${CMAKE_CURRENT_LIST_DIR}/resource.h
	)
	set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/Common.rc PROPERTIES LANGUAGE RC)
endif()

add_library(MDK SHARED ${SOURCES})
target_link_libraries(MDK uv_a libmariadb)

if (WITH_MARIADB)
	target_link_libraries(MDK libmariadb)
endif()
if (WITH_SQLITE)
	target_link_libraries(MDK sqlite3)   ##********************************
endif()
