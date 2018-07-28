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

	((uv_tcp_t*)m_socket)->data = (void*)this;

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

	if (!client_pointer->OnNewConnection((mdk_socket)req->handle, status) == 0)
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

	client_pointer->OnRead(client, buf->base, nread);

	uv_read_start(client, libuv_callback_allocate_buffer, libuv_callback_on_client_tcp_read);
}
