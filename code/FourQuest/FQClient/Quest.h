#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class Quest : public game_module::Component
	{
	public:
		Quest();
		~Quest();

	private:
		int mIndex;

		bool mIsMain; // QuestType
		int mPreQuestIndex;
	};
}

