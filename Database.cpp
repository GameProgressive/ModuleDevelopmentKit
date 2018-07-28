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
