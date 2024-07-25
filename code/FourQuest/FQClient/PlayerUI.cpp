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

		// ���� Ÿ�� �޾ƿ��� 
		// ���� ������, ��ų ������ ��ȭ
		fq::client::EArmourType armourType = mPlayer->GetArmourType();

		switch (armourType)
		{
			case fq::client::EArmourType::Knight:
				break;
			case fq::client::EArmourType::Magic:
				break;
			case fq::client::EArmourType::Warrior:
				break;
			case fq::client::EArmourType::Archer:
				break;
			default:
				break;
		}
	}
}

// HP �پ��� �����ϱ�
// ���ʿ� ���� ���� ������, ��ų ������ ��ȭ 
