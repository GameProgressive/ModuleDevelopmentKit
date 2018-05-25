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
#ifndef _GAMEPROGRESSIVE_MASTERSERVER_SERVER_H_
#define _GAMEPROGRESSIVE_MASTERSERVER_SERVER_H_

#ifndef _GAMEPROGRESSIVE_MASTERSERVER_MDK_H_
	#error "Please include <MasterServerMDK.h> not this file!"
#endif

class Server;

typedef struct SServerData
{
	mdk_loop *loop;
	Server *instance;
} TServerData;

/*
	This class rapresents a libuv implemented TCP or UDP Server
*/
class Server
{
public:
	GPMSAPI Server();
	GPMSAPI ~Server();

	/*
		Function: Bind
		Description: Bind the socket
		Return: true if the server binded, otherwise false
		Parameters:
			ip => The ip to bind
			port => The port to bind
			udp => True if this is a UDP server
	*/
	bool GPMSAPI Bind(const char *ip, int port, bool udp);

	/*
		Function: Write
		Description: Write something to a socket
		Parameters:
			client => the client
			data => the data to write
			size => size of the data to write
	*/
	static void GPMSAPI Write(mdk_client *client, void *data, int size);

	/*
		Function: Write
		Description: Write a set of characters to a socket
		Parameters:
			client => the client
			data => the data to write
	*/
	static void GPMSAPI Write(mdk_client *client, const char *data);

	/*
		Function: Write
		Description: Write a string to a socket
		Parameters:
			client => the client
			data => the string that will be written
	*/
	static void GPMSAPI Write(mdk_client *client, std::string data);

	/*
		Function: OnRead
		Description: This function is called when the server is processing
			readed data from the client
		Parameters:
			client => the client
			data => the data readed
			size => the size of the data
		NOTE: This function must be overwritten by an implementation of the Server
	*/
	virtual void GPMSAPI OnRead(mdk_client* client, const char *data, ssize_t size) = 0;

	/*
		Function: OnNewConnection
		Description: This function is called when a new connection is processed
		Parameters:
			client => the client that is connected
		NOTE: This function must be overwritten by an implementation of the Server
	*/
	virtual bool GPMSAPI OnNewConnection(mdk_client* client) = 0;

	/*
		Function: Close
		Description: Close a client connection
		Parameters:
			client => the client
	*/
	void GPMSAPI Close(mdk_client *client);

	/*
		Function: GetIPFromStream
		Description: Get the IP of a client
		Parameters:
			client => the client to get the ip
	*/
	static int GPMSAPI GetIPFromStream(mdk_client *client);
	
	/*
		Function: Run
		Description: Run the server
	*/
	void GPMSAPI Run(); 

private:
	mdk_loop* m_loop;

	mdk_tcp m_tcp;
	mdk_udp m_udp;

	TServerData m_data;
};

/*
	This class rapresents a generic client data
	that are passed throught all the clients
*/
class ClientData
{
public:
	GPMSAPI ClientData(Server *instance, bool udp);
	GPMSAPI ~ClientData();

	/*
		Function: GetInstance
		Description: Get the server instance
		Return: A pointer to the server instance
	*/
	GPMSAPI Server *GetInstance();
	
	/*
		Function: GetUserData
		Description: Get the user data
		Return: A pointer to the user data
	*/
	GPMSAPI void *GetUserData();

	/*
		Function: SetUserData
		Description: Set user data that could be passed throught the socket
		Parameters:
			data => A pointer to the data that will be passed
	*/
	GPMSAPI void SetUserData(void *data);

	/*
		Function: IsUDP
		Description: Check if the stream is in UDP mode
		Return: true if the stream is UDP otherwise false
	*/
	GPMSAPI bool IsUDP();

protected:
	Server *m_instance;
	void *m_data;
	bool m_udp;
};

#endif
