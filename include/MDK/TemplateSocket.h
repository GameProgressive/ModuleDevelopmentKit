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
#ifndef _MODULEDEVELOPMENTKIT_TEMPLATESOCKET_H_
#define _MODULEDEVELOPMENTKIT_TEMPLATESOCKET_H_

#include "MDK_Definitions.h"

class CTemplateSocket;

enum ESocketType
{
	SOCKET_TCP,
	SOCKET_UDP
};

typedef void* mdk_socket;
typedef void* mdk_loop;

class CClientData
{
public:
	inline CClientData(CTemplateSocket* socket)
	{
		m_socket = socket;
		m_data = NULL;
	}

	inline ~CClientData() {}

	inline void SetData(void* data) { m_data = data; }
	inline void* GetData() { return m_data; }

	inline CTemplateSocket* GetSocket() { return m_socket; }
protected:
	CTemplateSocket* m_socket;
	void* m_data;
};

/*
	This class is a generic libUV socket
*/
class CTemplateSocket
{
public:
	MDKDLLAPI CTemplateSocket();
	MDKDLLAPI ~CTemplateSocket();
	
	/* Get The socket type */
	inline ESocketType MDKDLLAPI GetType() { return m_sockType; }
	
	/* Virtual function called when something is readed */
	virtual MDKDLLAPI void OnRead(mdk_socket, const char *, ssize_t) = 0;
	
	/* Virtual function called when a new connection happens */
	virtual MDKDLLAPI bool OnNewConnection(mdk_socket, int) = 0;
	
	/* TODO: How can i do multiple loops???? */
	/* This function run the loop, but WTF is slow with this :X */
	void MDKDLLAPI Run();
	
	/* Close the connection of a socket */
	static void MDKDLLAPI Close(mdk_socket socket);
	
	/* Write some data to the socket */
	static void MDKDLLAPI Write(mdk_socket socket, std::string str);
	static void MDKDLLAPI Write(mdk_socket socket, void *data, int size);
	static void MDKDLLAPI Write(mdk_socket socket, const char* data);

	static int MDKDLLAPI GetIPFromSocket(mdk_socket socket);
	
	inline MDKDLLAPI mdk_loop GetLoop() { return m_loop; }
	
	inline int MDKDLLAPI GetIPFromSocket() { return GetIPFromSocket(m_socket); }
	
	inline void MDKDLLAPI Write(std::string str) { Write(m_socket, str); }
	inline void MDKDLLAPI Write(void* data, int size) { Write(m_socket, data, size); }
	inline void MDKDLLAPI Write(const char* data) { Write(m_socket, data); }
	
	inline void MDKDLLAPI Close() { Close(m_socket); }

	MDKDLLAPI static CClientData* GetSocketExtraData(mdk_socket socket);

protected:
	ESocketType m_sockType;
	mdk_loop m_loop;
	mdk_socket m_socket;
};

#endif
