#include "DynamicLightHelper.h"
#include "../FQGameModule/Component.h"
#include "../FQGameModule/Light.h"

namespace fq::client
{
	void fq::client::DynamicLightHelper::OnStart()
	{
		mLightOrNull = GetComponent<fq::game_module::Light>();
		mAccumulationTime = 0.f;
	}

	void fq::client::DynamicLightHelper::OnUpdate(float dt)
	{
		if (mLightOrNull == nullptr)
		{
			return;
		}
		
		mAccumulationTime += dt * mSpeed;

		// 1초 주기로 min ~ max ~ min으로 돌아옴
		const double _2PI = 3.14159265358979 * 2;
		float ratio = (cos(mAccumulationTime * _2PI) + 1) / 2.f;
		float intensity = mMinIntensity * ratio + mMaxIntensity * (1 - ratio);
		
		mLightOrNull->SetIntensity(intensity);
	}

	std::shared_ptr<fq::game_module::Component> fq::client::DynamicLightHelper::Clone(std::shared_ptr<fq::game_module::Component> clone) const
	{
		auto cloned = std::dynamic_pointer_cast<DynamicLightHelper>(clone);

		if (cloned == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloned = game_module::ObjectPool::GetInstance()->Assign<DynamicLightHelper>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloned = *this;
		}

		return cloned;
	}
}