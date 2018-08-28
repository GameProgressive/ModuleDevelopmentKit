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
#include "Query.h"

#include "Utility.h"

#ifdef __MARIADB__
#include <mysql.h> /*Still using MariaDB Connector C*/
#endif
#ifdef __SQLITE__
#include <sqlite3.h>
#endif

#include <stdio.h>

#ifdef __SQLITE__
int _sqlite3_query_exec_callback(void* user, int, char**, char**)
{
	return 0;
}
#endif


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
#ifdef __SQLITE__
	if (db->GetDatabaseType() == DATABASE_TYPE_SQLITE)
	{
		sqlite3_stmt* stmt = NULL;
		
		if (sqlite3_prepare_v2((sqlite3*)db->GetDatabasePointer(), str.c_str(), str.length(), &stmt, NULL) != SQLITE_OK)
		{
			LOG_ERROR("Query", "Cannot execute query. Error: %s\n", sqlite3_errmsg((sqlite3*)db->GetDatabasePointer()));
			return false;
		}
		

		fields = sqlite3_column_count(stmt);
		if (fields < 0)
		{
			sqlite3_finalize(stmt);
			return false;
		}
		
		if (sqlite3_step(stmt) != SQLITE_ROW)
		{ // No rows
			sqlite3_finalize(stmt);
			return false;
		}
		
		do
		{
			std::vector<std::string> vec;
			unsigned int i =  0;
			
			for (; i < fields; i++)
				vec.push_back(std::string((const char*)sqlite3_column_text(stmt, i)));
			
			m_vvRows.push_back(vec);
		} while (sqlite3_step(stmt) == SQLITE_ROW);
		
		sqlite3_finalize(stmt);
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
	char *errMsg = NULL;
	
	if (db->GetDatabaseType() == DATABASE_TYPE_SQLITE)
	{
		if (sqlite3_exec((sqlite3*)db->GetDatabasePointer(), query.c_str(), _sqlite3_query_exec_callback, NULL, (char**)&errMsg)!=SQLITE_OK)
		{
			LOG_ERROR("Database", "Cannot execute query. Error: %s\n", errMsg);
			return false;
		}
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

