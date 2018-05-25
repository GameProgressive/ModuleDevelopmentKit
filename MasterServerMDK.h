/*
	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at

	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
*/
#ifndef _GAMEPROGRESSIVE_MASTERSERVER_MDK_H_
#define _GAMEPROGRESSIVE_MASTERSERVER_MDK_H_

/* Exports DLL for Windows */
#if _WIN32 && _MSC_VER
	#if GPMS_EXPORT
		#define GPMSAPI __declspec(dllexport)
	#else
		#define GPMSAPI __declspec(dllimport)
	#endif
#else
	#define GPMSAPI 
#endif

/* Replacement for stdint (bundeled) */
#include "msstdint.h"

/* Required runtime inclusions */
#include <string>
#include <vector>
#include <map>

#ifndef _WIN32
	#include <sys/types.h>
	#include <unistd.h>
#else
	#include <BaseTsd.h>
	typedef SSIZE_T ssize_t;
	#define _SSIZE_T_DEFINED 1
#endif

/* Structs */
typedef void* mdk_client;
typedef void* mdk_loop;
typedef void* mdk_tcp;
typedef void* mdk_udp;
typedef void* mdk_mysql;

#include "Log.h"
#include "MSString.h"
#include "Server.h"
#include "Query.h"
#include "Database.h"

/* Module definition */
typedef std::map<std::string, std::string> ModuleConfigMap;

typedef struct SModuleMain
{
	char *ip;
	int port;
	ModuleConfigMap cfg;
	mdk_mysql* mysql;
} ModuleMain;

enum ErrorCodes
{
	ERROR_NONE = 0,
	ERROR_UNKNOWN,
	ERROR_MYSQL_POINTER,
	ERROR_BIND_ERROR,
	
	/* RESERVED FOR WINDOWS */
	ERROR_STILL_ALIVE = 259,
};

#endif
