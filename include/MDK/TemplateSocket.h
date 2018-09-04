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
	virtual MDKDLLAPI void OnTCPRead(mdk_socket, const char *, ssize_t) = 0;
	virtual MDKDLLAPI void OnUDPRead(mdk_socket, const struct sockaddr* addr, const char *, ssize_t) = 0;
	
	/* Virtual function called when a new connection happens */
	virtual MDKDLLAPI bool OnTCPNewConnection(mdk_socket, int) = 0;
	
	/* TODO: How can i do multiple loops???? */
	/* This function run the loop, but WTF is slow with this :X */
	void MDKDLLAPI Run();
	
	/* Close the connection of a socket */
	static void MDKDLLAPI Close(mdk_socket socket);
	
	/* Write some data to the socket */
	static void MDKDLLAPI WriteTCP(mdk_socket socket, std::string str);
	static void MDKDLLAPI WriteTCP(mdk_socket socket, void *data, int size);
	static void MDKDLLAPI WriteTCP(mdk_socket socket, const char* data);

	static void MDKDLLAPI WriteUDP(mdk_socket socket, std::string str, const struct sockaddr* addr);
	static void MDKDLLAPI WriteUDP(mdk_socket socket, void *data, int size, const struct sockaddr* addr);
	static void MDKDLLAPI WriteUDP(mdk_socket socket, const char* data, const struct sockaddr* addr);
	
	static int MDKDLLAPI GetIPFromSocket(mdk_socket socket);
	
	inline MDKDLLAPI mdk_loop GetLoop() { return m_loop; }
	
	inline int MDKDLLAPI GetIPFromSocket() { return GetIPFromSocket(m_socket); }
	
	inline void MDKDLLAPI WriteTCP(std::string str) { WriteTCP(m_socket, str); }
	inline void MDKDLLAPI WriteTCP(void* data, int size) { WriteTCP(m_socket, data, size); }
	inline void MDKDLLAPI WriteTCP(const char* data) { WriteTCP(m_socket, data); }
	
	inline void MDKDLLAPI WriteUDP(std::string str, const struct sockaddr* addr) { WriteUDP(m_socket, str, addr); }
	inline void MDKDLLAPI WriteUDP(void* data, int size, const struct sockaddr* addr) { WriteUDP(m_socket, data, size, addr); }
	inline void MDKDLLAPI WriteUDP(const char* data, const struct sockaddr* addr) { WriteUDP(m_socket, data, addr); }
	
	inline void MDKDLLAPI Close() { Close(m_socket); }

	MDKDLLAPI static CClientData* GetSocketExtraData(mdk_socket socket);

protected:
	ESocketType m_sockType;
	mdk_loop m_loop;
	mdk_socket m_socket;
};

#endif
