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
#ifndef _MODULEDEVELOPMENTKIT_MDK_DEFINITIONS_H_
#define _MODULEDEVELOPMENTKIT_MDK_DEFINITIONS_H_

#include "mdkstdint.h"

// C++ Runtime Inclusions
#include <string>

/* Exports DLL for Windows */
#if _WIN32 && _MSC_VER
	#if MDK_EXPORT_FUNCTIONS
		#define MDKDLLAPI __declspec(dllexport)
	#else
		#define MDKDLLAPI __declspec(dllimport)
	#endif
#else
	#define MDKDLLAPI 
#endif

/* ssize_t definition */
#ifndef _WIN32
	#include <sys/types.h>
	#include <unistd.h>
#else
	#include <BaseTsd.h>
	typedef SSIZE_T ssize_t;
	#define _SSIZE_T_DEFINED 1
#endif


#endif
