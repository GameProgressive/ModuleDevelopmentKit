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
project("MasterServerMDK")

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
include_directories("include")


# Mariadb inclusion
set(WITH_UNIT_TESTS OFF) # Remove useless tests from mariadb build
add_subdirectory(mariadb-connector-c)
include_directories("mariadb-connector-c/include")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/mariadb-connector-c/include") # mariadb config

set(SOURCES
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/MasterServerMDK.h
	${CMAKE_CURRENT_LIST_DIR}/Query.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/Query.h
	${CMAKE_CURRENT_LIST_DIR}/MSString.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/MSString.h
	${CMAKE_CURRENT_LIST_DIR}/Server.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/Server.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/msstdint.h
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/Log.h
	${CMAKE_CURRENT_LIST_DIR}/Log.cpp
	${CMAKE_CURRENT_LIST_DIR}/Database.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/MDK/Database.h
)

if (WIN32)
	set(SOURCES
		${SOURCES}
		${CMAKE_CURRENT_LIST_DIR}/resource.h
	)
	set_source_files_properties(${CMAKE_CURRENT_LIST_DIR}/Common.rc PROPERTIES LANGUAGE RC)
endif()

add_library(MDK SHARED ${SOURCES})
target_link_libraries(MDK uv libmariadb)
