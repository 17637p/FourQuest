#include "PlayerUI.h"

#include "../FQGameModule/ImageUI.h"
#include "Player.h"

fq::client::PlayerUI::PlayerUI()
	:mPlayerID(0)
{

}

fq::client::PlayerUI::~PlayerUI()
{

}

std::shared_ptr<fq::game_module::Component> fq::client::PlayerUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto clonePlayerUI = std::dynamic_pointer_cast<PlayerUI>(clone);

	if (clonePlayerUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		clonePlayerUI = game_module::ObjectPool::GetInstance()->Assign<PlayerUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*clonePlayerUI = *this;
	}

	return clonePlayerUI;
}

void fq::client::PlayerUI::OnStart()
{
	fq::game_module::Scene* scene = GetScene();

	mHPBarGauge = scene->GetObjectByName("StaminaGauge")->GetComponent<fq::game_module::ImageUI>();
	mHPWidth = mHPBarGauge->GetUIInfomations()[0].Width;
	mWeaponIcon = scene->GetObjectByName("WeaponIcon")->GetComponent<fq::game_module::ImageUI>();
	mSkillIconX = scene->GetObjectByName("SkillIconX")->GetComponent<fq::game_module::ImageUI>();
	mSkillIconA = scene->GetObjectByName("SkillIconA")->GetComponent<fq::game_module::ImageUI>();
	mSkillIconR = scene->GetObjectByName("SkillIconR")->GetComponent<fq::game_module::ImageUI>();

	for (auto& object : scene->GetComponentView<fq::client::Player>())
	{
		auto player = object.GetComponent<fq::client::Player>();

		if (mPlayerID == player->GetPlayerID())
		{
			mPlayer = player;
		}
	}
}

void fq::client::PlayerUI::OnUpdate(float dt)
{
	// �÷��̾����� HP �޾ƿͼ� Ratio �����ϱ�
	if (mPlayer != nullptr)
	{
		float hpRatio = mPlayer->GetHPRatio();
		std::vector<fq::graphics::UIInfo> uiInfos = mHPBarGauge->GetUIInfomations();
		uiInfos[0].XRatio = hpRatio;
		uiInfos[0].Width = mHPWidth * hpRatio;
		mHPBarGauge->SetUIInfomations(uiInfos);
	}

	// ���� Ÿ�� �޾ƿ��� 
	// ���� ������, ��ų ������ ��ȭ
}

// HP �پ��� �����ϱ�
// ���ʿ� ���� ���� ������, ��ų ������ ��ȭ 
