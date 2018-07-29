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
#define MDK_EXPORT_FUNCTIONS 1 /*Export the methods*/
#include "TemplateServer.h"

#include "Utility.h"

#include <uv.h>

#define DEFAULT_BACKLOG 128

// Parse callbacks from TemplateSocket
extern void libuv_callback_on_close(uv_handle_t *handle);
extern void libuv_callback_allocate_buffer(uv_handle_t *handle, size_t size, uv_buf_t* buf);
void libuv_callback_on_server_tcp_new_connection(uv_stream_t *server, int status);
void libuv_callback_on_server_udp_new_connection(uv_udp_t* handle, ssize_t nread, const uv_buf_t* recv, const struct sockaddr* addr, unsigned int flags);
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
		real_udp_socket = (uv_udp_t*)malloc(sizeof uv_udp_t);
		r = uv_udp_init((uv_loop_t*)m_loop, real_udp_socket);
	}
	else
	{
		real_tcp_socket = (uv_tcp_t*)malloc(sizeof uv_tcp_t);
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
		r = uv_udp_recv_start((uv_udp_t*)real_tcp_socket, libuv_callback_allocate_buffer, libuv_callback_on_server_udp_new_connection);
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
		real_udp_socket->data = (void*)this;
		m_socket = (void*)real_udp_socket;
	}
	else
	{
		real_tcp_socket->data = (void*)this;
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
	CTemplateServer* server_pointer = NULL;

	if (status < 0 || server_socket->data == NULL)
		return; // data is NULL, no pointer to the Template server or cannot accept the connection

	server_pointer = (CTemplateServer*)server_socket->data;
	
	client_socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));

	uv_tcp_init((uv_loop_t*)server_pointer->GetLoop(), client_socket);

	// Accept the client
	status = uv_accept(server_socket, (uv_stream_t *)client_socket); 

	if (status != 0)
	{
		uv_close((uv_handle_t*)client_socket, libuv_callback_on_close);
	}

	client_socket->data = server_socket->data;
	
	if (!server_pointer->OnNewConnection((mdk_socket)client_socket, 0))
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
	CTemplateSocket *socket_ptr = NULL;

	if (nread < 0 || stream->data == NULL)
	{
		uv_close((uv_handle_t*)stream, libuv_callback_on_close);

		if (buf->base)
			free(buf->base);

		return;
	}
	else if (nread == 0)
	{
		if (buf->base)
			free(buf->base);

		return;
	}

	socket_ptr = (CTemplateServer*)stream->data;
	socket_ptr->OnRead((mdk_socket)stream, buf->base, nread);
	
	if (buf->base)
		free(buf->base);
}

void libuv_callback_on_server_udp_new_connection(uv_udp_t* handle, ssize_t nread, const uv_buf_t* recv, const struct sockaddr* addr, unsigned int flags)
{
	CTemplateSocket* socket_ptr = NULL;
	char ipstr[256];
	
	if (nread <= 0)
	{
		free(recv->base);
		return;
	}
	
	if (handle == NULL || handle->data == NULL)
	{
		free(recv->base);
		return;
	}
	
	socket_ptr = (CTemplateSocket*)handle->data;
	
	int x = CTemplateServer::GetIPFromSocket(handle);
	inet_ntop(AF_INET, &x, ipstr, sizeof(ipstr));
	
	LOG_INFO("Server", "%s %s %d", recv->base, ipstr, nread);
	
	free(recv->base);
}
