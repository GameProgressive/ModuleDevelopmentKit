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
#ifndef _GAMEPROGRESSIVE_MDK_DATABASE_H
#define _GAMEPROGRESSIVE_MDK_DATABASE_H

#ifndef _GAMEPROGRESSIVE_MASTERSERVER_MDK_H_
	#error "Please include <MasterServerMDK.h> not this file!"
#endif

class Database
{
public:
	/**
	Function: Connect
	Description: Connec to a MySQL connection
	Parameters:
		mysql => the connection to be started
		host => the host to be used to connect
		port => the port to be used to connect
		username => username used to login
		dbname => database name used to login
		password => password to be used (leave NULL for no password)
	Return: true if the connection succeded, otherwise false
	*/
	GPMSAPI static bool Connect(mdk_mysql mysql, const char *host, int port, const char *username, const char *dbname, const char *password = NULL);

	/**
	Function: Connect
	Description: Connec to a MySQL connection
	Parameters:
		mysql => the connection to be started
		socket => the socket to be used for connect
		username => username used to login
		dbname => database name used to login
		password => password to be used (leave NULL for no password)
	Return: true if the connection succeded, otherwise false
	*/
	GPMSAPI static bool Connect(mdk_mysql mysql, const char* socket, const char *username, const char *dbname, const char* password = NULL);
	
	/**
	Function: Disconnect
	Description: Disconnect a MySQL connection
	Parameters:
		mysql => the connection to be disconnected	
	*/
	GPMSAPI static void Disconnect(mdk_mysql mysql);
	
	/**
	Function: Init
	Description: Initialize a MySQL connection
	Parameters:
		mysql => the connection to be initialized	
	*/
	GPMSAPI static void Init(mdk_mysql* mysql);
	
	/**
	Function: RunDBQuery
	Description: Executes a generic query
	Parameters:
		mysql => the connection to be executed the query
		str => the query
	Return: true if the query was successfully executed, otherwise false
	*/
	GPMSAPI static bool RunDBQuery(mdk_mysql mysql, std::string str);

	/**
	Function: RunDBQuery
	Description: Executes a generic query and store it's result to the passed result res
	Parameters:
		mysql => the connection to be executed the query
		str => the query
		rs => the result set to be filled
	Return: true if the query was successfully executed, otherwise false
	*/
	GPMSAPI static bool RunDBQuery(mdk_mysql mysql, std::string str, ResultSet **rs);

	/**
	Function: EscapeSQLString
	Description: Add escapes from a standard string
	Parameters:
		con => The connection to be used to perform the escaping
		str => The string to be escaped
	Return: Escaped string
	*/
	GPMSAPI static std::string EscapeSQLString(mdk_mysql con, std::string str);

	/**
	Function: EscapeSQLString
	Description: Modify the passed string by adding escapes
	Parameters:
		con => The connection to be used to perform the escaping
		str => The string to be escaped
	*/
	GPMSAPI static void EscapeSQLString(mdk_mysql con, std::string &str);
	
	/**
	Function: IsConnected
	Description: Check if a connection is still alive
	Parameters:
		con => The connection to be checked
	Return: true if it's connected, otherwise false
	*/
	GPMSAPI static bool IsConnected(mdk_mysql con);
};

#endif
