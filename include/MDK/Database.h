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
#ifndef _MODULEDEVELOPMENTKIT_DATABASE_H_
#define _MODULEDEVELOPMENTKIT_DATABASE_H_

#include "MDK_Definitions.h"

enum EDatabaseType
{
	DATABASE_TYPE_MYSQL = 0,
	DATABASE_TYPE_SQLITE = 1,
};

typedef void* mdk_database;

class CDatabase
{
public:
	MDKDLLAPI CDatabase();
	MDKDLLAPI ~CDatabase();

	/**
	Function: Connect
	Description: Connect to a database
	Parameters:
		host => the host to be used to connect
		port => the port to be used to connect
		username => username used to login
		dbname => database name used to login
		password => password to be used (leave NULL for no password)
	Return: true if the connection succeded, otherwise false
	
	NOTE: If port is lesser than 1, the host parametra will be considerated as UNIX Socket
	*/
	bool MDKDLLAPI Connect(void* io_service, EDatabaseType type, const char *host, int port, const char *username, const char *database_name, const char *password);
	
	/**
	Function: Disconnect
	Description: Disconnects from the database
	*/	
	void MDKDLLAPI Disconnect();
	
	/**
	Function: IsConnected
	Description: Check if a connection is still alive
	Return: true if it's connected, otherwise false
	*/
	bool MDKDLLAPI IsConnected();

	inline MDKDLLAPI EDatabaseType GetDatabaseType() { return m_eDatabasetype; }
	inline MDKDLLAPI mdk_database GetDatabasePointer() { return m_Pointed_db; }

	bool MDKDLLAPI IsConnecting();
	
private:
	//Instantiation of mdk_database_pointer
	mdk_database m_Pointed_db;

	//Instantiation of EDatabaseType	
	EDatabaseType m_eDatabasetype;
};

#endif
