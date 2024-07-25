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

	mWeaponIcons.push_back(scene->GetObjectByName("KnightWeaponIcon")->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(scene->GetObjectByName("ArcherWeaponIcon")->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(scene->GetObjectByName("MagicWeaponIcon")->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(scene->GetObjectByName("WarriorWeaponIcon")->GetComponent<fq::game_module::ImageUI>());

	mSkillIconXs.push_back(scene->GetObjectByName("KnightSkillIconX")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(scene->GetObjectByName("ArcherSkillIconX")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(scene->GetObjectByName("MagicSkillIconX")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(scene->GetObjectByName("WarriorSkillIconX")->GetComponent<fq::game_module::ImageUI>());

	mSkillIconAs.push_back(scene->GetObjectByName("KnightSkillIconA")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(scene->GetObjectByName("ArcherSkillIconA")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(scene->GetObjectByName("MagicSkillIconA")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(scene->GetObjectByName("WarriorSkillIconA")->GetComponent<fq::game_module::ImageUI>());

	mSkillIconRs.push_back(scene->GetObjectByName("KnightSkillIconR")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(scene->GetObjectByName("ArcherSkillIconR")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(scene->GetObjectByName("MagicSkillIconR")->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(scene->GetObjectByName("WarriorSkillIconR")->GetComponent<fq::game_module::ImageUI>());

	for (auto& object : scene->GetComponentView<fq::client::Player>())
	{
		auto player = object.GetComponent<fq::client::Player>();

		if (mPlayerID == player->GetPlayerID())
		{
			mPlayer = player;
		}
	}

	for (int i = 0; i < 4; i++)
	{
		mWeaponIcons[i]->SetIsRender(0, false);
		mSkillIconXs[i]->SetIsRender(0, false);
		mSkillIconAs[i]->SetIsRender(0, false);
		mSkillIconRs[i]->SetIsRender(0, false);
	}
}

void fq::client::PlayerUI::OnUpdate(float dt)
{
	// �÷��̾����� HP �޾ƿͼ� Ratio �����ϱ�
	if (mPlayer != nullptr)
	{
		// HP �� ����
		float hpRatio = mPlayer->GetHPRatio();
		std::vector<fq::graphics::UIInfo> uiInfos = mHPBarGauge->GetUIInfomations();
		uiInfos[0].XRatio = hpRatio;
		uiInfos[0].Width = mHPWidth * hpRatio;
		mHPBarGauge->SetUIInfomations(uiInfos);

		for (int i = 0; i < 4; i++)
		{
			mWeaponIcons[i]->SetIsRender(0, false);
			mSkillIconXs[i]->SetIsRender(0, false);
			mSkillIconAs[i]->SetIsRender(0, false);
			mSkillIconRs[i]->SetIsRender(0, false);
		}

		// ���� Ÿ�� �޾ƿ��� 
		// ���� ������, ��ų ������ ��ȭ
		fq::client::EArmourType armourType = mPlayer->GetArmourType();
		int armourTypeIndex = -1;

		switch (armourType)
		{
			case fq::client::EArmourType::Knight:
				armourTypeIndex = 0;
				break;
			case fq::client::EArmourType::Magic:
				armourTypeIndex = 1;
				break;
			case fq::client::EArmourType::Warrior:
				armourTypeIndex = 3;
				break;
			case fq::client::EArmourType::Archer:
				armourTypeIndex = 2;
				break;
			default:
				break;
		}

		mWeaponIcons[armourTypeIndex]->SetIsRender(0, true);
		mSkillIconXs[armourTypeIndex]->SetIsRender(0, true);
		mSkillIconAs[armourTypeIndex]->SetIsRender(0, true);
		mSkillIconRs[armourTypeIndex]->SetIsRender(0, true);
	}
}

// HP �پ��� �����ϱ�
// ���ʿ� ���� ���� ������, ��ų ������ ��ȭ 
