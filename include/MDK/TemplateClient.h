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
#ifndef _MODULEDEVELOPMENTKIT_TEMPLATECLIENT_H_
#define _MODULEDEVELOPMENTKIT_TEMPLATECLIENT_H_

#include "TemplateSocket.h"

/*
	This class create comunication between Server to Server like 
	
	natneg1.gamespy.com (Server 1)
	gpsp.gamespy.com (Server 2)

	natneg send this: /valid/blalkba/final/ to gpsp.gamespy.com with CTemplateClient

*/
class CTemplateClient : public CTemplateSocket
{
public:
	MDKDLLAPI CTemplateClient();
	MDKDLLAPI ~CTemplateClient();
	
	MDKDLLAPI void ConnectTCP(const char*ip, int port);
};

#endif
