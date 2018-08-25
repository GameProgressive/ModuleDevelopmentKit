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
#define MDK_EXPORT_FUNCTIONS 1 /*Export the methods*/
#include "Query.h"

#include "Utility.h"

#include <mysql.h> /*Still using MariaDB Connector C*/
#include <stdio.h>

MDKDLLAPI CResultSet::CResultSet()
{
	m_uiPos = 0;
}

MDKDLLAPI CResultSet::~CResultSet()
{
	size_t i = 0;
	
	for (; i < m_vvRows.size(); i++)
	{
		m_vvRows.at(i).clear();
	}
	
	m_vvRows.clear();
}

MDKDLLAPI bool CResultSet::ExecuteQuery(CDatabase* db, std::string str)
{
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	unsigned int fields = 0;
	
#ifdef __MARIADB__
	if (db->GetDatabaseType() == DATABASE_TYPE_MARIADB)
	{
		if (mysql_query((MYSQL*)db->GetDatabasePointer(), str.c_str()) != 0)
		{
			LOG_ERROR("Query", "Cannot execute query. Error: %s\n", mysql_error((MYSQL*)db->GetDatabasePointer()));
			return false;		
		}
		
		result = mysql_store_result((MYSQL*)db->GetDatabasePointer());
		if (!result)
		{
			LOG_ERROR("Query", "Cannot execute query. Error: %s\n", mysql_error((MYSQL*)db->GetDatabasePointer()));
			return false;				
		}
		
		if (result->row_count == 0)
		{
			mysql_free_result(result);
			return true;
		}
		
		fields = mysql_num_fields(result);
		if (fields == 0)
		{
			mysql_free_result(result);
			return true;		
		}
		
		row = mysql_fetch_row(result);
		if (row == NULL)
		{
			mysql_free_result(result);
			LOG_ERROR("Query", "Cannot execute query. Error: %s\n", mysql_error((MYSQL*)db));
			return false;
		}
		
		do
		{
			std::vector<std::string> vec;
			unsigned int i =  0;
			
			for (; i < fields; i++)
				vec.push_back(row[i]);
			
			m_vvRows.push_back(vec);
		} while ((row = mysql_fetch_row(result)));
		
		mysql_free_result(result);
	}
#endif

	return true;
}

MDKDLLAPI size_t CResultSet::GetTotalRows()
{
	return m_vvRows.size();
}

MDKDLLAPI bool CResultSet::GotoFirstRow()
{
	if (m_vvRows.size() < 1)
		return false;
	
	m_uiPos = 0;
	return true;
}

MDKDLLAPI bool CResultSet::GotoNextRow()
{
	m_uiPos++;
	
	if (m_vvRows.size() < (m_uiPos+1))
		return false;
	
	return true;
}

MDKDLLAPI bool CResultSet::GotoRow(size_t row)
{
	m_uiPos = row;
	
	if (m_vvRows.size() < m_uiPos)
		return false;
	
	return true;
}

MDKDLLAPI double CResultSet::GetDoubleFromRow(size_t index)
{
	if (m_vvRows.at(m_uiPos).size() < index)
		return 0;
	
	return atof(m_vvRows.at(m_uiPos).at(index).c_str());
}

unsigned int atoui(const char *x)
{
	return (unsigned int)strtol(x, (char **) NULL, 10);
}

MDKDLLAPI unsigned int CResultSet::GetUIntFromRow(size_t index)
{
	if (m_vvRows.at(m_uiPos).size() < index)
		return 0;
	
	return atoui(m_vvRows.at(m_uiPos).at(index).c_str());
}

MDKDLLAPI std::string CResultSet::GetStringFromRow(size_t index)
{
	if (m_vvRows.at(m_uiPos).size() < index)
		return 0;
	
	return m_vvRows.at(m_uiPos).at(index);
}

MDKDLLAPI int CResultSet::GetIntFromRow(size_t index)
{
	if (m_vvRows.at(m_uiPos).size() < index)
		return 0;
	
	return atoi(m_vvRows.at(m_uiPos).at(index).c_str());
}

bool MDKDLLAPI mdk_only_run_query(CDatabase* db, std::string query)
{
	 if (!db->GetDatabasePointer())
		 return false;

#ifdef __MARIADB__
	 if (db->GetDatabaseType() == DATABASE_TYPE_MARIADB)
	 {
		 if (mysql_query((MYSQL*)db->GetDatabasePointer(), query.c_str()) != 0)
		 {
			 LOG_ERROR("Database", "Cannot execute query. Error: %s\n", mysql_error((MYSQL*)db->GetDatabasePointer()));
			 return false;
		 }
	 }
#endif
#ifdef __SQLITE__
	 if (db->GetDatabaseType() == DATABASE_TYPE_SQLITE)
	 {
			//*TODO!
	 }
#endif

	 return true;
}

bool MDKDLLAPI mdk_escape_query_string(CDatabase* db, std::string& inputString)
{
#ifdef __MARIADB__
	if (db->GetDatabaseType() == DATABASE_TYPE_MARIADB)
	{
		char *temporanyString = (char*)malloc(sizeof(char)*(inputString.length() * 2 + 5));
		if (!temporanyString)
			return false; // Out of memory, return null string

		mysql_real_escape_string((MYSQL*)db->GetDatabasePointer(), temporanyString, inputString.c_str(), inputString.length());
		inputString = std::string(temporanyString);

		free(temporanyString);
	}
#endif

	return true;	
}

