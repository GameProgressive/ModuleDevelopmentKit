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
