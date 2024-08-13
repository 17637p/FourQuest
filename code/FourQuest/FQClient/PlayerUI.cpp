#include "PlayerUI.h"

#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"

#include "Player.h"

fq::client::PlayerUI::PlayerUI()
	:mPlayerID(0),
	mPlayer(nullptr),
	mHPWidth(0),
	mHPBarGauge(nullptr),
	mSoulGaugeDegree(230),
	mSoulGauge(nullptr),
	mSoulIcon(nullptr),
	mWeaponIcons(),
	mSkillIconXs(),
	mSkillIconAs(),
	mSkillIconRs(),
	mScreenManager(nullptr)
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
	std::vector<fq::game_module::GameObject*> children = GetGameObject()->GetChildren();

	fq::game_module::GameObject* staminaGauge = GetGameObject()->GetChildren()[3]->GetChildren()[0];
	mHPBarGauge = staminaGauge->GetComponent<fq::game_module::ImageUI>();
	mHPWidth = mHPBarGauge->GetUIInfomations()[0].Width;

	std::vector<fq::game_module::GameObject*> souls = GetGameObject()->GetChildren()[1]->GetChildren();

	mSoulGauge = souls[1]->GetComponent<fq::game_module::ImageUI>();
	mSoulIcon = souls[2]->GetComponent<fq::game_module::ImageUI>();

	std::vector<fq::game_module::GameObject*> weapons = children[2]->GetChildren();
	mWeaponIcons.push_back(weapons[0]->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(weapons[1]->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(weapons[2]->GetComponent<fq::game_module::ImageUI>());
	mWeaponIcons.push_back(weapons[3]->GetComponent<fq::game_module::ImageUI>());

	std::vector<fq::game_module::GameObject*> skillXs = children[0]->GetChildren()[0]->GetChildren();
	mSkillIconXs.push_back(skillXs[0]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(skillXs[1]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(skillXs[2]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconXs.push_back(skillXs[3]->GetComponent<fq::game_module::ImageUI>());

	std::vector<fq::game_module::GameObject*> skillAs = children[0]->GetChildren()[1]->GetChildren();
	mSkillIconAs.push_back(skillAs[0]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[1]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[2]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[3]->GetComponent<fq::game_module::ImageUI>());

	std::vector<fq::game_module::GameObject*> skillRs = children[0]->GetChildren()[2]->GetChildren();
	mSkillIconRs.push_back(skillRs[0]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[1]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[2]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[3]->GetComponent<fq::game_module::ImageUI>());

	fq::game_module::Scene* scene = GetScene();
	SetPlayer();

	for (int i = 0; i < 4; i++)
	{
		SetWeaponAndSkillIcons(i, false);
	}

	mScreenManager = GetScene()->GetScreenManager();
}

void fq::client::PlayerUI::OnUpdate(float dt)
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	for (int i = 0; i < 4; i++)
	{
		SetWeaponAndSkillIcons(i, false);
	}

	// Scale 자동 조정 
	UINT screenWidth = mScreenManager->GetScreenWidth();
	UINT screenHeight = mScreenManager->GetScreenHeight();
	{
		myTransform->SetLocalScale({ screenWidth / (float)1920, screenHeight / (float)1080, 1 });
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
			SetHPBar(mPlayer->GetHPRatio());
			
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
			SetSoulGauge(mPlayer->GetHPRatio());
		}
	}
	else
	{
		SetSoulGauge(0);
		SetHPBar(0);
	}
}

void fq::client::PlayerUI::SetWeaponAndSkillIcons(int index, bool isRender)
{
	mWeaponIcons[index]->SetIsRender(0, isRender);
	mSkillIconXs[index]->SetIsRender(0, isRender);
	mSkillIconAs[index]->SetIsRender(0, isRender);
	mSkillIconRs[index]->SetIsRender(0, isRender);
}

void fq::client::PlayerUI::SetPlayer()
{
	fq::game_module::Scene* scene = GetScene();
	for (auto& object : scene->GetComponentView<fq::client::Player>())
	{
		auto player = object.GetComponent<fq::client::Player>();

		if (GetPlayerID() == player->GetPlayerID())
		{
			mPlayer = player;
		}
	}
}

void fq::client::PlayerUI::SetSoulGauge(float ratio)
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Soul Gauge 조정
	float soulGauge = ratio; // Test 용으로 일단 HP
	float soulDegree = mSoulGaugeDegree * soulGauge + 25;
	mSoulGauge->SetFillDegree(0, soulDegree);

	// Soul Gauge 끝에 Icon 붙이기
	float radian = DirectX::XMConvertToRadians(soulDegree + 90);
	float radius = (mSoulGauge->GetUIInfomation(0).Width / 2) * myTransform->GetLocalScale().x;

	float cos = std::cosf(radian) * radius;
	float sin = std::sinf(radian) * radius;

	mSoulIcon->GetComponent<game_module::Transform>()->SetLocalPosition({ cos, sin, 0 });
}

void fq::client::PlayerUI::SetHPBar(float ratio)
{
	float hpRatio = ratio;
	std::vector<fq::graphics::UIInfo> uiInfos = mHPBarGauge->GetUIInfomations();
	uiInfos[0].XRatio = hpRatio;
	uiInfos[0].Width = mHPWidth * hpRatio;
	mHPBarGauge->SetUIInfomations(uiInfos);
}

// HP 줄어들면 반응하기
// 갑옷에 따라 무기 아이콘, 스킬 아이콘 변화 
