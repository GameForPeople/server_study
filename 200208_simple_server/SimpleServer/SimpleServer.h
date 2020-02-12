#pragma once

#include "../global_header.hh"

struct UserInfo;

class SingleIOCPServer
{
	static constexpr int RECV_BUFFER_SIZE = 100;

public:
	SingleSimpleServer();
	~SingleSimpleServer();
	
	DISABLED_COPY(SingleIOCPServer)
	DISABLED_MOVE(SingleIOCPServer)

	void InitNetwork();
	void Run();

private:
	SOCKET listenSocket;
	std::atomic<bool> workerThreadLoopFlag;
	_Pos xPos;
};

class IOCPServer
{
	static constexpr int RECV_BUFFER_SIZE = 100;

public:
	IOCPServer();
	~IOCPServer();
	
	DISABLED_COPY(IOCPServer)
	DISABLED_MOVE(IOCPServer)

	void InitNetwork();
	void Run();

	void WorkerThreadFunction(_Key inkey);

private:
	SOCKET listenSocket;
	std::atomic<bool> workerThreadLoopFlag;
	std::list<std::thread> threadCont;

	std::array<std::unique_ptr<UserInfo>, MAX_USER> userCont;
	std::mutex userLock;

	std::queue<_Key> keyPool;
	std::mutex keyLock;
};

