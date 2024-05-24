#define UNUSED(x) (void)(x);
#include "Engine/Net/NetSystem.hpp"
//#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
//-----------------------------------------------------------------------------------------------

// SD1 NOTE: THIS MEANS *EVERY* GAME MUST HAVE AN EngineBuildPreferences.hpp FILE IN ITS CODE/GAME FOLDER!!
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Core/DevConsole.hpp"
#if !defined( ENGINE_DISABLE_NET )

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

extern NetSystem* g_theNet;
extern DevConsole* g_theDevConsole;

//-----------------------------------------------------------------------------------------------
// Initialization code based on example from "FMOD Studio Programmers API for Windows"
//
NetSystem::NetSystem(NetSystemConfig const& config)
{
	m_config = config;
	const int sendSize = m_config.m_sendBufferSize;
	m_sendBuffer = new char[sendSize];
	const int recvSize = m_config.m_receiveBufferSize;
	m_recvBuffer = new char[recvSize];
	SubscribeEventCallbackFunction("RemoteCommand", &Event_RemoteCommand);
	SubscribeEventCallbackFunction("BurstTest", &Event_BurstTest);
}


//-----------------------------------------------------------------------------------------------
NetSystem::~NetSystem()
{
	delete m_sendBuffer;
	m_sendBuffer = nullptr;
	delete m_recvBuffer;
	m_recvBuffer = nullptr;
}


//------------------------------------------------------------------------------------------------
void NetSystem::Startup()
{
	if (m_config.m_mode == NetSystemMode::SERVER) {
		ServerStartup();
	}
	else if (m_config.m_mode == NetSystemMode::CLIENT) {
		ClientStartup();
	}

	
}


//------------------------------------------------------------------------------------------------
void NetSystem::Shutdown()
{
	if (m_config.m_mode == NetSystemMode::SERVER) {
		closesocket(m_clientSocket);
		closesocket(m_listenSocket);

		WSACleanup();
	}
	else if (m_config.m_mode == NetSystemMode::CLIENT) {
		closesocket(m_clientSocket);
		WSACleanup();
	}
}


//-----------------------------------------------------------------------------------------------
void NetSystem::BeginFrame()
{
	if (m_config.m_mode == NetSystemMode::SERVER) {
		ServerBeginFrame();
	}
	else if (m_config.m_mode == NetSystemMode::CLIENT) {
		ClientBeginFrame();
	}
}


//-----------------------------------------------------------------------------------------------
void NetSystem::EndFrame()
{
}

void NetSystem::AddSendMessage(std::string message)
{
	m_sendQueue.push_back(message);
}

void NetSystem::AddQueueToSendBuffer()
{
	int size = 0;
	for (int i = 0; i < (int)m_sendQueue.size(); ++i)
	{

		if (!strncpy_s(m_sendBuffer, m_config.m_sendBufferSize, m_sendQueue[i].c_str(), strlen(m_sendQueue[i].c_str()) + 1) == 0)
			//Copy failed if true
		{
			ERROR_AND_DIE("This copy action is failed");
		}
		size = (int)m_sendQueue[i].size() + 1;
		if ( size > m_config.m_sendBufferSize)
		{
			ERROR_AND_DIE("the size of send buffer is too big");
		}
		int sendResult = send(m_clientSocket, m_sendBuffer, (int)strlen(m_sendBuffer) + 1, 0);
		if (sendResult == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				ERROR_AND_DIE("Error happened at send()\n");
			}
		}
	}
	
	//check the send buffer size if bigger than m_sendBufferSize, ERROR_AND_DIE
	//check the receive buffer size if bigger than m_sendBufferSize, ERROR_AND_DIE

	m_sendQueue.clear();
}

bool NetSystem::ReceiveMessage(char*& messageBuffer, int length)
{
	for (int i = 0; i < length; i++) {
		char const& temp = messageBuffer[i];
		if (temp == -51) {
			break;
		}
		if (temp == '\0') {
			if (!m_recvRemaining.empty()) {
				g_theDevConsole->Execute(m_recvRemaining);
				if (m_config.m_mode == NetSystemMode::SERVER) {
					std::string echo = Stringf("Echo Message=\"Executed remote console command: %s\"", m_recvRemaining.c_str());
					AddSendMessage(echo);
				}
				m_recvRemaining.clear();
			}
		}
		else {
			m_recvRemaining += temp;
		}
	}
	return true;
}

