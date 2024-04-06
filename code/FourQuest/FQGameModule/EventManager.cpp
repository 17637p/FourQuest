#include "GameModulePCH.h"
#include "EventManager.h"

bool fq::game_module::EventManager::RemoveHandle(const EventHandler& eventHandler) noexcept
{
	if (!eventHandler.Handle())
	{
		return false;
	}

	bool result = false;
	accessUniqueHandler([this, &result, &eventHandler]()
		{
			for (auto iter = mEventHandles.begin(); iter != mEventHandles.end(); ++iter)
			{
				if (static_cast<const void*>(&(iter->second)) == eventHandler.Handle())
				{
					mEventHandles.erase(iter);
					result = true;
					break;
				}
			}
		}
		);

	return result;
}

void fq::game_module::EventManager::RemoveAllHandles() noexcept
{
	accessUniqueHandler([this]() { mEventHandles.clear(); });
}

std::size_t fq::game_module::EventManager::GetHandlerCount() noexcept
{
	std::size_t count{};
	accessSharedHandler([this, &count]() {count = mEventHandles.size(); });
	return count;
}
