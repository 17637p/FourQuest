#include "EventHandler.h"
#include "EventManager.h"

fq::game_module::EventHandler::EventHandler(const void* handle, EventManager* eventMgr)
	:mHandle(handle), mEventManager(eventMgr)
{}

fq::game_module::EventHandler::EventHandler(EventHandler&& other) noexcept
	:mHandle(std::exchange(other.mHandle,nullptr))
	,mEventManager(std::exchange(other.mEventManager,nullptr))
{}

fq::game_module::EventHandler::EventHandler()
	:mHandle(nullptr)
	,mEventManager(nullptr)
{}

fq::game_module::EventHandler& fq::game_module::EventHandler::operator=(EventHandler&& other) noexcept
{
	mHandle = std::exchange(other.mHandle, nullptr);
	mEventManager = std::exchange(other.mEventManager, nullptr);

	return *this;
}

fq::game_module::EventHandler::~EventHandler()
{
	Unregister();
}

const void* fq::game_module::EventHandler::Handle() const
{
	return mHandle;
}

void fq::game_module::EventHandler::Unregister() noexcept
{
	if (mEventManager && mHandle)
	{
		mEventManager->RemoveHandle(*this);
		mHandle = nullptr;
	}
}

bool fq::game_module::EventHandler::IsRegistered() const
{
	return mEventManager && mHandle;
}
