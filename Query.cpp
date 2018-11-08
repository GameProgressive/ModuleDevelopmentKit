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

#define check_error(x) if (x) return;

#ifdef ENABLE_MYSQL_SUPPORT
	#include "centosworkaround.h"
	#include <amy.hpp>
#endif

#ifdef ENABLE_SQLITE3_SUPPORT
#include <sqlite3.h>
#endif

#include <stdio.h>

static unsigned int atoui(const char *x)
{
	return (unsigned int)strtol(x, (char **) NULL, 10);
}

#ifdef ENABLE_SQLITE3_SUPPORT
static int sqlite3_query_exec_callback(void* user, int, char**, char**)
{
	return 0;
}
#endif

#ifdef ENABLE_MYSQL_SUPPORT
static void amy_handle_store_result(CDatabase* db, CResultSet* res, AMY_SYSTEM_NS::error_code const& ec, amy::result_set rs, amy::connector*)
{
	check_error(ec);
	res->ProcessResultSet(db, (void*)&rs);
}

static void amy_handle_query(CDatabase* db, CResultSet* res, AMY_SYSTEM_NS::error_code const& ec, amy::connector* connector)
{
	check_error(ec);
	connector->async_store_result(std::bind(amy_handle_store_result, db, res, amy::placeholders::error, amy::placeholders::result_set, connector));
}
#endif

MDKDLLAPI CResultSet::CResultSet()
{
	m_uiPos = 0;
	m_bExecuting = false;
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
#ifdef ENABLE_MYSQL_SUPPORT
	if (db->GetDatabaseType() == DATABASE_TYPE_MYSQL)
	{
		m_bExecuting = true;
		amy::connector* connector = (amy::connector*)db->GetDatabasePointer();
		connector->async_query(str, std::bind(amy_handle_query, db, this, amy::placeholders::error, connector));
	}
#endif
#ifdef ENABLE_SQLITE3_SUPPORT
	if (db->GetDatabaseType() == DATABASE_TYPE_SQLITE)
	{
//#error "Missing Asynchronous support for SQLite3"
	}
#endif

	return true;
}

MDKDLLAPI void CResultSet::ProcessResultSet(CDatabase* db, void* _rs)
{
	// Clear result set
	size_t i = 0;
	
	for (; i < m_vvRows.size(); i++)
	{
		m_vvRows.at(i).clear();
	}
	
	m_vvRows.clear();
	
#ifdef ENABLE_MYSQL_SUPPORT
	if (db->GetDatabaseType() == DATABASE_TYPE_MYSQL)
	{
		amy::result_set* rs = (amy::result_set*)_rs;
		
		for (const auto& row : (*rs))
		{
			std::vector<std::string> entry;
			
			for (i = 0; i < rs->field_count(); rs++)
			{
				entry.push_back(row[i].as<std::string>());
			}
			
			m_vvRows.push_back(entry);
		}
	}
#endif

#ifdef ENABLE_SQLITE3_SUPPORT
	if (db->GetDatabaseType() == DATABASE_TYPE_SQLITE)
	{
//#error "Missing Asynchronous support for SQLite3"
	}
#endif

	m_bExecuting = false;
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

#ifdef ENABLE_MYSQL_SUPPORT
	if (db->GetDatabaseType() == DATABASE_TYPE_MYSQL)
	{
		amy::connector* connector = (amy::connector*)db->GetDatabasePointer();
		connector->query(query);
	}
#endif

#ifdef ENABLE_SQLITE3_SUPPORT
	char *errMsg = NULL;
	
	if (db->GetDatabaseType() == DATABASE_TYPE_SQLITE)
	{
		if (sqlite3_exec((sqlite3*)db->GetDatabasePointer(), query.c_str(), sqlite3_query_exec_callback, NULL, (char**)&errMsg)!=SQLITE_OK)
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
#ifdef ENABLE_MYSQL_SUPPORT
	if (db->GetDatabaseType() == DATABASE_TYPE_MYSQL)
	{
		char *temporanyString = (char*)malloc(sizeof(char)*(inputString.length() * 2 + 5));
		if (!temporanyString)
			return false; // Out of memory, return null string

		mysql_real_escape_string((MYSQL*)db->GetDatabasePointer(), temporanyString, inputString.c_str(), (int)inputString.length());
		inputString = std::string(temporanyString);

		free(temporanyString);
	}
#endif

	return true;	
}

