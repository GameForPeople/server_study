#pragma once

#include "../global_header.hh"
#include "../BaseSingleton.h"

struct TaskUnit;

class TaskManager
	: public TSingleton<TaskManager>
{
	static constexpr int TASK_MEMORY_POOL_SIZE = 100000;
	static constexpr int ADD_TASK_MEMORY_POOL_SIZE = 1000;

public:
	enum class TASK_PROCESS_CASE
	{
		MAIN_ZONE = 0,
		SIZE
	};

	TaskManager();
	~TaskManager();

public:
	void ProduceTask(TASK_PROCESS_CASE taskProcessCase, TaskUnit* const taskUnit);
	bool ConsumeTask(TASK_PROCESS_CASE taskProcessCase, TaskUnit* const taskUnit);
	
	TaskUnit* PopTaskUnit();
	void PushTaskUnit(TaskUnit* taskUnit);
private:
	concurrency::concurrent_queue<TaskUnit*> taskMemoryPool;
	std::array<concurrency::concurrent_queue<TaskUnit*>, static_cast<size_t>(TASK_PROCESS_CASE::SIZE)> taskCont;
};