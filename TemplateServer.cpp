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
#include "TemplateServer.h"

#include "Utility.h"

#include <uv.h>

#define DEFAULT_BACKLOG 128

// Parse callbacks from TemplateSocket
extern void libuv_callback_on_close(uv_handle_t *handle);
extern void libuv_callback_allocate_buffer(uv_handle_t *handle, size_t size, uv_buf_t* buf);
void libuv_callback_on_server_tcp_new_connection(uv_stream_t *server, int status);
void libuv_callback_on_server_udp_read(uv_udp_t* handle, ssize_t nread, const uv_buf_t* recv, const struct sockaddr* addr, unsigned int flags);
void libuv_callback_on_server_tcp_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);

MDKDLLAPI CTemplateServer::CTemplateServer() {}
MDKDLLAPI CTemplateServer::~CTemplateServer() {}

bool MDKDLLAPI CTemplateServer::Bind(const char *ip, int port, bool udp)
{
	int r = 0;
	struct sockaddr_in addr;
	uv_udp_t* real_udp_socket = NULL;
	uv_tcp_t* real_tcp_socket = NULL;

	// Initialize the socket
	if (udp)
	{
		m_sockType = SOCKET_UDP;
		real_udp_socket = (uv_udp_t*)malloc(sizeof(uv_udp_t));
		r = uv_udp_init((uv_loop_t*)m_loop, real_udp_socket);
	}
	else
	{
		real_tcp_socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
		r = uv_tcp_init((uv_loop_t*)m_loop, real_tcp_socket);
	}

	if (r)
	{
		LOG_ERROR("Server", "%s listen error: %s\n", udp ? "UDP" : "TCP", uv_strerror(r));
		return false;
	}

	// Resolve ip and port
	uv_ip4_addr(ip, port, &addr);

	if (udp)
		r = uv_udp_bind(real_udp_socket, (const struct sockaddr*)&addr, 0);
	else
		r = uv_tcp_bind(real_tcp_socket, (const struct sockaddr*)&addr, 0);

	if (r)
	{
		LOG_ERROR("Server", "%s listen error: %s\n", udp ? "UDP" : "TCP", uv_strerror(r));
		return false;
	}

	/* Listen the socket */
	if (udp)
		r = uv_udp_recv_start((uv_udp_t*)real_udp_socket, (uv_alloc_cb)libuv_callback_allocate_buffer, libuv_callback_on_server_udp_read);
	else
		r = uv_listen((uv_stream_t*)real_tcp_socket, DEFAULT_BACKLOG, libuv_callback_on_server_tcp_new_connection);

	if (r)
	{
		LOG_ERROR("Server", "%s listen error: %s\n", udp ? "UDP" : "TCP", uv_strerror(r));
		return false;
	}

	// Access the loop from the other functions
	if (udp)
	{
		real_udp_socket->data = (void*)new CClientData(this);
		m_socket = (void*)real_udp_socket;
	}
	else
	{
		real_tcp_socket->data = (void*)new CClientData(this);
		m_socket = (void*)real_tcp_socket;
	}
	
	if (r)
	{
		LOG_ERROR("Server", "%s listen error: %s\n", udp ? "UDP" : "TCP", uv_strerror(r));
		return false;
	}

	return true;
}

void libuv_callback_on_server_tcp_new_connection(uv_stream_t *server_socket, int status)
{
	//_t equals to structure in libuv code
	uv_tcp_t* client_socket = NULL;
	CClientData* client_data = NULL;

	if (status < 0 || server_socket->data == NULL)
		return; // data is NULL, no pointer to the Template server or cannot accept the connection


	client_data = CTemplateSocket::GetSocketExtraData((mdk_socket)server_socket);

	client_socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));

	uv_tcp_init((uv_loop_t*)client_data->GetSocket()->GetLoop(), client_socket);

	// Accept the client
	status = uv_accept(server_socket, (uv_stream_t *)client_socket); 

	if (status != 0)
	{
		uv_close((uv_handle_t*)client_socket, libuv_callback_on_close);
	}

	client_socket->data = server_socket->data;
	
	if (!client_data->GetSocket()->OnTCPNewConnection((mdk_socket)client_socket, 0))
	{
		uv_close((uv_handle_t*)client_socket, libuv_callback_on_close);
		return;
	}

	status = uv_read_start((uv_stream_t*)client_socket, libuv_callback_allocate_buffer, libuv_callback_on_server_tcp_read);

	if (status != 0)
		uv_close((uv_handle_t*)client_socket, libuv_callback_on_close);
}


void libuv_callback_on_server_tcp_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	if (nread < 0 || stream->data == NULL)
	{
		uv_close((uv_handle_t*)stream, libuv_callback_on_close);

		//if (buf->base)
		//	free(buf->base);

		return;
	}
	else if (nread == 0)
	{
		//if (buf->base)
		//	free(buf->base);

		return;
	}

#ifdef _DEBUG
	LOG_INFO("Server", "TCP: Received %s from client", buf->base);
#endif

	CTemplateSocket::GetSocketExtraData((mdk_socket)stream)->GetSocket()->OnTCPRead(stream, buf->base, nread);
	
	//if (buf->base)
	//	free(buf->base);
}

void libuv_callback_on_server_udp_read(uv_udp_t* handle, ssize_t nread, const uv_buf_t* recv, const struct sockaddr* addr, unsigned int flags)
{
	CTemplateSocket* socket_ptr = NULL;
	char ip[INET_ADDRSTRLEN];
	struct sockaddr_in* addr_in = (struct sockaddr_in*)addr;
	
	ip[0] = '\0';
	
	if (nread <= 0)
	{
		LOG_INFO("Server", "UDP: Received nothing from client");
		return;
	}
	
	if (handle->data == NULL)
	{
		LOG_INFO("Server", "UDP: Wrong pointer!");
		return;
	}
	
	socket_ptr = (CTemplateSocket*)handle->data;
	
	int x = CTemplateServer::GetIPFromSocket(handle);
	inet_ntop(AF_INET, &(addr_in->sin_addr), ip, INET_ADDRSTRLEN);
	
	LOG_INFO("Server", "UDP: Connected %s(%d) %s (Flags: %u)", recv->base, nread, ip, flags);
	
	CTemplateSocket::GetSocketExtraData((mdk_socket)handle)->GetSocket()->OnUDPRead(handle, addr, recv->base, nread);
}
