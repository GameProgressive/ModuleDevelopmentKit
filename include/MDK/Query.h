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
#ifndef _MODULEDEVELOPMENTKIT_QUERY_H_
#define _MODULEDEVELOPMENTKIT_QUERY_H_

#include "Database.h"

#include <vector>

/**
	This class rappresents the result data of a SELECT or COUNT query.

	1) Execute the query with your database pointer
	2) Go to the first line and start parsing the data

	You can use Get*FromRow() to get the data the row you selected
	The index is the colum number of your query.

	Make sure to match each value type with each coulmn.
	(Don't try to read a int from a string data)
*/
class CResultSet
{
public:
	MDKDLLAPI CResultSet();
	MDKDLLAPI ~CResultSet();

	MDKDLLAPI bool ExecuteQuery(CDatabase* db, std::string str);

	MDKDLLAPI unsigned int GetUIntFromRow(size_t index);
	MDKDLLAPI std::string GetStringFromRow(size_t index);
	MDKDLLAPI int GetIntFromRow(size_t index);
	MDKDLLAPI double GetDoubleFromRow(size_t index);

	MDKDLLAPI size_t GetTotalRows();

	MDKDLLAPI bool GotoFirstRow();
	MDKDLLAPI bool GotoNextRow();
	MDKDLLAPI bool GotoRow(size_t row);

private:
	std::vector<std::vector<std::string>> m_vvRows;
	size_t m_uiPos;
};

bool MDKDLLAPI mdk_only_run_query(CDatabase* db, std::string query);
bool MDKDLLAPI mdk_escape_query_string(CDatabase* db, std::string& escapeString);

#endif
