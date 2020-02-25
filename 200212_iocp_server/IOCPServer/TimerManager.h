#pragma once

#include "../global_header.hh"
#include "../BaseSingleton.h"


enum class TIMER_TYPE
{
	PUSH_OLD_KEY = 0
};

enum class TIME : unsigned long long
{
	SECOND = 1000,
	MINUATE = 60000
};

struct TimerUnit
{
	TIMER_TYPE timerType;
	_Time eventTime;
	_Key ownerKey;

	// 꼭 필요 없을 수 있는 것. // 
	_Key targetKey;
	// std::any data;
};

class TimerManager
	: public TSingleton<TimerManager>
{
	static constexpr int TIMER_MEMORY_POOL_SIZE = 100000;
	static constexpr int ADD_TIMER_MEMORY_POOL_SIZE = 10000;

public:
	TimerManager();
	~TimerManager();

public:
	void AddTimerEvent(const TIMER_TYPE, const _Key ownerKey, const _Key targetKey, const TIME waitTime);
	void AddTimerEvent(TimerUnit* timerUnit, const TIME waitTime);

private:
	struct TimerUnitCompareFunction
	{
		bool operator()(TimerUnit* left, TimerUnit* right) noexcept
		{
			return (left->eventTime) > (right->eventTime);
		}
	};

private:

	void TimerThread();

	bool ProcessTimerUnit(TimerUnit* timerUnit);
	void PushTimerUnit(TimerUnit* timerUnit);
	TimerUnit* PopTimerUnit();

	std::thread timerThread;

	concurrency::concurrent_priority_queue<TimerUnit*, TimerUnitCompareFunction> timerCont;
	concurrency::concurrent_queue<TimerUnit*> timerMemoryPool;
};


