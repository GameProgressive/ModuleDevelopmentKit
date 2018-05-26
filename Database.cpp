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
#include "Database.h"
#include "MSConfig.h"

#include <stdio.h>
#include <mysql.h>

GPMSAPI bool Database::Connect(mdk_mysql* mysql)
{
	char *pass = (char*)CConfig::GetDatabasePassword();
	char *sock = (char*)CConfig::GetDatabaseSocket();
	
	if (strlen(pass) < 1)
		pass = NULL;

	if (strlen(sock) < 1)
		sock = NULL;

	if (!mysql_real_connect((MYSQL*)mysql, sock ? NULL : CConfig::GetDatabaseHost(), CConfig::GetDatabaseUsername(),
		pass, CConfig::GetDatabaseName(), CConfig::GetDatabasePort(), sock, 0))
	{
		LOG_ERROR("Database", "Cannot connect to MySQL Server. Error: %s\n", mysql_error(mysql));
		return false;
	}

	return true;
}

GPMSAPI void Database::Disconnect(mdk_mysql *mysql)
{
	if (mysql)
		mysql_close((MYSQL*)mysql);
	
	mysql = NULL;
}


GPMSAPI std::string Database::EscapeSQLString(mdk_mysql* con, std::string str)
{
	char *x = (char*)malloc(sizeof(char)*(str.length()*2+5));
	if (!x)
		return "";
	
	mysql_real_escape_string((MYSQL*)con, x, str.c_str(), str.length());
	std::string k = std::string(x);
	
	free(x);
	return k;
}

GPMSAPI void Database::EscapeSQLString(mdk_mysql* c, std::string &str)
{
	str = EscapeSQLString(c, str.c_str());
}

GPMSAPI bool Database::RunDBQuery(mdk_mysql* con, std::string str)
{
	if (mysql_query((MYSQL*)con, str.c_str()) != 0)
	{
		LOG_ERROR("Database", "Cannot execute query. Error: %s\n", mysql_error((MYSQL*)con));
		return false;		
	}
	
	return true;
}

GPMSAPI bool Database::RunDBQuery(mdk_mysql *con, std::string query, ResultSet **rs)
{
	*rs = new ResultSet();
	if (!(*rs)->executeQuery(con, query))
	{
		delete rs;

		LOG_ERROR("Database", "Cannot execute query. Error: %s\n",  mysql_error((MYSQL*)con));
		return false;		
	}

	return true;
}

GPMSAPI bool Database::IsConnected(mdk_mysql *con)
{
	if (mysql_stat(con) == NULL)
		return false;
	
	return true;
}
