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
#include <vector>
#include <map>

#include "Database.h"

/* Module definition */
typedef std::map<std::string, std::string> ModuleConfigMap;

typedef struct SModuleMain
{
	char *ip;
	int port;
	ModuleConfigMap cfg;
	CDatabase* db;
} ModuleMain;

enum EErrorCodes
{
	ERROR_NONE = 0,
	ERROR_UNKNOWN,
	ERROR_MYSQL_POINTER,
	ERROR_BIND_ERROR,
	
	/* Reserved for Windows */
	ERROR_STILL_ALIVE = 259,

	/* Reserved for Linux */
	ERROR_LINUX_UNSUPPORTED_RESERVED = 260,
};

#endif