bool NetSystem::Event_RemoteCommand(EventArgs& args)
{
	std::string command = args.GetValue("Command", "");
	TrimString(command, "\"");
	g_theNet->AddSendMessage(command);
	return true;
}

bool NetSystem::Event_AddSendMessage(std::string msg)
{
	g_theNet->AddSendMessage(msg);
	return true;
}

bool NetSystem::Event_BurstTest(EventArgs& args)
{
	UNUSED(args);
	for (int index = 1; index <= 20; ++index)
	{
		g_theNet->m_sendQueue.emplace_back(Stringf("Echo Message=%d", index));
	}
	return true;
}




NetSystemConfig const& NetSystem::GetConfig() const
{
	return m_config;
}


bool NetSystem::AmIConnected() const
{
	if (m_config.m_mode == NetSystemMode::NONE)
		return true;
	if (m_config.m_mode == NetSystemMode::CLIENT && m_clientState == NetClientState::CONNECTED)
		return true;
	if (m_config.m_mode == NetSystemMode::SERVER && m_serverState == NetServerState::CONNECTED)
		return true;

	return false;
}

void NetSystem::ServerStartup()
{
	WSADATA data;
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result == SOCKET_ERROR) {
		ERROR_AND_DIE(Stringf("Server startup error: %d\n", WSAGetLastError()));
	}

	// create listen socket
	m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// set blocking mode
	unsigned long blockingMode = 1;
	result = ioctlsocket(m_listenSocket, FIONBIO, &blockingMode);
	if (result == SOCKET_ERROR) {
		ERROR_AND_DIE(Stringf("Server set blocking mode error: %d\n", WSAGetLastError()));
	}
	// get host port from string
	Strings host = SplitStringOnDelimiter(m_config.m_host, ":");
	m_hostAddress = INADDR_ANY;
	m_hostPort = (unsigned short)(atoi(host[1].c_str()));

	// bind the listen socket to a port
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(m_hostAddress);
	addr.sin_port = htons(m_hostPort);
	result = bind(m_listenSocket, (sockaddr*)&addr, (int)sizeof(addr));
	if (result == SOCKET_ERROR) {
		ERROR_AND_DIE(Stringf("Server bind listen socket error: %d\n", WSAGetLastError()));
	}

	// listen for connections to accept
	result = listen(m_listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		ERROR_AND_DIE(Stringf("Server listen for connection error: %d\n", WSAGetLastError()));
	}

	m_serverState = NetServerState::LISTENING;
}

void NetSystem::ClientStartup()
{
	// startup windows sockets
	WSADATA data;
	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result == SOCKET_ERROR) {
		ERROR_AND_DIE(Stringf("Client startup error: %d\n", WSAGetLastError()));
	}
	// create client socket
	m_clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// set blocking mode
	unsigned long blockingMode = 1;
	result = ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
	if (result == SOCKET_ERROR) {
		ERROR_AND_DIE(Stringf("Client set blocking mode error: %d\n", WSAGetLastError()));
	}

	m_clientState = NetClientState::READY_TO_CONNECT;

	// get host address 
	Strings host = SplitStringOnDelimiter(m_config.m_host, ":");
	IN_ADDR addr;
	result = inet_pton(AF_INET, host[0].c_str(), &addr);
	if (result == SOCKET_ERROR) {
		ERROR_AND_DIE(Stringf("Client get host address error: %d\n", WSAGetLastError()));
	}
	m_hostAddress = ntohl(addr.S_un.S_addr);

	// get host port from string
	m_hostPort = (unsigned short)(atoi(host[1].c_str()));
}

