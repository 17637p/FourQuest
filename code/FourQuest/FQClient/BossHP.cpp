#include "BossHP.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"

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

	// HP Bar
	mHPBarGauge = myTransform->GetChildren()[0]->GetComponent<fq::game_module::ImageUI>();
	mHPWidth = mHPBarGauge->GetUIInfomations()[0].Width;

	// Shield Bar
	mShieldGauge = GetGameObject()->GetChildren()[1]->GetChildren()[0]->GetComponent <game_module::ImageUI>();
	mShieldWidth = mShieldGauge->GetUIInfomations()[0].Width;

	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::BossHP::OnUpdate(float dt)
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Scale �ڵ� ���� 
	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}

	game_module::ImageUI* myImage = GetComponent<game_module::ImageUI>();
	// Position �ڵ� ����
	{
		myTransform->SetLocalPosition({ (screenWidth / 2) - ((myImage->GetUIInfomation(0).Width / 2) * scaleX), 
			screenHeight / (float)1080, 1 });
	}

	// HP �� ����
	float gaugeRatio = mBoss->GetHPRatio();
	std::vector<fq::graphics::UIInfo> uiInfos = mHPBarGauge->GetUIInfomations();
	uiInfos[0].XRatio = gaugeRatio;
	uiInfos[0].Width = mHPWidth * gaugeRatio;
	mHPBarGauge->SetUIInfomations(uiInfos);

	gaugeRatio = mBoss->GetHPRatio();
	uiInfos = mShieldGauge->GetUIInfomations();
	uiInfos[0].XRatio = gaugeRatio;
	uiInfos[0].Width = mShieldWidth * gaugeRatio;
	mShieldGauge->SetUIInfomations(uiInfos);
}

fq::client::BossHP::BossHP()
	:mHPWidth(0),
	mHPBarGauge(nullptr),
	mBoss(nullptr),
	mScreenManager(nullptr),
	mShieldWidth(0),
	mShieldGauge(nullptr)
{

}

fq::client::BossHP::~BossHP()
{
}
