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

	if (clonePlayerUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		clonePlayerUI = game_module::ObjectPool::GetInstance()->Assign<PlayerUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*clonePlayerUI = *this;
	}

	return clonePlayerUI;
}

void fq::client::PlayerUI::OnStart()
{
	fq::game_module::Scene* scene = GetScene();

	mHPBarGauge = scene->GetObjectByName("StaminaGauge")->GetComponent<fq::game_module::ImageUI>();
	mHPWidth = mHPBarGauge->GetUIInfomations()[0].Width;

	std::vector<fq::game_module::GameObject*> weapons = GetGameObject()->GetChildren()[2]->GetChildren();
	mWeaponIcons.push_back(weapons[0]->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(weapons[1]->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(weapons[2]->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(weapons[3]->GetComponent<fq::game_module::ImageUI>());

	std::vector<fq::game_module::GameObject*> skillXs = GetGameObject()->GetChildren()[0]->GetChildren()[0]->GetChildren();
	mSkillIconXs.push_back(skillXs[0]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(skillXs[1]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(skillXs[2]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(skillXs[3]->GetComponent<fq::game_module::ImageUI>());

	std::vector<fq::game_module::GameObject*> skillAs = GetGameObject()->GetChildren()[0]->GetChildren()[5]->GetChildren();
	mSkillIconAs.push_back(skillAs[0]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[1]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[2]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[3]->GetComponent<fq::game_module::ImageUI>());

	std::vector<fq::game_module::GameObject*> skillRs = GetGameObject()->GetChildren()[0]->GetChildren()[4]->GetChildren();
	mSkillIconRs.push_back(skillRs[0]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[1]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[2]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[3]->GetComponent<fq::game_module::ImageUI>());

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
		SetWeaponAndSkillIcons(i, false);
	}
}

void fq::client::PlayerUI::OnUpdate(float dt)
{
	for (int i = 0; i < 4; i++)
	{
		SetWeaponAndSkillIcons(i, false);
	}

	// 플레이어한테 HP 받아와서 Ratio 조절하기
	if (mPlayer != nullptr)
	{
		if (mPlayer->GetGameObject()->IsDestroyed())
		{
			mPlayer = nullptr;
		}

		if (mPlayer != nullptr)
		{
			// HP 바 조정
			float hpRatio = mPlayer->GetHPRatio();
			std::vector<fq::graphics::UIInfo> uiInfos = mHPBarGauge->GetUIInfomations();
			uiInfos[0].XRatio = hpRatio;
			uiInfos[0].Width = mHPWidth * hpRatio;
			mHPBarGauge->SetUIInfomations(uiInfos);

			// 갑옷 타입 받아오기 
			// 무기 아이콘, 스킬 아이콘 변화
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

			SetWeaponAndSkillIcons(armourTypeIndex, true);
		}
	}
}

void fq::client::PlayerUI::SetWeaponAndSkillIcons(int index, bool isRender)
{
	mWeaponIcons[index]->SetIsRender(0, isRender);
	mSkillIconXs[index]->SetIsRender(0, isRender);
	mSkillIconAs[index]->SetIsRender(0, isRender);
	mSkillIconRs[index]->SetIsRender(0, isRender);
}

// HP 줄어들면 반응하기
// 갑옷에 따라 무기 아이콘, 스킬 아이콘 변화 
