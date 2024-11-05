#include "CircleEffectHelper.h"
#include "Player.h"
#include "Soul.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/UVAnimator.h"
#include "../FQGameModule/Particle.h"

std::shared_ptr<fq::game_module::Component> fq::client::CircleEffectHelper::Clone(std::shared_ptr<Component> clone) const
{
	std::shared_ptr<CircleEffectHelper> circleEffectHelper = std::dynamic_pointer_cast<CircleEffectHelper>(clone);

	if (circleEffectHelper == nullptr) // 새로 생성해서 복사본을 준다
	{
		circleEffectHelper = game_module::ObjectPool::GetInstance()->Assign<CircleEffectHelper>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*circleEffectHelper = *this;
	}

	return circleEffectHelper;
}

void fq::client::CircleEffectHelper::OnStart()
{
	mTransform = GetTransform();
	mUVAnimator = GetComponent<game_module::UVAnimator>();

	mElapsed = 0.f;

	for (auto child : mTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName().find(mPointName) != std::string::npos)
		{
			mEndPointObject = child->GetGameObject();
			break;
		}
	}

	if (mUVAnimator != nullptr)
	{
		mUVAnimator->SetTimePos(0.f);
	}
}

void fq::client::CircleEffectHelper::OnUpdate(float dt)
{
	mElapsed += dt;

	float ratio = std::min<float>(mElapsed / mDuration, 1.f);

	if (mEndPointObject != nullptr)
	{
		auto matrix = DirectX::SimpleMath::Matrix::CreateRotationY(-ratio * DirectX::XM_2PI);
		mEndPointObject->GetTransform()->SetLocalMatrix(matrix);

		auto particle = mEndPointObject->GetComponent<game_module::Particle>();

		if (particle != nullptr)
		{
			auto particleObject = particle->GetParticleObject();

			if (particleObject != nullptr)
			{
				auto info = particleObject->GetInfo();
				particleObject->SetInfo(info);
			}
		}
	}

	if (mUVAnimator != nullptr)
	{
		mUVAnimator->SetTimePos(ratio * mUVAnimator->GetDuration());
	}
}