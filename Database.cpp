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
#include "Database.h"

#include "Utility.h"

#ifdef __MARIADB__
	#include <mysql.h>
#endif
#ifdef __SQLITE__
	#include <sqlite3.h>
#endif

#if !defined(__MARIADB__) && !defined(__SQLITE__)
	#error "Please change your configuration and choose at least one database engine!"
#endif

MDKDLLAPI CDatabase::CDatabase()
{
#ifdef __MARIADB__
	m_eDatabasetype = DATABASE_TYPE_MARIADB;
#elif defined(__SQLITE__)
	m_eDatabasetype = DATABASE_TYPE_SQLITE;
#endif
	m_Pointed_db = NULL;
}

MDKDLLAPI CDatabase::~CDatabase()
{
	Disconnect();
}

MDKDLLAPI bool CDatabase::Connect(EDatabaseType type, const char *host, int port, const char *username, const char *database_name, const char *password)
{
	m_eDatabasetype = type;
	
#ifdef __MARIADB__
	if (m_eDatabasetype == DATABASE_TYPE_MARIADB)
	{
		if (!m_Pointed_db)
		{
			MYSQL* connection1 = NULL, *connection2 = NULL;
			connection2 = mysql_init(connection1);
			
			if (!connection1)
				m_Pointed_db = (mdk_database)connection2;
			else
				m_Pointed_db = (mdk_database)connection1;
		}

		if (!mysql_real_connect((MYSQL*)m_Pointed_db, port > 0 ? host : NULL, username, password, database_name, port, port > 0 ? NULL : host, 0))
		{
			LOG_ERROR("Database", "Cannot connect to Database Server. Error: %s\n", mysql_error((MYSQL*)m_Pointed_db));
			return false;
		}
	}
#endif
#ifdef __SQLITE__
	if (m_eDatabasetype == DATABASE_TYPE_SQLITE)
	{
		if (sqlite3_open_v2(database_name, (sqlite3**)&m_Pointed_db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL)) {
			LOG_ERROR("Database", "Cannot open Database. Error: %s\n", sqlite3_errmsg((sqlite3*)m_Pointed_db));
			return false;
		}
	}
#endif

	return true;
}

MDKDLLAPI void CDatabase::Disconnect()
{
#ifdef __MARIADB__
	if ((m_Pointed_db) && m_eDatabasetype == DATABASE_TYPE_MARIADB)
		mysql_close((MYSQL*)m_Pointed_db);
#endif
#ifdef __SQLITE__
	if ((m_Pointed_db) && m_eDatabasetype == DATABASE_TYPE_SQLITE)
		sqlite3_close((sqlite3*)m_Pointed_db);
#endif
	m_Pointed_db = NULL;
}

MDKDLLAPI bool CDatabase::IsConnected()
{
	if (!m_Pointed_db)
		return false;

#ifdef __MARIADB__
	if (m_eDatabasetype == DATABASE_TYPE_MARIADB)
	{
		if (mysql_stat((MYSQL*)m_Pointed_db) == NULL)
			return false;
	}
#endif

	// SQLite dosen't need this code because the database is always "connected" thanks to the nature
	// of SQLite, the file is always opened until you close it
	
	return true;
}
