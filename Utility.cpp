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
#define MDK_EXPORT_FUNCTIONS 1 /*Export the methods*/

#include "Utility.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

bool MDKDLLAPI strendswith(const char *buffer, const char *what)
{
	size_t endsize = strlen(what), buffsize = strlen(buffer);
	char *checkbuf = (char *)malloc(sizeof(char) * (endsize + 1));

	for (; endsize > -1; buffsize--, endsize--)
	{
		checkbuf[endsize] =  buffer[buffsize];
	}

	if (strcmp(checkbuf, what) == 0)
	{
		if (checkbuf)
			free(checkbuf);

		return true;
	}

	if (checkbuf)
		free(checkbuf);

	return false;
}

bool MDKDLLAPI strdelete(char *buffer, size_t buffersize, int number)
{
	size_t counter = 0;

	if (buffersize < number)
		return false;

	buffersize -= number;

	for (; counter < buffersize; counter++, number++)
	{
		buffer[counter] = buffer[number];
	}

	buffer[counter] = '\0';

	return true;
}

void MDKDLLAPI strrand(char *buffer, size_t buffersize, const char *table)
{
	size_t table_len = strlen(table), i = 0;

	for (; i < (buffersize - 1); i++)
	{
		int randnum = rand() % (table_len - 1);
		buffer[i] = table[randnum];
	}

	buffer[i] = '\0';
}

bool MDKDLLAPI strfind(char *buffer, const char what)
{
	while (*buffer != '\0')
	{
		if (*buffer == what)
			return true;

		buffer++;
	}

	return false;
}

bool MDKDLLAPI strstartswith(const char* buffer, const char* what)
{
	size_t whatsize = strlen(what), count = 0;
	char *checkbuf = (char *)malloc(sizeof(char) * (whatsize + 1));

	if (whatsize > strlen(buffer))
		return false;
	
	for (; count < whatsize; count++)
	{
		checkbuf[count] = buffer[count];
	}
	checkbuf[count] = '\0';

	if (strcmp(checkbuf, what) == 0)
	{
		if (checkbuf)
			free(checkbuf);

		return true;
	}

	if (checkbuf)
		free(checkbuf);

	return false;
}

void MDKDLLAPI CmdLoginInfoDisplay(const char *name, const char *type, const char *fmt, ...)
{
	va_list vl;
	
	if (!fmt)
		return;
	
	if (name)
		printf("[%s] ", name);
	
	if (type)
		printf("[%s] ", type);
		
	printf(">> ");
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);
	printf("\n");
}
