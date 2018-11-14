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
#define MDK_EXPORT_FUNCTIONS 1 /*Export the methods*/
#include "TemplateSocket.h"

#include <string.h>
#include <uv.h>
#include <stdlib.h>

#include "Utility.h"

/* Prototypes */
int uv_udp_getpeername(const uv_udp_t* handle, struct sockaddr* name, int* namelen);
void libuv_callback_allocate_buffer(uv_handle_t *handle, size_t size, uv_buf_t* buf);
void libuv_callback_on_close(uv_handle_t *handle);
void libuv_callback_when_tcp_write_finished(uv_write_t* req, int status);
void libuv_callback_when_udp_write_finished(uv_udp_send_t* req, int);

MDKDLLAPI CTemplateSocket::CTemplateSocket()
{
	m_sockType = SOCKET_TCP;
	m_socket = NULL;

	uv_loop_t* real_loop = (uv_loop_t*)malloc(sizeof(uv_loop_t));
	uv_loop_init(real_loop);
	m_loop = (mdk_loop)real_loop;
}

MDKDLLAPI CTemplateSocket::~CTemplateSocket()
{
	if (m_socket)
		free(m_socket);

	if (m_loop)
	{
		uv_loop_t* real_loop = (uv_loop_t*)m_loop;
		while (uv_loop_close(real_loop) != UV_EBUSY);
		free(real_loop);
	}
	
	m_loop = NULL;
	m_socket = NULL;
}

int MDKDLLAPI CTemplateSocket::StartServer()
{
	return uv_run((uv_loop_t*)m_loop, UV_RUN_DEFAULT);
}

void MDKDLLAPI CTemplateSocket::Close(mdk_socket socket)
{
	uv_close((uv_handle_t*)socket, libuv_callback_on_close);
}

void MDKDLLAPI CTemplateSocket::StopServer()
{
	if (m_sockType == SOCKET_UDP)
	{
		uv_udp_recv_stop((uv_udp_t*)m_socket);
	}
	
	Close(m_socket);
	uv_stop((uv_loop_t*)m_loop);
}

void MDKDLLAPI CTemplateSocket::WriteTCP(mdk_socket socket, void *data, size_t size)
{
	uv_write_t *req = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_buf_t buf;
	uv_stream_t* real_socket = (uv_stream_t*)socket;

#ifdef _DEBUG
	LOG_INFO("Socket", "TCP: Sending %s to client", (char*)data);
#endif
	buf.len = size;
	buf.base = (char*)data;

	uv_write(req, real_socket, &buf, 1, libuv_callback_when_tcp_write_finished);
}

void MDKDLLAPI CTemplateSocket::WriteUDP(mdk_socket socket, void *data, size_t size, const struct sockaddr* addr)
{
	uv_udp_send_t* req = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
	uv_buf_t buf;

#ifdef _DEBUG
	LOG_INFO("Socket", "UDP: Sending %s to client", (char*)data);
#endif
	buf.len = size;
	buf.base = (char*)data;

	uv_udp_send(req, (uv_udp_t*)socket, &buf, 1, addr, libuv_callback_when_udp_write_finished);
}

void MDKDLLAPI CTemplateSocket::WriteUDP(mdk_socket socket, std::string str, const struct sockaddr* addr)
{
	WriteUDP(socket, (void*)str.c_str(), str.length(), addr);
}

void MDKDLLAPI CTemplateSocket::WriteUDP(mdk_socket socket, const char* data, const struct sockaddr* addr)
{
	WriteUDP(socket, (void*)data, strlen(data), addr);
}

void MDKDLLAPI CTemplateSocket::WriteTCP(mdk_socket socket, std::string str)
{
	WriteTCP(socket, (void*)str.c_str(), str.length());
}

void MDKDLLAPI CTemplateSocket::WriteTCP(mdk_socket socket,  const char* data)
{
	WriteTCP(socket, (void*)data, strlen(data));
}

int MDKDLLAPI CTemplateSocket::GetIPFromSocket(mdk_socket socket)
{
	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len;
	uv_handle_t* real_socket = (uv_handle_t*)socket;
	CTemplateSocket* socket_ptr = NULL;
	
	if (!real_socket->data)
		return -1;

	socket_ptr = (CTemplateSocket*)real_socket->data;
	
	if (socket_ptr->GetType() == SOCKET_UDP)
		uv_udp_getpeername((const uv_udp_t*)socket, (struct sockaddr*)&clientaddr, (int*)&clientaddr_len);
	else
		uv_tcp_getpeername((const uv_tcp_t*)socket, (struct sockaddr*)&clientaddr, (int*)&clientaddr_len);

	return clientaddr.sin_addr.s_addr;
}

MDKDLLAPI CClientData* CTemplateSocket::GetSocketExtraData(mdk_socket socket)
{
	uv_stream_t* real_socket = (uv_stream_t*)socket;
	return (CClientData*)real_socket->data;
}

/* Virtual functions */
MDKDLLAPI void CTemplateSocket::OnTCPRead(mdk_socket, const char *, ssize_t) {}
MDKDLLAPI bool CTemplateSocket::OnTCPNewConnection(mdk_socket, int) { return true; }
MDKDLLAPI void CTemplateSocket::OnUDPRead(mdk_socket, const struct sockaddr* addr, const char *, ssize_t) {}

// UV Callbacks
void libuv_callback_on_close(uv_handle_t* handle)
{
	if (handle)
	{
		// Remove the pointer of CTemplateSocket
		if (handle->data)
			handle->data = NULL;

		free(handle);
	}
}

void libuv_callback_when_tcp_write_finished(uv_write_t* req, int)
{
	if (req)
		free(req);
}

void libuv_callback_when_udp_write_finished(uv_udp_send_t* req, int)
{
	if (req)
		free(req);
}

void libuv_callback_allocate_buffer(uv_handle_t*, size_t size, uv_buf_t *buf)
{
	*buf = uv_buf_init((char*)malloc(size), size);
}

/* uv_udp_getpeername (since it dosen't exists in libUV)*/
#ifdef _WIN32
	#define s_addr S_un.S_addr

	int uv_udp_getpeername(const uv_udp_t* handle, struct sockaddr* name, int* namelen)
	{
		int result;

		if (handle->socket == INVALID_SOCKET) {
			return UV_EINVAL;
	  }

		result = getpeername(handle->socket, name, namelen);
		if (result != 0)
		{
			return uv_translate_sys_error(WSAGetLastError());
		}

	  return 0;
	}
#else
	//Internal definition
	#if defined(__APPLE__)
		int uv___stream_fd(const uv_stream_t* handle);
		#define uv__stream_fd(handle) (uv___stream_fd((const uv_stream_t*) (handle)))
	#else
		#define uv__stream_fd(handle) ((handle)->io_watcher.fd)
	#endif /* defined(__APPLE__) */

	int uv_udp_getpeername(const uv_udp_t* handle, struct sockaddr* name, int* namelen)
	{
		socklen_t socklen;

		if (uv__stream_fd(handle) < 0)
			return UV_EINVAL;  /* FIXME(bnoordhuis) UV_EBADF */

		/* sizeof(socklen_t) != sizeof(int) on some systems. */
		socklen = (socklen_t) *namelen;

		if (getpeername(uv__stream_fd(handle), name, &socklen))
			return -errno;

		*namelen = (int) socklen;
		return 0;
	}
#endif
/* End of uv_udp_getpeername */
