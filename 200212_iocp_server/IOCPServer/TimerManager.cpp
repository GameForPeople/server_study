#include "stdafx.h"

#include "TimerManager.h"

TimerManager::TimerManager()
{
	for (int i = 0; i < TIMER_MEMORY_POOL_SIZE; ++i)
	{
		timerMemoryPool.push(new TimerUnit());
	}

	timerThread = static_cast<std::thread>([&](){ this->TimerThread(); });
}

TimerManager::~TimerManager()
{
	TimerUnit* retTimerUnit = nullptr;
	while (timerMemoryPool.try_pop(retTimerUnit)) { delete retTimerUnit; }
	while (timerCont.try_pop(retTimerUnit)) { delete retTimerUnit; }
}

void TimerManager::AddTimerEvent(const TIMER_TYPE timerType, const _Key ownerKey, const _Key targetKey, const TIME waitTime)
{
	TimerUnit* timerUnit = PopTimerUnit();

	timerUnit->timerType = timerType;
	timerUnit->ownerKey = ownerKey;
	timerUnit->targetKey = targetKey;
	timerUnit->eventTime = GetTickCount64() + static_cast<_Time>(waitTime);

	timerCont.push(timerUnit);
}

void TimerManager::AddTimerEvent(TimerUnit* timerUnit, const TIME waitTime)
{
	timerUnit->eventTime = GetTickCount64() + static_cast<_Time>(waitTime);
	timerCont.push(timerUnit);
}

void TimerManager::TimerThread()
{
	using namespace std::literals;
	while (7)
	{
		std::this_thread::sleep_for(0ns);
		const auto tempNowTime = GetTickCount64();

		while (timerCont.size())
		{
			TimerUnit* retTimerUnit{ nullptr };

			// Timer Cont에 등록된 타이머가 없으면 있을 때 까지, 추후에는 최소 타이머로 Loop!
			while (!timerCont.try_pop(retTimerUnit)) { std::this_thread::sleep_for(100ms); }

			if (tempNowTime < retTimerUnit->eventTime)
			{
				// 재등록
				timerCont.push(retTimerUnit);
				break;
			}

			if (ProcessTimerUnit(retTimerUnit)) { PushTimerUnit(retTimerUnit); }
		}
	}
}

bool TimerManager::ProcessTimerUnit(TimerUnit* retTimerUnit)	// return true - 반환 필요, false - 재사용함
{
	switch (retTimerUnit->timerType)
	{
	case TIMER_TYPE::PUSH_OLD_KEY:
		//ConnectManager::GetInstance().PushOldKey(retTimerUnit->ownerKey);
		return true; break;
	default:
		//LogManager::GetInstance().AddLog(LOG_TYPE::WARNING_LOG, L"TimerUnit에 처리하지않는 타입이 존재합니다.", SourceLocation(SOURCE_LOCATION));
		return true; break;
	}
}

_NODISCARD TimerUnit* TimerManager::PopTimerUnit()
{
	TimerUnit* retTimerUnit{ nullptr };

	return timerMemoryPool.try_pop(retTimerUnit)
		? retTimerUnit
		: []()->TimerUnit*
	{
		return new TimerUnit();
	}();
}

void TimerManager::PushTimerUnit(TimerUnit* timerUnit)
{
	timerMemoryPool.push(timerUnit);
}