#include "MagicCircleEndPoint.h"
#include "Player.h"
#include "Soul.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/UVAnimator.h"
#include "../FQGameModule/Particle.h"

std::shared_ptr<fq::game_module::Component> fq::client::MagicCircleEndPoint::Clone(std::shared_ptr<Component> clone) const
{
	std::shared_ptr<MagicCircleEndPoint> magicCircleEndPoint = std::dynamic_pointer_cast<MagicCircleEndPoint>(clone);

	if (magicCircleEndPoint == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		magicCircleEndPoint = game_module::ObjectPool::GetInstance()->Assign<MagicCircleEndPoint>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*magicCircleEndPoint = *this;
	}

	return magicCircleEndPoint;
}

void fq::client::MagicCircleEndPoint::OnStart()
{
	mTransform = GetTransform();
	mUVAnimator = GetComponent<game_module::UVAnimator>();

	mElapsed = 0.f;
	mPlayerCount = 0;
	mbIsStart = false;

	for (auto child : mTransform->GetChildren())
	{
		if (child->GetGameObject()->GetName() == "E_Gauge")
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

void fq::client::MagicCircleEndPoint::OnUpdate(float dt)
{
	if (mPlayerCount > 0)
	{
		mElapsed += dt;
	}

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
				info.InstanceData.bIsEmit = mbIsStart;
				particleObject->SetInfo(info);
			}
		}
	}

	if (mUVAnimator != nullptr)
	{
		mUVAnimator->SetTimePos(ratio * mUVAnimator->GetDuration());
	}
}

void fq::client::MagicCircleEndPoint::OnTriggerEnter(const game_module::Collision& collision)
{
	mbIsStart = true;

	if (collision.other->GetComponent<Player>() != nullptr
		|| collision.other->GetComponent<Soul>() != nullptr)
	{
		++mPlayerCount;
	}
}

void fq::client::MagicCircleEndPoint::OnTriggerExit(const game_module::Collision& collision)
{
	if (collision.other->GetComponent<Player>() != nullptr
		|| collision.other->GetComponent<Soul>() != nullptr)
	{
		--mPlayerCount;
	}
}
