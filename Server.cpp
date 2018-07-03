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
#define GPMS_EXPORT 1 /*Export the methods*/
#include <MDK/MasterServerMDK.h>

#include <string.h>
#include <uv.h> /* Still using libUV */

/* Defines */
#define DEFAULT_BACKLOG 128

/* uv_udp_getpeername (since it dosen't exists in libUV)*/
#ifdef _WIN32
#define s_addr S_un.S_addr

int uv_udp_getpeername(const uv_udp_t* handle,
                       struct sockaddr* name,
                       int* namelen) {
  int result;

  if (handle->socket == INVALID_SOCKET) {
    return UV_EINVAL;
  }

  result = getpeername(handle->socket, name, namelen);
  if (result != 0) {
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

int uv_udp_getpeername(const uv_udp_t* handle,
                       struct sockaddr* name,
                       int* namelen) {
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

/* Prototypes */
void _OnTCPNewConnection(uv_stream_t *server, int status);
void _OnUDPNewConnection(uv_stream_t *server, int status);

void _AllocBuffer(uv_handle_t *handle, size_t size, uv_buf_t* buf);
void _OnRead(uv_stream_t *stream, ssize_t size, const uv_buf_t *buf);
void _OnClose(uv_handle_t *handle);
void _OnWrite(uv_write_t* req, int status);


GPMSAPI Server::Server()
{
	m_loop = (mdk_loop)malloc(sizeof uv_loop_t);
	uv_loop_init((uv_loop_t*)m_loop);
}

GPMSAPI Server::~Server()
{
	if (m_loop)
	{
		uv_loop_close((uv_loop_t*)m_loop);
		free(m_loop);
	}
	
	m_loop = NULL;
}

bool GPMSAPI Server::Bind(const char *ip, int port, bool udp)
{
	int r = 0;
	struct sockaddr_in addr;

	m_data.loop = m_loop;
	m_data.instance = this;

	// Initialize the socket

	if (udp)
		uv_udp_init((uv_loop_t*)m_loop, (uv_udp_t*)&m_udp);
	else
		uv_tcp_init((uv_loop_t*)m_loop, (uv_tcp_t*)&m_tcp);

	// Resolve ip and port
	uv_ip4_addr(ip, port, &addr);

	if (udp)
		uv_udp_bind((uv_udp_t*)&m_udp, (const struct sockaddr*)&addr, 0);
	else
		uv_tcp_bind((uv_tcp_t*)&m_tcp, (const struct sockaddr*)&addr, 0);

	/* Listen the socket */
	if (udp)
		r = uv_listen((uv_stream_t*)&m_udp, DEFAULT_BACKLOG, _OnUDPNewConnection);
	else
		r = uv_listen((uv_stream_t*)&m_tcp, DEFAULT_BACKLOG, _OnTCPNewConnection);

	// Access the loop from the other functions
	if (udp)
	{
		uv_udp_t* udpp = (uv_udp_t*)&m_udp;
		udpp->data = (void*)&m_data;
	}
	else
	{
		uv_tcp_t* tcpp = (uv_tcp_t*)&m_tcp;
		tcpp->data = (void*)&m_data;
	}

	if (r)
	{
		LOG_ERROR("Server", "%s listen error: %s\n", udp ? "UDP" : "TCP", uv_strerror(r));
		return false;
	}

	return true;
}

void GPMSAPI Server::Write(mdk_client client, void *data, int size)
{
	uv_write_t *req = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_buf_t buf;

	buf.len = size;
	buf.base = (char*)data;

	uv_write(req, (uv_stream_t*)client, &buf, 1, _OnWrite);
}

void GPMSAPI Server::Write(mdk_client client, std::string str)
{
	Write(client, (void*)str.c_str(), str.length());
}

void GPMSAPI Server::Write(mdk_client client, const char* data)
{
	Write(client, (void*)data, strlen(data));
}

void GPMSAPI Server::Close(mdk_client client)
{
	uv_close((uv_handle_t*)client, _OnClose);
}

int GPMSAPI Server::GetIPFromStream(mdk_client client)
{
	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len;
	ClientData *data = NULL;
	uv_stream_t* stream = (uv_stream_t*)client;

	if (!stream->data)
		return -1;

	data = (ClientData*)stream->data;

	if (data->IsUDP())
		uv_udp_getpeername((const uv_udp_t*)stream, (struct sockaddr*)&clientaddr, (int*)&clientaddr_len);
	else
		uv_tcp_getpeername((const uv_tcp_t*)stream, (struct sockaddr*)&clientaddr, (int*)&clientaddr_len);

	return clientaddr.sin_addr.s_addr;
}

void GPMSAPI Server::Run()
{
	uv_run((uv_loop_t*)m_loop, UV_RUN_DEFAULT);
}

// Pure virtual functions
void GPMSAPI Server::OnRead(mdk_client, const char *, ssize_t) {}
bool GPMSAPI Server::OnNewConnection(mdk_client) { return true; }

// UV Callbacks
void _OnClose(uv_handle_t* handle)
{
	if (handle)
	{
		// Free CClientData
		if (handle->data)
			delete (ClientData*)handle->data;

		free(handle);
	}
}

// ClientData
GPMSAPI ClientData::~ClientData() {}

GPMSAPI ClientData::ClientData(Server *instance, bool udp)
{
	m_instance = instance;
	m_data = NULL;
	m_udp = udp;
}

GPMSAPI Server* ClientData::GetInstance()
{
	return m_instance;
}

GPMSAPI void *ClientData::GetUserData()
{
	return m_data;
}

GPMSAPI void ClientData::SetUserData(void *data)
{
	m_data = data;
}

GPMSAPI bool ClientData::IsUDP()
{
	return m_udp;
}

/* Prototypes */
void _OnWrite(uv_write_t* req, int)
{
	if (req)
		free(req);
}

void _AllocBuffer(uv_handle_t*, size_t size, uv_buf_t *buf)
{
	*buf = uv_buf_init((char*)malloc(size), size);
}

void _OnRead(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
	ClientData *data = NULL;

	if (nread < 0 || stream->data == NULL)
	{
		uv_close((uv_handle_t*)stream, _OnClose);

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

	data = (ClientData*)stream->data;
	data->GetInstance()->OnRead((mdk_client)stream, buf->base, nread);
	
	if (buf->base)
		free(buf->base);
}

// TCP Callbacks
void _OnTCPNewConnection(uv_stream_t *server, int status)
{
	uv_tcp_t *client = NULL;
	TServerData *data = NULL;

	if (status < 0 || server->data == NULL)
		return;

	data = (TServerData*)server->data;

	client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));

	uv_tcp_init((uv_loop_t*)data->loop, client);

	// Accept the client
	status = uv_accept(server, (uv_stream_t *)client); 

	if (status != 0)
	{
		uv_close((uv_handle_t*)client, _OnClose);
	}

	client->data = (void*)new ClientData(data->instance, false);
	
	if (!data->instance->OnNewConnection((mdk_client)client))
	{
		uv_close((uv_handle_t*)client, _OnClose);
		return;
	}

	status = uv_read_start((uv_stream_t*)client, _AllocBuffer, _OnRead);

	if (status != 0)
		uv_close((uv_handle_t*)client, _OnClose);
}

// UDP Callbacks
void _OnUDPNewConnection(uv_stream_t *server, int status)
{
	uv_udp_t *client = NULL;
	TServerData *data = NULL;
	uv_loop_t *loop = NULL;

	if (status < 0 || server->data == NULL)
		return;

	data = (TServerData*)server->data;

	client = (uv_udp_t *)malloc(sizeof(uv_udp_t));
	
	loop = (uv_loop_t*)data->loop;

	// Accept the client
	uv_udp_init(loop, client);

	if (status != 0)
	{
		uv_close((uv_handle_t*)client, _OnClose);
	}

	client->data = (void*)new ClientData(data->instance, true);
	
	if (!data->instance->OnNewConnection((mdk_client*)client))
	{
		uv_close((uv_handle_t*)client, _OnClose);
		return;
	}

	status = uv_read_start((uv_stream_t*)client, _AllocBuffer, _OnRead);

	if (status != 0)
		uv_close((uv_handle_t*)client, _OnClose);
}

GPMSAPI ClientData* Server::GetData(mdk_client client)
{
	uv_stream_t* stream = (uv_stream_t*)client;
	
	if (!stream)
		return NULL;
	
	if (!stream->data)
		return NULL;
	
	return (ClientData*)stream->data;
}
