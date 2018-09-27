/*
	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _MODULEDEVELOPMENTKIT_MODULENTRYPOINT_H_
#define _MODULEDEVELOPMENTKIT_MODULENTRYPOINT_H_

/* Required runtime inclusions */
#include "ThreadServer.h"

enum EErrorCodes
{
	ERROR_NONE = 0,
	ERROR_UNKNOWN,
	ERROR_BIND_ERROR,
	ERROR_DATABASE_ERROR,
	ERROR_CONFIGURATION_ERROR,
	
	ERROR_STILL_ALIVE = -1,
};

#if _WIN32 && _MSC_VER
	#define DECL_DLLEXPORT  __declspec(dllexport)
#else
	#define DECL_DLLEXPORT
#endif

#ifdef __cplusplus
	#define CPP_EXTERN_C_BEGIN extern "C" {
	#define CPP_EXTERN_C_END }
#else
	#define CPP_EXTERN_C_BEGIN
	#define CPP_EXTERN_C_END
#endif

#define ModuleEntryPoint(class, default_port, udp) CPP_EXTERN_C_BEGIN DECL_DLLEXPORT CThreadServer* MDKModule() { return new class(default_port, udp); } CPP_EXTERN_C_END

#endif
