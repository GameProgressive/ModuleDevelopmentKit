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
#define GPMS_EXPORT 1 /*Export the methods*/
#include "MasterServerMDK.h"

#include <mysql.h> /*Still using MariaDB Connector C*/
#include <stdio.h>

GPMSAPI ResultSet::ResultSet()
{
	m_pos = 0;
}

GPMSAPI ResultSet::~ResultSet()
{
	size_t i = 0;
	
	for (; i < m_rows.size(); i++)
	{
		m_rows.at(i).clear();
	}
	
	m_rows.clear();
}

GPMSAPI bool ResultSet::executeQuery(mdk_mysql* con, std::string str)
{
	MYSQL_RES *result = NULL;
	MYSQL_ROW row;
	unsigned int fields = 0;
	
	if (mysql_query((MYSQL*)con, str.c_str()) != 0)
	{
		LOG_ERROR("Query", "Cannot execute query. Error: %s\n", mysql_error((MYSQL*)con));
		return false;		
	}
	
	result = mysql_store_result((MYSQL*)con);
	if (!result)
	{
		LOG_ERROR("Query" "Cannot execute query. Error: %s\n", mysql_error((MYSQL*)con));
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
		LOG_ERROR("Query", "Cannot execute query. Error: %s\n", mysql_error((MYSQL*)con));
		return false;			
	}
	
	do
	{
		std::vector<std::string> vec;
		unsigned int i =  0;
		
		for (; i < fields; i++)
			vec.push_back(row[i]);
		
		m_rows.push_back(vec);
	} while (row);
	
	mysql_free_result(result);
	
	return true;
}

size_t ResultSet::getRows()
{
	return m_rows.size();
}

bool ResultSet::first()
{
	if (m_rows.size() < 1)
		return false;
	
	m_pos = 0;
	return true;
}

bool ResultSet::next()
{
	m_pos++;
	
	if (m_rows.size() < (m_pos+1))
	{
		m_pos = 0;
		return false;
	}
	
	return true;
}

double ResultSet::getDouble(size_t index)
{
	if (m_rows.at(m_pos).size() < index)
		return 0;
	
	return atof(m_rows.at(m_pos).at(index).c_str());
}

unsigned int atoui(const char *x)
{
	return (unsigned int) strtol (x, (char **) NULL, 10);
}

GPMSAPI unsigned int ResultSet::getUInt(size_t index)
{
	if (m_rows.at(m_pos).size() < index)
		return 0;
	
	return atoui(m_rows.at(m_pos).at(index).c_str());
}

GPMSAPI std::string ResultSet::getString(size_t index)
{
	if (m_rows.at(m_pos).size() < index)
		return 0;
	
	return m_rows.at(m_pos).at(index);
}

GPMSAPI int ResultSet::getInt(size_t index)
{
	if (m_rows.at(m_pos).size() < index)
		return 0;
	
	return atoi(m_rows.at(m_pos).at(index).c_str());
}