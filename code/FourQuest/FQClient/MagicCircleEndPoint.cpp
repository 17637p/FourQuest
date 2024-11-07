#include "MagicCircleEndPoint.h"
#include "Player.h"
#include "Soul.h"
#include "DefenceCounter.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/UVAnimator.h"
#include "../FQGameModule/Particle.h"

std::shared_ptr<fq::game_module::Component> fq::client::MagicCircleEndPoint::Clone(std::shared_ptr<Component> clone) const
{
	std::shared_ptr<MagicCircleEndPoint> magicCircleEndPoint = std::dynamic_pointer_cast<MagicCircleEndPoint>(clone);

	if (magicCircleEndPoint == nullptr) // 새로 생성해서 복사본을 준다
	{
		magicCircleEndPoint = game_module::ObjectPool::GetInstance()->Assign<MagicCircleEndPoint>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*magicCircleEndPoint = *this;
	}

	return magicCircleEndPoint;
}

void fq::client::MagicCircleEndPoint::OnStart()
{
	mTransform = GetTransform();
	mUVAnimator = GetComponent<game_module::UVAnimator>();

	for (auto child : mTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName().find("E_Gauge") != std::string::npos)
		{
			mEndPointObject = child->GetGameObject();
			break;
		}
	}

	if (mUVAnimator != nullptr)
	{
		mUVAnimator->SetTimePos(0.f);
	}

	mDefenceCounterObjectOrNull = GetScene()->GetObjectByName(mDefenceCounterObjectName);
}

void fq::client::MagicCircleEndPoint::OnUpdate(float dt)
{
	if (mDefenceCounterObjectOrNull == nullptr)
	{
		return;
	}
	if (mDefenceCounterObjectOrNull->IsDestroyed())
	{
		mDefenceCounterObjectOrNull = nullptr;
		return;
	}

	float ratio = 0;

	if (mDefenceCounterObjectOrNull != nullptr)
	{
		auto defenceCounter = mDefenceCounterObjectOrNull->GetComponent<DefenceCounter>();

		if (defenceCounter != nullptr)
		{
			ratio = defenceCounter->GetCountRatio();
		}
	}

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
				info.InstanceData.bIsEmit = ratio > 0.f;
				particleObject->SetInfo(info);
			}
		}
	}

	if (mUVAnimator != nullptr)
	{
		mUVAnimator->SetTimePos(ratio * mUVAnimator->GetDuration());
	}
}
