#pragma once

#include "../global_header.hh"
#include "MemoryUnit.h"

struct UserInfo
{
	MemoryUnit memoryUnit;
	
	char loadedBuffer[DATA_BUFFER_SIZE];
	int loadedSize; // �̰� ���� ��Ʈ? ĳ�� �̽� ����.

	SOCKET socket;

	_Pos posX;

public:
	UserInfo();
};