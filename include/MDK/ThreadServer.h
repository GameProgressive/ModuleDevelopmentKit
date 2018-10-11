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
#ifndef _MODULEDEVELOPMENTKIT_THREAD_H
#define _MODULEDEVELOPMENTKIT_THREAD_H

#include "Database.h"
#include "TemplateServer.h"

#include <string>
#include <map>

/* Module definition */
typedef std::map<std::string, std::string> ModuleConfigMap;

typedef void* mdk_thread;

/*
	A template server ment to work with multithreading
*/
class CThreadServer : public CTemplateServer
{
public:
	CThreadServer(int defaultport, bool udp);
	~CThreadServer();
	
	/*
		Function: Initialize
		Description: Initialize the thread server after it has been started
		Return: exit code if error, ERROR_NONE if no error
	*/
	virtual int Initialize();
	
	/*
		Function: Start
		Description: Starts the thread
		Parameters:
			arg => any argument to be passed into the thread
	*/
	bool Start(const char* ip, int port, CDatabase* db, ModuleConfigMap cfg);
	
	/*
		Function: Stop
		Description: Stop the thread
	*/
	void Stop();

	/*
		Function: IsRunning
		Descrption: Checks if the module is running
		Return: true if the module is running, otherwise false
	*/
	bool IsRunning();

	/*
		Function: GetExitCode
		Description: Get the exit code of the thread
		Return: the exit code
	*/
	unsigned long GetExitCode();
	

	/*
		Function: Execute
		Description: Execute the thread after it has been started
		NOTE: You should only call this function if you want to handle your loop manually
	*/
	void Execute();
	
protected:
	mdk_thread m_lpThread;
	unsigned long m_ulExitCode;
	CDatabase* m_lpDatabase;
	ModuleConfigMap m_cfg;
	int m_iDefaultPort;
	char* m_ip;
	int m_port;
	bool m_bUDP;
};


#endif
