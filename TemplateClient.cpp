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
#include "TemplateClient.h"

#include <stdlib.h>
#include <string.h>

#include <uv.h>

void libuv_callback_on_client_tcp_connection(uv_connect_t *req, int status);
void libuv_callback_on_client_tcp_read(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf);
extern void libuv_callback_allocate_buffer(uv_handle_t *handle, size_t size, uv_buf_t* buf);

MDKDLLAPI CTemplateClient::CTemplateClient() {}
MDKDLLAPI CTemplateClient::~CTemplateClient() {}

void MDKDLLAPI CTemplateClient::ConnectTCP(const char*ip, int port)
{
	struct sockaddr_in addr;
	m_socket = (mdk_socket)malloc(sizeof(uv_tcp_t));

	uv_tcp_init((uv_loop_t*)m_loop, (uv_tcp_t*)m_socket);
	uv_ip4_addr(ip, port, &addr);

	((uv_tcp_t*)m_socket)->data = (void*)new CClientData(this);

	uv_connect_t* req = (uv_connect_t*)malloc(sizeof(uv_connect_t));
	uv_tcp_connect(req, (uv_tcp_t*)m_socket, (const sockaddr*)&addr, libuv_callback_on_client_tcp_connection);
}

void libuv_callback_on_client_tcp_connection(uv_connect_t *req, int status)
{
	CTemplateClient* client_pointer = NULL;
	
	if (!req)
		return;

	if (!req->handle->data)
	{
		free(req);
		return;
	}
	
	client_pointer = (CTemplateClient*)req->handle->data;

	if (!client_pointer->OnTCPNewConnection((mdk_socket)req->handle, status) == 0)
	{
		free(req);
		return;
	}

	if (status == 0)
		uv_read_start(req->handle, libuv_callback_allocate_buffer, libuv_callback_on_client_tcp_read);

	free(req);
}

void libuv_callback_on_client_tcp_read(uv_stream_t *client, ssize_t nread, const uv_buf_t* buf)
{
	CTemplateClient* client_pointer = NULL;
	
	if (nread < 1)
		return;
	
	if (!buf)
		return;
	
	if (!client)
		return;
	
	if (!client->data)
		return;

	client_pointer->OnTCPRead(client, buf->base, nread);

	uv_read_start(client, libuv_callback_allocate_buffer, libuv_callback_on_client_tcp_read);
}
