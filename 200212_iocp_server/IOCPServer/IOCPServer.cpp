#include "stdafx.h"
#include "../Utils.hpp"

//#include "UserInfo.h"
#include "IOCPServer.h"

#include "MemoryUnit.h"

IOCPServer::IOCPServer()
	: listenSocket()
	, hIOCP()

	, acceptThread()

	, workerThreadCont()
	, workerThreadLoopFlag(true)

	, keyPool()
	, waitingUserPool()
	, userCont()
{
	for (int i = 0; i < MAX_USER; ++i) { keyPool.push(i); }
	for (auto& user : userCont)
	{
	}

	InitNetwork();
}

IOCPServer::~IOCPServer()
{
	workerThreadLoopFlag = false;
	{
		using namespace std::chrono_literals;
		for (int i = 5; i != 0; --i)
		{
			std::cout << i << " �� ��, ������ ����˴ϴ�.\n";
			std::this_thread::sleep_for(1s);
			system("cls");
		}
	}

	//for (auto& thread : workerThreadCont) { thread.join(); };
	closesocket(listenSocket);
	WSACleanup();
}

void IOCPServer::InitNetwork()
{
#pragma region [ ���� �ʱ�ȭ ]
	if (WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
	{ 
		ERROR_UTIL::Error("WSAStartup()");
	}
#pragma endregion

#pragma region [ ����� �Ϸ� ��Ʈ ���� ]
	if (hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)
		; hIOCP == NULL) ERROR_UTIL::Error("Create_IOCompletionPort()");
#pragma endregion

#pragma region [ ��Ŀ ������ ���� ]
	//printf("!. ���� ��Ŀ������ ������ �ھ��� ������ ������� %d ���� ����, �����մϴ�. \n", NETWORK_UTIL::WORKER_THREAD_NUM);
	workerThreadCont.reserve(WORKER_THREAD_NUM);
	for (int i = 0; i < /* (int)si.dwNumberOfProcessors * 2 */ WORKER_THREAD_NUM; ++i)
	{
		workerThreadCont.emplace_back([&]()
		{
			this->WorkerThreadFunction();
		}
		);
	}
#pragma endregion

#pragma region [ socket() ]
	if (listenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)
		; listenSocket == INVALID_SOCKET)
	{
		ERROR_UTIL::Error("socket()");
	}
#pragma endregion

#pragma region [ Bind() ]
	SOCKADDR_IN serverAddr;
	ZeroMemory(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVER_LISTEN_PORT_NUMBER);
	if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		ERROR_UTIL::Error("bind()");
	}
#pragma endregion

#pragma region [Print ServerUI]
	PHOSTENT host;
	char name[255];
	char* ip{};

	if (gethostname(name, sizeof(name)) != 0)
	{
		ERROR_UTIL::Error("gethostname()");
	}
	if ((host = gethostbyname(name)) != NULL)
	{
		ip = inet_ntoa(*(struct in_addr*) * host->h_addr_list);
	}

	std::cout << "�������������������������������" << std::endl;
	std::cout << "��  Simple Server	               ��" << std::endl;
	std::cout << "��                                       ver 0.1  200208  ��" << std::endl;
	std::cout << "��                                                        ��" << std::endl;
	std::cout << "��  IP ADDRESS  : " << /*serverAddr.sin_addr.s_addr*/ ip << "                         ��" << std::endl;
	std::cout << "��  PORT NUMBER : " << SERVER_LISTEN_PORT_NUMBER << "                                    ��" << std::endl;
	std::cout << "�������������������������������" << std::endl << std::endl;
#pragma endregion
}

void IOCPServer::Run()
{
#pragma region [ Listen() ]
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		ERROR_UTIL::Error("listen()");
	}
	std::cout << "	- On Listen!,   Server Start! " << std::endl << std::endl;
#pragma endregion

	acceptThread = static_cast<std::thread>([&]() {this->AcceptThreadFunction(); });

	acceptThread.join();
	for (auto& thread : workerThreadCont) thread.join();
}

void IOCPServer::AcceptThreadFunction()
{
	SOCKET acceptedSocket{};
	SOCKADDR_IN clientAddr{};
	int addrLen{};

	while (7)
	{
		//accept
		addrLen = sizeof(clientAddr);
		if (acceptedSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
			acceptedSocket == INVALID_SOCKET) {
			ERROR_UTIL::Error("accept()");
		}

		std::cout << "[ ���ο� Ŭ���̾�Ʈ ���� IP : " << inet_ntoa(clientAddr.sin_addr) << "  PORT : " << ntohs(clientAddr.sin_port) << "  ] " << std::endl;

		_Key key;
		
		if (!waitingUserPool.empty())
		{
			waitingUserPool.push(acceptedSocket);
			continue;
		}

		if (!keyPool.try_pop(key))
		{
			// std::cout << "[ MAXCLIENT ������ �źεǾ����ϴ�. IP : " << inet_ntoa(clientAddr.sin_addr) << "  PORT : " << ntohs(clientAddr.sin_port) << "  ] " << std::endl;
			// closesocket(acceptedSocket);
			waitingUserPool.push(acceptedSocket);
			continue;
		}

		std::cout << key << " - ���� ����" << "\n";

		/*
			Get User Info From DB
		*/

		// DB�κ��� ���� ���� ���� �� ó���ϵ��� ���� �ʿ���.
		// _Pos posX = rand() % MAX_MAP_SIZE;
		// PACKET::SERVER_TO_CLIENT::ADD_OBJECT addObject{ key, posX };
		// send(acceptedSocket, reinterpret_cast<char*>(&addObject), sizeof(addObject), 0);
		// 
		// userCont[key].socket = acceptedSocket;
		// userCont[key].posX = posX;
	}
}

void IOCPServer::WorkerThreadFunction()
{
	int retVal{};
	DWORD cbTransferred;
	unsigned long long key{};

	LPVOID pMemoryUnit{ nullptr };

	while (workerThreadLoopFlag)
	{
		retVal = GetQueuedCompletionStatus(hIOCP
			, &cbTransferred
			, &key
			, reinterpret_cast<LPOVERLAPPED*>(&pMemoryUnit)
			, INFINITE
		);

		// ���� ó�� ����
		switch (reinterpret_cast<MemoryUnit*>(pMemoryUnit)->memoryUnitType)
		{
		}
	}
}
