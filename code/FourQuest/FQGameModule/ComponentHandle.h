#pragma once


namespace fq::game_module
{
	namespace internal
	{
		template<typename T>
		class ComponentHandle
		{
		public:
			ComponentHandle() : mComponent(nullptr) {}
			ComponentHandle(T* component) : mComponent(component){}

			T& Get() { return *mComponent; }
			T* operator->() { return mComponent; }

			bool IsValid()const { return mComponent != nullptr; }
		private:
			T* mComponent;
		};


	}
}