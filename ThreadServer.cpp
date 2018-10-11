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
#include "ThreadServer.h"
#include "ModuleEntryPoint.h"

#include <uv.h>

void libuv_callback_general_thread_function(void* arg);

CThreadServer::CThreadServer(int defaultport, bool udp)
{
	m_bUDP = udp;
	m_iDefaultPort = defaultport;
	m_lpThread = NULL;
	m_iExitCode = 0;
	m_lpDatabase = NULL;
	m_port = -1;
	m_ip = NULL;
}

CThreadServer::~CThreadServer()
{
	Stop();
}

bool CThreadServer::Start(const char* ip, int port, CDatabase* db, ModuleConfigMap cfg)
{
	uv_thread_t* thread = (uv_thread_t*)malloc(sizeof(uv_thread_t));
	
	m_ip = (char*)ip;
	m_cfg = cfg;
	m_lpDatabase = db;
	
	if (port < 0)
		m_port = m_iDefaultPort;
	else
		m_port = port;

	
	if (uv_thread_create(thread, libuv_callback_general_thread_function, this) != 0)
	{
		free(thread);
		return false;
	}

	m_lpThread = (mdk_thread)thread;
	
	return true;
}

void CThreadServer::Stop()
{
	if (!m_lpThread)
		return;
	
	StopServer();
		
	uv_thread_join((uv_thread_t*)m_lpThread);
	
	free(m_lpThread);
	
	m_lpThread = NULL;
}

bool CThreadServer::IsRunning()
{
	return m_iExitCode == ERROR_STILL_ALIVE;
}

unsigned long CThreadServer::GetExitCode()
{
	return m_iExitCode;
}

void CThreadServer::Execute()
{
	m_iExitCode = Initialize();
	if (m_iExitCode != ERROR_NONE)
		return;
	
	if (!Bind((const char*)m_ip, m_port, m_bUDP))
	{
		m_iExitCode = ERROR_BIND_ERROR;
		return;
	}
	
	m_iExitCode = StartServer();
}

int CThreadServer::Initialize() { return ERROR_NONE; }

void libuv_callback_general_thread_function(void* arg)
{
	if (!arg)
		return;
	
	((CThreadServer*)arg)->Execute();
}
