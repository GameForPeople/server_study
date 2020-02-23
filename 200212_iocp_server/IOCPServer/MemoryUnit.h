#pragma once

#include "../global_header.hh"

enum class MEMORY_UNIT_TYPE
{
	SEND_TO_CLIENT
	, RECV_FROM_CLIENT
	, SEND_TO_QUERY
	, RECV_FROM_QUERY
	, TIMER_FUNCTION
};

struct MemoryUnit
{
public:
	OVERLAPPED overlapped;
	WSABUF wsaBuffer;

	const MEMORY_UNIT_TYPE memoryUnitType;
	char* dataBuffer;

public:
	MemoryUnit(const MEMORY_UNIT_TYPE);
	~MemoryUnit();
};


