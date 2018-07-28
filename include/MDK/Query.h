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
