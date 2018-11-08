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

static bool bIsConnecting = false;

#ifdef ENABLE_MYSQL_SUPPORT
	#include "centosworkaround.h"
	#include <amy.hpp>
	
	static void amy_handle_connect(AMY_SYSTEM_NS::error_code const& ec)
	{
		bIsConnecting = false;
	}
#endif

#ifdef ENABLE_SQLITE3_SUPPORT
	#include <sqlite3.h>
#endif

#if !defined(ENABLE_MYSQL_SUPPORT) && !defined(ENABLE_SQLITE3_SUPPORT)
	#error "Please change your configuration and choose at least one database engine!"
#endif

MDKDLLAPI CDatabase::CDatabase()
{
#ifdef ENABLE_MYSQL_SUPPORT
	m_eDatabasetype = DATABASE_TYPE_MYSQL;
#elif defined(ENABLE_SQLITE3_SUPPORT)
	m_eDatabasetype = DATABASE_TYPE_SQLITE;
#endif

	bIsConnecting = false;
	m_Pointed_db = NULL;
}

MDKDLLAPI CDatabase::~CDatabase()
{
	Disconnect();
}

MDKDLLAPI bool CDatabase::Connect(void* io_service, EDatabaseType type, const char *host, int port, const char *username, const char *database_name, const char *password)
{
	m_eDatabasetype = type;
	bIsConnecting = false;

#ifdef ENABLE_MYSQL_SUPPORT
	if (m_eDatabasetype == DATABASE_TYPE_MYSQL)
	{
		AMY_ASIO_NS::io_service* io = (AMY_ASIO_NS::io_service*)io_service;
		AMY_ASIO_NS::ip::tcp::endpoint ep;
		amy::connector* connector = new amy::connector(*io);

		ep = AMY_ASIO_NS::ip::tcp::endpoint(AMY_ASIO_NS::ip::address::from_string(host), port);
				
		connector->async_connect(ep, amy::auth_info(username, password), database_name, amy::default_flags, std::bind(amy_handle_connect, amy::placeholders::error));	
		bIsConnecting = true;
		m_Pointed_db = connector;
	}
#endif

#ifdef ENABLE_SQLITE3_SUPPORT
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
	bIsConnecting = false;
	
#ifdef ENABLE_MYSQL_SUPPORT
	if ((m_Pointed_db) && m_eDatabasetype == DATABASE_TYPE_MYSQL)
	{
		amy::connector* connector = (amy::connector*)m_Pointed_db;
		delete connector;
	}
#endif

#ifdef ENABLE_SQLITE3_SUPPORT
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
	if (m_eDatabasetype == DATABASE_TYPE_MYSQL)
	{
		if (mysql_stat((MYSQL*)m_Pointed_db) == NULL)
			return false;
	}
#endif

	// SQLite dosen't need this code because the database is always "connected" thanks to the nature
	// of SQLite, the file is always opened until you close it
	
	return true;
}

MDKDLLAPI bool CDatabase::IsConnecting()
{
	return bIsConnecting;
}

	