void NetSystem::ServerBeginFrame()
{
	if (m_serverState == NetServerState::LISTENING) {
		
		uintptr_t socket = accept(m_listenSocket, NULL, NULL);

		//m_clientSocket = socket;
		if (socket != INVALID_SOCKET) {
			m_clientSocket = socket;
			unsigned long blockingMode = 1;
			int result = ioctlsocket(m_clientSocket, FIONBIO, &blockingMode);
			if (result == SOCKET_ERROR) {
				ERROR_AND_DIE(Stringf("Server listening set blocking mode error: %d\n", WSAGetLastError()));
			}
			else {
				m_serverState = NetServerState::CONNECTED;
			}

		}
		else {
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				ERROR_AND_DIE(Stringf("Server accept error: %d\n", WSAGetLastError()));
			}
			else
			{
				DebuggerPrintf("No connection for Server to accept\n");
				return;
			}
		}
	}

	if (m_serverState == NetServerState::CONNECTED) {

		int result = recv(m_clientSocket, m_recvBuffer, sizeof(m_recvBuffer), 0);
		if (result == SOCKET_ERROR) {
			int error = WSAGetLastError();
			if (error == WSAECONNRESET) {
				m_serverState = NetServerState::LISTENING;
				Shutdown();
				Startup();
				return;
			}
			else if (error != WSAEWOULDBLOCK) {
				ERROR_AND_DIE(Stringf("Server receiving error: %d\n", WSAGetLastError()));
			}
		}
		else if (result == 0) {
			m_serverState = NetServerState::LISTENING;
			Shutdown();
			Startup();
			return;
		}

		// receive msg too long
		if (result != SOCKET_ERROR && result > m_config.m_receiveBufferSize) {
			ERROR_AND_DIE("Server receive message size is larger than receive buffer");
		}

		// receive message
		ReceiveMessage(m_recvBuffer, result);

		// send message
		if ((int)m_sendQueue.size() > 0) {
			AddQueueToSendBuffer();
		}
	}
}

void NetSystem::ClientBeginFrame()
{
	if (m_clientState == NetClientState::READY_TO_CONNECT)
	{
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = htonl(m_hostAddress);
		addr.sin_port = htons(m_hostPort);
		int result = connect(m_clientSocket, (sockaddr*)(&addr), (int)sizeof(addr));

		if (result == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error != WSAEWOULDBLOCK)
			{
				ERROR_AND_DIE(Stringf("Client connect error: %d\n", WSAGetLastError()));
			}
			else
			{
				m_clientState = NetClientState::CONNECTING;
				return;
			}
		}
	}

	if (m_clientState == NetClientState::CONNECTING) {
		//Check if our connection attempt failed.
		fd_set failedSockets;
		FD_ZERO(&failedSockets);
		FD_SET(m_clientSocket, &failedSockets);
		timeval failedWaitTime = { };
		int failedResult = select(0, NULL, NULL, &failedSockets, &failedWaitTime);

		if (failedResult == SOCKET_ERROR) {
			ERROR_AND_DIE(Stringf("Client connection attempt failed error: %d\n", WSAGetLastError()));
		}
		if (failedResult > 0 && FD_ISSET(m_clientSocket, &failedSockets)) {
			m_clientState = NetClientState::READY_TO_CONNECT;
			return;
		}

		//Check if our connection attempt completed.
		fd_set successSockets;
		FD_ZERO(&successSockets);
		FD_SET(m_clientSocket, &successSockets);
		timeval successWaitTime = { };
		int successResult = select(0, NULL, &successSockets, NULL, &successWaitTime);
		if (successResult == SOCKET_ERROR) {
			ERROR_AND_DIE(Stringf("Client connection attempt completed error: %d\n", WSAGetLastError()));
		}
		//We are connected if the following is true.
		if (successResult > 0 && FD_ISSET(m_clientSocket, &successSockets)) {
			m_clientState = NetClientState::CONNECTED;
		}
	}

	if (m_clientState == NetClientState::CONNECTED)
	{
		//Send and receive if we are connected.
		int result = recv(m_clientSocket, m_recvBuffer, m_config.m_receiveBufferSize, 0);
		if (result == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			if (error == WSAECONNRESET)
			{
				m_clientState = NetClientState::READY_TO_CONNECT;
				Shutdown();
				Startup();
				return;
			}
			else if (error != WSAEWOULDBLOCK)
			{
				ERROR_AND_DIE(Stringf("Client receiving error: %d\n", WSAGetLastError()));
			}
		}
		else if (result == 0)
		{
			m_clientState = NetClientState::READY_TO_CONNECT;
			Shutdown();
			Startup();
			return;
		}

		// receive msg too long
		if (result != SOCKET_ERROR && result > m_config.m_receiveBufferSize) {
			ERROR_AND_DIE("Client receive message size is larger than receive buffer");
		}

		// receive message
		ReceiveMessage(m_recvBuffer, result);
		// send message
		if ((int)m_sendQueue.size() > 0) {
			AddQueueToSendBuffer();
		}
	}
}
#endif // !defined( ENGINE_DISABLE_NET )