#pragma once

#include "../global_header.hh"
#include "MemoryUnit.h"

struct TaskUnit;

struct UserUnit
{
	MemoryUnit memoryUnit;
	
	TaskUnit* taskUnit; // loadedBuffer[DATA_BUFFER_SIZE];
	int loadedSize; // �̰� ���� ��Ʈ? ĳ�� �̽� ����.

	SOCKET socket;

	//

	_Key key;
	_Pos posX;

public:
	UserUnit();
};