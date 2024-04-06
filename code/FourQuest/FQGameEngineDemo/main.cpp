#include <iostream>

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

using namespace fq::game_module;

struct Event
{
	int value = 0;
};

struct MyStruct
{
	MyStruct(EventManager* eventMgr)
		:handle(eventMgr->RegisterHandle<Event>(this, &MyStruct::Call))
	{}

	void Call(const Event& event) { std::cout << event.value << " 멤버함수 호출\n"; }
	EventHandler handle;
};

int main()
{
	EventManager ev;

	auto handle =  ev.RegisterHandle<Event>([](const Event& event) 
		{std::cout << event.value  << " 람다 호출\n" ; });


	MyStruct s{ &ev };

	ev.FireEvent(Event{ 10 });

	handle.Unregister();

	ev.FireEvent(Event{ 10 });

	return 0;
}