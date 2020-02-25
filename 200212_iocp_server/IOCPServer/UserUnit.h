#pragma once

#include "../global_header.hh"
#include "MemoryUnit.h"

struct TaskUnit;

struct UserUnit
{
	MemoryUnit memoryUnit;
	
	TaskUnit* taskUnit; // loadedBuffer[DATA_BUFFER_SIZE];
	int loadedSize; // 이거 굳이 인트? 캐시 미스 오짐.

	SOCKET socket;

	//

	_Key key;
	_Pos posX;

public:
	UserUnit();
};