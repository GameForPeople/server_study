#include "stdafx.h"

#include "TaskUnit.h"
#include "UserUnit.h"

UserUnit::UserUnit()
	: memoryUnit(MEMORY_UNIT_TYPE::RECV_FROM_CLIENT)
	
	, taskUnit(nullptr)
	, loadedSize(0)
	
	, socket()
	, posX()
{
}