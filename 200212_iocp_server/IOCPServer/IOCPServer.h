#pragma once

#include "../global_header.hh"

#include "UserInfo.h" //자주 엑세스하는 만큼, 포인터 성능 저하 방지.

class IOCPServer
{
	static constexpr int RECV_BUFFER_SIZE = 100;
	static constexpr int WORKER_THREAD_NUM = 4;

public:
	IOCPServer();
	~IOCPServer();
	
	DISABLED_COPY(IOCPServer)
	DISABLED_MOVE(IOCPServer)

	void InitNetwork();
	void Run();

	void AcceptThreadFunction();
	void WorkerThreadFunction();

private:
	SOCKET listenSocket;
	HANDLE hIOCP;
	
	std::thread acceptThread;

	std::vector<std::thread> workerThreadCont;
	//std::atomic<bool> workerThreadLoopFlag;
	bool workerThreadLoopFlag;

	concurrency::concurrent_queue<_Key> keyPool;
	concurrency::concurrent_queue<SOCKET> waitingUserPool;
	std::array<UserInfo, MAX_USER> userCont;
};
