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
#ifndef _MODULEDEVELOPMENTKIT_TEMPLATESERVER_H_
#define _MODULEDEVELOPMENTKIT_TEMPLATESERVER_H_

#include "TemplateSocket.h"

/*
	This class rapresents a libuv TCP or UDP Server
*/
class CTemplateServer : public CTemplateSocket
{
public:
	MDKDLLAPI CTemplateServer();
	MDKDLLAPI ~CTemplateServer();

	/*
		Function: Bind
		Description: Bind the socket
		Return: true if the server binded, otherwise false
		Parameters:
			ip => The ip to bind
			port => The port to bind
			udp => True if this is a UDP server
	*/
	bool MDKDLLAPI Bind(const char *ip, int port, bool udp);
};

#endif
