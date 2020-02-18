#include "stdafx.h"

#include "MemoryUnit.h"

MemoryUnit::MemoryUnit(const MEMORY_UNIT_TYPE memoryUnitType)
	: overlapped()
	, wsaBuffer()
	, memoryUnitType(memoryUnitType)
	, dataBuffer()
{
	dataBuffer = new char[DATA_BUFFER_SIZE];

	if (memoryUnitType == MEMORY_UNIT_TYPE::RECV_FROM_CLIENT
		|| memoryUnitType == MEMORY_UNIT_TYPE::RECV_FROM_QUERY)
	{
		wsaBuffer.len = DATA_BUFFER_SIZE;
	}

	wsaBuffer.buf = dataBuffer;
}

MemoryUnit::~MemoryUnit()
{
	delete[] dataBuffer;
}