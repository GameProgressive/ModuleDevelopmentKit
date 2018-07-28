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
#ifndef _MODULEDEVELOPMENTKIT_DATABASE_H_
#define _MODULEDEVELOPMENTKIT_DATABASE_H_

#include "MDK_Definitions.h"

enum EDatabaseType
{
	DATABASE_TYPE_MARIADB = 0,
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
	bool MDKDLLAPI Connect(EDatabaseType type, const char *host, int port, const char *username, const char *database_name, const char *password);
	
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

private:
	mdk_database m_Pointed_db;
	//Instantiation of mdk_database_pointer
	EDatabaseType m_eDatabasetype;
	//Instantiation of EDatabaseType
};

#endif
