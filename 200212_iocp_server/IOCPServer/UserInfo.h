#pragma once

#include "../global_header.hh"
#include "MemoryUnit.h"

struct UserInfo
{
	MemoryUnit memoryUnit;
	
	char loadedBuffer[DATA_BUFFER_SIZE];
	int loadedSize; // 이거 굳이 인트? 캐시 미스 오짐.

	SOCKET socket;

	_Pos posX;

public:
	UserInfo();
};