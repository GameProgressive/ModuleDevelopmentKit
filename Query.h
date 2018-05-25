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
#ifndef _GAMEPROGRESSIVE_MASTERSERVER_QUERY_H_
#define _GAMEPROGRESSIVE_MASTERSERVER_QUERY_H_

#ifndef _GAMEPROGRESSIVE_MASTERSERVER_MDK_H_
	#error "Please include <MasterServerMDK.h> not this file!"
#endif

class ResultSet
{
public:
GPMSAPI ResultSet();
GPMSAPI ~ResultSet();

GPMSAPI bool executeQuery(mdk_mysql* conn, std::string str);

GPMSAPI bool first();
GPMSAPI unsigned int getUInt(size_t index);
GPMSAPI std::string getString(size_t index);
GPMSAPI int getInt(size_t index);
GPMSAPI double getDouble(size_t index);

GPMSAPI size_t getRows();

GPMSAPI bool next();

private:
	std::vector<std::vector<std::string>> m_rows;
	size_t m_pos;
};

/**
	Function: RunDBQuery
	Description: Executes a generic query
	Parameters:
		mysql => the connection to be executed the query
		str => the query
	Return: true if the query was successfully executed, otherwise false
*/
GPMSAPI bool RunDBQuery(mdk_mysql* mysql, std::string str);

/**
	Function: RunDBQuery
	Description: Executes a generic query and store it's result to the passed result res
	Parameters:
		mysql => the connection to be executed the query
		str => the query
		rs => the result set to be filled
	Return: true if the query was successfully executed, otherwise false
*/
GPMSAPI bool RunDBQuery(mdk_mysql* mysql, std::string str, ResultSet **rs);

/**
	Function: EscapeSQLString
	Description: Add escapes from a standard string
	Parameters:
		con => The connection to be used to perform the escaping
		str => The string to be escaped
	Return: Escaped string
*/
GPMSAPI std::string EscapeSQLString(mdk_mysql* con, std::string str);

/**
	Function: EscapeSQLString
	Description: Modify the passed string by adding escapes
	Parameters:
		con => The connection to be used to perform the escaping
		str => The string to be escaped
*/
GPMSAPI void EscapeSQLString(mdk_mysql* con, std::string &str);

#endif
