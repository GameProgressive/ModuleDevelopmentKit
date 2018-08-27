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
#ifndef _MODULEDEVELOPMENTKIT_UTILITY_H_
#define _MODULEDEVELOPMENTKIT_UTILITY_H_

#include "MDK_Definitions.h"

// String processing functions
bool MDKDLLAPI strendswith(const char *buffer, const char *what);
bool MDKDLLAPI strdelete(char *buffer, int buffersize, int number);
void MDKDLLAPI strrand(char *buffer, int buffersize, const char *table);
bool MDKDLLAPI strfind(char *buffer, const char what);
bool MDKDLLAPI strstartswith(const char *buffer, const char *what);

// Display an information log
#define LOG_INFO(name, fmt, ...) CmdLoginInfoDisplay(name, "INFO", fmt, __VA_ARGS__)

// Display an error log
#define LOG_ERROR(name, fmt, ...) CmdLoginInfoDisplay(name, "ERROR", fmt, __VA_ARGS__)

// Display a warning log
#define LOG_WARN(name, fmt, ...) CmdLoginInfoDisplay(name, "WARN", fmt, __VA_ARGS__)

/*
	This functions are use to display information on the console
	The output follow C string formatting, and it display he following:
	[name] [type] >> fmt

	Example call: LOG_INFO("Database", "Unable to connect to %d", 98)
	This will write into the console the following text:
	[Database] [INFO] >> Unable to connect to 98
*/  
void MDKDLLAPI CmdLoginInfoDisplay(const char *name, const char *type, const char *fmt, ...);

#endif
