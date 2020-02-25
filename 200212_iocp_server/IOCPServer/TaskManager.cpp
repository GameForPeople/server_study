#include "stdafx.h"

#include "TaskUnit.h"
#include "TaskManager.h"

#include "Utils.h"

TaskManager::TaskManager()
	: taskMemoryPool()
{
	for (int i = 0; i < TASK_MEMORY_POOL_SIZE; ++i)
	{
		taskMemoryPool.push(new TaskUnit());
	}
}

TaskManager::~TaskManager()
{
	TaskUnit* memoryUnit{ nullptr };
	while (taskMemoryPool.try_pop(memoryUnit))
	{
		delete memoryUnit;
	}
}

void TaskManager::ProduceTask(TASK_PROCESS_CASE taskProcessCase, TaskUnit* const taskUnit)
{
	taskCont[static_cast<size_t>(taskProcessCase)].push(taskUnit);
}

bool TaskManager::ConsumeTask(TASK_PROCESS_CASE taskProcessCase, TaskUnit* taskUnit)
{
	return taskCont[static_cast<size_t>(taskProcessCase)].try_pop(taskUnit);
}

TaskUnit* TaskManager::PopTaskUnit()
{
	if (TaskUnit* retTaskUnit{ nullptr }
	; taskMemoryPool.try_pop(retTaskUnit))
	{
		return retTaskUnit;
	}
	else
	{
		std::cout << "TaskUnit을 추가로 할당합니다.";
		for (int i = 0; i < ADD_TASK_MEMORY_POOL_SIZE; ++i)
		{
			retTaskUnit = new TaskUnit();
			taskMemoryPool.push(retTaskUnit);
		}

		return new TaskUnit();
	}
}
void TaskManager::PushTaskUnit(TaskUnit* const taskUnit)
{
	taskMemoryPool.push(taskUnit);
	// memoryUnit = nullptr;
}

