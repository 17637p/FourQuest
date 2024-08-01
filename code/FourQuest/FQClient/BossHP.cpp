#include "BossHP.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/ImageUI.h"

#include "BossMonster.h"

std::shared_ptr<fq::game_module::Component> fq::client::BossHP::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneBossHP = std::dynamic_pointer_cast<BossHP>(clone);

	if (cloneBossHP == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		cloneBossHP = game_module::ObjectPool::GetInstance()->Assign<BossHP>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*cloneBossHP = *this;
	}

	return cloneBossHP;
}

void fq::client::BossHP::OnStart()
{
	game_module::Transform* myTransform = GetGameObject()->GetComponent<game_module::Transform>();
	mHPBarGauge = myTransform->GetChildren()[0]->GetComponent<fq::game_module::ImageUI>();
	mHPWidth = mHPBarGauge->GetUIInfomations()[0].Width;
}

void fq::client::BossHP::OnUpdate(float dt)
{
	// HP �� ����
	//float hpRatio = mBoss->GetHPRatio();
	float hpRatio = 0.2f;
	std::vector<fq::graphics::UIInfo> uiInfos = mHPBarGauge->GetUIInfomations();
	uiInfos[0].XRatio = hpRatio;
	uiInfos[0].Width = mHPWidth * hpRatio;
	mHPBarGauge->SetUIInfomations(uiInfos);
}

fq::client::BossHP::BossHP()
	:mHPWidth(0),
	mHPBarGauge(nullptr),
	mBoss(nullptr),
	mScreenManager(nullptr)
{

}

fq::client::BossHP::~BossHP()
{
}
