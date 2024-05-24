#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <string>

enum class NetSystemMode
{
	NONE,
	CLIENT,
	SERVER
};

enum class NetClientState
{
	INVALID,
	READY_TO_CONNECT,
	CONNECTING,
	CONNECTED,
};

enum class NetServerState
{
	INVALID,
	LISTENING,
	CONNECTED,
};

struct NetSystemConfig
{
	NetSystemMode	m_mode = NetSystemMode::NONE;
	std::string		m_host;
	int				m_sendBufferSize = 2048;
	int				m_receiveBufferSize = 2048;
};


//-----------------------------------------------------------------------------------------------
class NetSystem;


/////////////////////////////////////////////////////////////////////////////////////////////////
class NetSystem
{
public:
	NetSystem(NetSystemConfig const& config);
	virtual ~NetSystem();

public:
	void						Startup();
	void						Shutdown();
	virtual void				BeginFrame();
	virtual void				EndFrame();

	void		AddSendMessage(std::string message);
	void		AddQueueToSendBuffer();
	bool		ReceiveMessage(char*& messageBuffer, int length);

	static bool Event_RemoteCommand(EventArgs& args);
	static bool Event_AddSendMessage(std::string msg);
	static bool Event_BurstTest(EventArgs& args);

	NetSystemConfig const& GetConfig() const;

	bool		AmIConnected() const;
private:
	void		ServerStartup();
	void		ClientStartup();

	void		ServerBeginFrame();
	void		ClientBeginFrame();		

protected:
	NetSystemConfig					m_config;

	NetClientState m_clientState = NetClientState::INVALID;
	NetServerState m_serverState = NetServerState::INVALID;

	uintptr_t m_clientSocket = ~0ull;
	uintptr_t m_listenSocket = ~0ull;

	unsigned long m_hostAddress = 0;
	unsigned short m_hostPort = 0;

	char* m_sendBuffer = nullptr;
	char* m_recvBuffer = nullptr;

	std::vector<std::string> m_sendQueue;
	std::string m_recvRemaining;
};

