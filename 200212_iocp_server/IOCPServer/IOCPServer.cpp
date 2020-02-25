#include "stdafx.h"
#include "Utils.h"

//#include "UserUnit.h"
#include "IOCPServer.h"

#include "MemoryUnit.h"
#include "TaskUnit.h"

#include "NetworkManager.h"
#include "TimerManager.h"
#include "TaskManager.h"

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
	TimerManager::GetInstance();
	NetworkManager::GetInstance();
	TaskManager::GetInstance();

	for (int i = 0; i < MAX_USER; ++i) { keyPool.push(i); }
	for (auto& user : userCont)
	{
	}
	// InitNetwork
	{
#pragma region [ 윈속 초기화 ]
		if (WSADATA wsa; WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			ERROR_UTIL::Error("WSAStartup()");
		}
#pragma endregion

#pragma region [ 입출력 완료 포트 생성 ]
		if (hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)
			; hIOCP == NULL) ERROR_UTIL::Error("Create_IOCompletionPort()");
#pragma endregion

#pragma region [ 워커 쓰레드 생성 ]
		//printf("!. 현재 워커쓰레드 개수는 코어의 개수와 상관없이 %d 개로 제한, 생성합니다. \n", NETWORK_UTIL::WORKER_THREAD_NUM);
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

		std::cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << std::endl;
		std::cout << "■  Simple Server	               ■" << std::endl;
		std::cout << "■                                       ver 0.1  200208  ■" << std::endl;
		std::cout << "■                                                        ■" << std::endl;
		std::cout << "■  IP ADDRESS  : " << /*serverAddr.sin_addr.s_addr*/ ip << "                         ■" << std::endl;
		std::cout << "■  PORT NUMBER : " << SERVER_LISTEN_PORT_NUMBER << "                                    ■" << std::endl;
		std::cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << std::endl << std::endl;
#pragma endregion
	}
}

IOCPServer::~IOCPServer()
{
	// ?? 이 코드봐 미쳤나봐
	workerThreadLoopFlag = false;
	{
		using namespace std::chrono_literals;
		for (int i = 5; i != 0; --i)
		{
			std::cout << i << " 초 후, 서버가 종료됩니다.\n";
			std::this_thread::sleep_for(1s);
			system("cls");
		}
	}

	{
		SOCKET socket{};
		while (waitingUserPool.try_pop(socket))
		{
			closesocket(socket);
		}
	}

	closesocket(listenSocket);
	WSACleanup();
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

		std::cout << "[ 새로운 클라이언트 접속 IP : " << inet_ntoa(clientAddr.sin_addr) << "  PORT : " << ntohs(clientAddr.sin_port) << "  ] " << std::endl;

		_Key key;
		
		if (!waitingUserPool.empty())
		{
			// 클라단에서 대기처리
			waitingUserPool.push(acceptedSocket);
			continue;
		}

		if (!keyPool.try_pop(key))
		{
			// 클라단에서 대기처리
			waitingUserPool.push(acceptedSocket);
			continue;
		}

		std::cout << key << " - 유저 접속" << "\n";

		// 소켓과 입출력 완료 포트 연결
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(acceptedSocket), hIOCP, key, 0);

		/*
			Get User Info From DB
		*/

		// DB로부터 유저 정보 받은 후 처리하도록 수정 필요함.
		// _Pos posX = rand() % MAX_MAP_SIZE;
		// PACKET::SERVER_TO_CLIENT::ADD_OBJECT addObject{ key, posX };
		// send(acceptedSocket, reinterpret_cast<char*>(&addObject), sizeof(addObject), 0);
		// 
		// userCont[key].socket = acceptedSocket;
		// userCont[key].posX = posX;

		// 비동기 입출력의 시작.
	}
}

void IOCPServer::WorkerThreadFunction()
{
	int retVal{};
	DWORD cbTransferred;
	unsigned long long key{};

	/*LPVOID*/ MemoryUnit* pMemoryUnit{ nullptr };

	while (workerThreadLoopFlag)
	{
		retVal = GetQueuedCompletionStatus(hIOCP
			, &cbTransferred
			, &key
			, reinterpret_cast<LPOVERLAPPED*>(&pMemoryUnit)
			, INFINITE
		);

		// 에러 처리 없음
		switch (reinterpret_cast<MemoryUnit*>(pMemoryUnit)->memoryUnitType)
		{
			case MEMORY_UNIT_TYPE::SEND_TO_CLIENT:
			{
				NetworkManager::GetInstance().PushSendMemoryUnit(pMemoryUnit);
			}
			break;
			case MEMORY_UNIT_TYPE::RECV_FROM_CLIENT:
			{
				MakePacketFromRecvData(reinterpret_cast<UserUnit*>(pMemoryUnit), cbTransferred);
			}
			break;
			default:
			{

			}
		}
	}
}

void IOCPServer::MakePacketFromRecvData(UserUnit* pUserUnit, int recvSize)
{
	char* pBuf = pUserUnit->memoryUnit.dataBuffer;
	int packetSize{ 0 }; 

	if (0 < pUserUnit->loadedSize) { packetSize = pUserUnit->taskUnit->buffer[0]; }
	else 
	{
		pUserUnit->taskUnit = TaskManager::GetInstance().PopTaskUnit(); 
		pUserUnit->taskUnit->key = pUserUnit->key;
	}

	while (recvSize > 0)
	{
		if (packetSize == 0) packetSize = static_cast<int>(pBuf[0]);

		// 처리해야하는 패킷 사이즈 중에서, 이전에 이미 처리한 패킷 사이즈를 빼준다.
		int required = (packetSize) - (pUserUnit->loadedSize);

		// 패킷을 완성할 수 있을 때 (요청해야할 사이즈보다, 남은 사이즈가 크거나 같을 때)
		if (recvSize >= required)
		{
			memcpy(pUserUnit->taskUnit->buffer + pUserUnit->loadedSize, pBuf, required);

			//-------------------------------------------------------------------------------
			TaskManager::GetInstance().ProduceTask(TaskManager::TASK_PROCESS_CASE::MAIN_ZONE, pUserUnit->taskUnit);
			//-------------------------------------------------------------------------------

			pUserUnit->taskUnit = nullptr;
			pUserUnit->loadedSize = 0;
			recvSize -= required;
			pBuf += required;
			packetSize = 0;
		}
		// 패킷을 완성할 수 없을 때
		else
		{
			memcpy(pUserUnit->taskUnit->buffer + pUserUnit->loadedSize, pBuf, recvSize);
			pUserUnit->loadedSize += recvSize;
			break;
			//restSize = 0; 
		}
	}
}
