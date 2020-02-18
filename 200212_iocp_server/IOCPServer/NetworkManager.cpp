#include "stdafx.h"

#include "MemoryUnit.h"
#include "UserInfo.h"
#include "NetworkManager.h"

NetworkManager::NetworkManager()
	: sendMemoryPool()
{
	for (int i = 0; i < SEND_MEMORY_POOL_SIZE; ++i)
	{
		sendMemoryPool.push(new MemoryUnit(MEMORY_UNIT_TYPE::SEND_TO_CLIENT));
	}
}

NetworkManager::~NetworkManager()
{
	MemoryUnit* memoryUnit{ nullptr };
	while (sendMemoryPool.try_pop(memoryUnit))
	{
		delete memoryUnit;
	}
}

void NetworkManager::SendPacket(UserInfo* pUser, char* packetData)
{
	auto sendMemoryUnit = PopSendMemoryUnit();
	
	memcpy(sendMemoryUnit->dataBuffer, packetData, packetData[0]);
	sendMemoryUnit->wsaBuffer.len = static_cast<ULONG>(packetData[0]);

	ZeroMemory(&(sendMemoryUnit->overlapped), sizeof(sendMemoryUnit->overlapped));

	if (SOCKET_ERROR == WSASend(pUser->socket, &(sendMemoryUnit->wsaBuffer), 1, NULL, 0, &(sendMemoryUnit->overlapped), NULL))
	{
		// if (ERROR_HANDLING::HandleSendError())
		// {
		// 	closesocket(pUser->socket);
		// }
	}
}

MemoryUnit* NetworkManager::PopSendMemoryUnit()
{
	MemoryUnit* retMemoryUnit;
	while (!sendMemoryPool.try_pop(retMemoryUnit))
	{
		std::cout << "메모리를 할당합니다.";
		for (int i = 0; i < ADD_SEND_MEMORY_POOL_SIZE; ++i)
		{
			retMemoryUnit = new MemoryUnit(MEMORY_UNIT_TYPE::SEND_TO_CLIENT);
			sendMemoryPool.push(retMemoryUnit);
		}
	}
	return retMemoryUnit;
}

void NetworkManager::PushSendMemoryUnit(MemoryUnit* const memoryUnit)
{
	sendMemoryPool.push(memoryUnit);
	// memoryUnit = nullptr;
}