#pragma once

#include "../global_header.hh"
#include "../BaseSingleton.h"

struct MemoryUnit;
struct TaskUnit;
struct UserInfo;

class NetworkManager
	: public TSingleton<NetworkManager>
{
	static constexpr int SEND_MEMORY_POOL_SIZE = 100000;
	static constexpr int ADD_SEND_MEMORY_POOL_SIZE = 1000;

public:
	NetworkManager();
	~NetworkManager();

public:
	void SendPacket(UserInfo* pUser, char* packetData);
	void PushSendMemoryUnit(MemoryUnit* const memoryUnit);

private:
	MemoryUnit* PopSendMemoryUnit();
	concurrency::concurrent_queue<MemoryUnit*> sendMemoryPool;
	concurrency::concurrent_queue<TaskUnit*> taskMemoryPool;
};