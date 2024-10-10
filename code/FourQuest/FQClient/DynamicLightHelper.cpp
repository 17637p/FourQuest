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

		// 1�� �ֱ�� min ~ max ~ min���� ���ƿ�
		const double _2PI = 3.14159265358979 * 2;
		float ratio = (cos(mAccumulationTime * _2PI) + 1) / 2.f;
		float intensity = mMinIntensity * ratio + mMaxIntensity * (1 - ratio);
		
		mLightOrNull->SetIntensity(intensity);
	}

	std::shared_ptr<fq::game_module::Component> fq::client::DynamicLightHelper::Clone(std::shared_ptr<fq::game_module::Component> clone) const
	{
		auto cloned = std::dynamic_pointer_cast<DynamicLightHelper>(clone);

		if (cloned == nullptr) // ���� �����ؼ� ���纻�� �ش�
		{
			cloned = game_module::ObjectPool::GetInstance()->Assign<DynamicLightHelper>(*this);
		}
		else // clone�� �����͸� �����Ѵ�.
		{
			// �⺻ ���� ������ ȣ���Ѵ�.
			*cloned = *this;
		}

		return cloned;
	}
}