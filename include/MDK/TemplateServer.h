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
