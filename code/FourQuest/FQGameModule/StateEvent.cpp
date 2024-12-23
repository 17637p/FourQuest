#include "StateEvent.h"

namespace fq::game_module
{
	std::shared_ptr<Component> StateEvent::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneAnimator = std::dynamic_pointer_cast<StateEvent>(clone);

		if (cloneAnimator == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneAnimator = ObjectPool::GetInstance()->Assign<StateEvent>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneAnimator = *this;
		}

		return cloneAnimator;
	}

	entt::meta_handle StateEvent::GetHandle()
	{
		return *this;
	}

	void StateEvent::UpdateMapData()
	{
		mInstantiatePrefabMap.clear();
		mPlaySoundInfoMap.clear();

		for (const auto& instantiatePrefab : mInstantiatePrefabs)
		{
			mInstantiatePrefabMap.insert({ instantiatePrefab.FunctionName, instantiatePrefab });
		}

		for (const auto& playSoundInfo : mPlaySoundInfos)
		{
			mPlaySoundInfoMap.insert({ playSoundInfo.FunctionName, playSoundInfo });
		}
	}
}