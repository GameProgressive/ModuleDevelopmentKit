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
#define LOG_INFO(name, fmt, ...) CmdLoginInfoDisplay(name, "INFO", fmt, ##__VA_ARGS__)

// Display an error log
#define LOG_ERROR(name, fmt, ...) CmdLoginInfoDisplay(name, "ERROR", fmt, ##__VA_ARGS__)

// Display a warning log
#define LOG_WARN(name, fmt, ...) CmdLoginInfoDisplay(name, "WARN", fmt, ##__VA_ARGS__)

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
