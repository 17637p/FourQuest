#include "PlayerUI.h"

#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/CharacterController.h"

#include "GameManager.h"
#include "Player.h"
#include "Soul.h"
#include "SoulVariable.h"
#include "GameManager.h"

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
	mPlayerState(),
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
	mWeaponIcons.clear();
	mSkillIconXs.clear();
	mSkillIconAs.clear();
	mSkillIconRs.clear();
	mSoulSkillIcons.clear();

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
	mXCoolTimeImage = skillXs[4]->GetComponent<fq::game_module::ImageUI>();

	std::vector<fq::game_module::GameObject*> skillAs = children[0]->GetChildren()[1]->GetChildren();
	mSkillIconAs.push_back(skillAs[0]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[1]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[2]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconAs.push_back(skillAs[3]->GetComponent<fq::game_module::ImageUI>());
	mACoolTimeImage = skillAs[4]->GetComponent<fq::game_module::ImageUI>();

	std::vector<fq::game_module::GameObject*> skillRs = children[0]->GetChildren()[2]->GetChildren();
	mSkillIconRs.push_back(skillRs[0]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[1]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[2]->GetComponent<fq::game_module::ImageUI>());
	mSkillIconRs.push_back(skillRs[3]->GetComponent<fq::game_module::ImageUI>());

	std::vector<fq::game_module::GameObject*> soulIcons = children[1]->GetChildren()[0]->GetChildren();
	mSoulSkillIcons.push_back(soulIcons[0]->GetComponent<fq::game_module::ImageUI>());
	mSoulSkillIcons.push_back(soulIcons[1]->GetComponent<fq::game_module::ImageUI>());
	mSoulSkillIcons.push_back(soulIcons[2]->GetComponent<fq::game_module::ImageUI>());
	mSoulSkillIcons.push_back(soulIcons[3]->GetComponent<fq::game_module::ImageUI>());

	mRCoolTimeImage = skillRs[4]->GetComponent<fq::game_module::ImageUI>();
	mCoolTimeHeight = mRCoolTimeImage->GetUIInfomation(0).Height;

	if (children.size() > 4)
	{
		fq::game_module::GameObject* playerState = children[4];
		mPlayerState = playerState->GetComponent<fq::game_module::ImageUI>();
	}

	for (int i = 0; i < 4; i++)
	{
		setWeaponAndSkillIcons(i, false);
	}

	mScreenManager = GetScene()->GetScreenManager();
	setSoulSkillIcon();
}

void fq::client::PlayerUI::OnUpdate(float dt)
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	for (int i = 0; i < 4; i++)
	{
		setWeaponAndSkillIcons(i, false);
	}

	// Scale 자동 조정 
	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
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
			setWeaponAndSkillIcons(armourTypeIndex, true);
			SetSoulGauge(mPlayer->GetSoultGaugeRatio());
			setSkillCoolTime();
		}
	}
	else if (mSoul) // 소울 상태 설정 
	{
		if (mSoul->GetGameObject()->IsDestroyed())
		{
			mSoul = nullptr;
		}
		else
		{
			SetHPBar(mSoul->GetSoulHpRatio());
			SetSoulGauge(0);
			resetSkillCoolTime();
		}
	}
	else
	{
		SetSoulGauge(0);
		SetHPBar(0);
		resetSkillCoolTime();
	}

	// 플레이어 상태 UI 위치조정 및 렌더러
	SetPlayerStateUpdate();
}

void fq::client::PlayerUI::setWeaponAndSkillIcons(int index, bool isRender)
{
	mWeaponIcons[index]->SetIsRender(0, isRender);
	mSkillIconXs[index]->SetIsRender(0, isRender);
	mSkillIconAs[index]->SetIsRender(0, isRender);
	mSkillIconRs[index]->SetIsRender(0, isRender);
}

void fq::client::PlayerUI::SetPlayerStateUpdate()
{
	if (mPlayerState == nullptr)
		return;

	if (mPlayerState->GetUIInfomations().size() <= 5)
		return;

	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// UI 위치 조정
	for (int i = 0; i < mPlayerState->GetUIInfomations().size(); i++)
	{
		float localX = mPlayerState->GetGameObject()->GetComponent<fq::game_module::Transform>()->GetLocalPosition().x;
		float localY = mPlayerState->GetGameObject()->GetComponent<fq::game_module::Transform>()->GetLocalPosition().y;

		mPlayerState->SetUIPosition(i, myTransform->GetWorldPosition().x + localX, myTransform->GetWorldPosition().y + localY);

		UINT screenWidth = mScreenManager->GetFixScreenWidth();
		UINT screenHeight = mScreenManager->GetFixScreenHeight();
		mPlayerState->SetUIScale(i, screenWidth / (float)1920, screenHeight / (float)1080);
	}

	bool isRetire = false;
	bool isDestroyArmour = false;

	// Player의 상태 체크
	if (mPlayerID == 0)
	{
		if (SoulVariable::Player1Type == EPlayerType::SoulDestoryed)
			isRetire = true;

		if (SoulVariable::Player1Type == EPlayerType::ArmourDestroyed)
			isDestroyArmour = true;
	}
	else if (mPlayerID == 1)
	{
		if (SoulVariable::Player2Type == EPlayerType::SoulDestoryed)
			isRetire = true;

		if (SoulVariable::Player2Type == EPlayerType::ArmourDestroyed)
			isDestroyArmour = true;
	}
	else if (mPlayerID == 2)
	{
		if (SoulVariable::Player3Type == EPlayerType::SoulDestoryed)
			isRetire = true;

		if (SoulVariable::Player3Type == EPlayerType::ArmourDestroyed)
			isDestroyArmour = true;
	}
	else if (mPlayerID == 3)
	{
		if (SoulVariable::Player4Type == EPlayerType::SoulDestoryed)
			isRetire = true;

		if (SoulVariable::Player4Type == EPlayerType::ArmourDestroyed)
			isDestroyArmour = true;
	}

	// 갑옷 파괴 UI
	if (isDestroyArmour)
	{
		for (auto& manager : GetScene()->GetComponentView<GameManager>())
		{
			float delayTime = manager.GetComponent<GameManager>()->GetDestoryArmourSoulDelayTime(mPlayerID);

			if (SoulVariable::OutTime - delayTime < 1.f)
			{
				mPlayerState->SetIsRender(0, true);
				mPlayerState->SetIsRender(1, false);
				mPlayerState->SetIsRender(2, false);
				mPlayerState->SetIsRender(3, false);
				mPlayerState->SetIsRender(4, false);
				mPlayerState->SetIsRender(5, false);
			}
			else if (SoulVariable::OutTime - delayTime < 2.f)
			{
				mPlayerState->SetIsRender(0, false);
				mPlayerState->SetIsRender(1, true);
				mPlayerState->SetIsRender(2, false);
				mPlayerState->SetIsRender(3, false);
				mPlayerState->SetIsRender(4, false);
				mPlayerState->SetIsRender(5, false);
			}
			else if (SoulVariable::OutTime - delayTime < 3.f)
			{
				mPlayerState->SetIsRender(0, false);
				mPlayerState->SetIsRender(1, false);
				mPlayerState->SetIsRender(2, true);
				mPlayerState->SetIsRender(3, false);
				mPlayerState->SetIsRender(4, false);
				mPlayerState->SetIsRender(5, false);
			}
			else if (SoulVariable::OutTime - delayTime < 4.f)
			{
				mPlayerState->SetIsRender(0, false);
				mPlayerState->SetIsRender(1, false);
				mPlayerState->SetIsRender(2, false);
				mPlayerState->SetIsRender(3, true);
				mPlayerState->SetIsRender(4, false);
				mPlayerState->SetIsRender(5, false);
			}
			else if (SoulVariable::OutTime - delayTime < 5.f)
			{
				mPlayerState->SetIsRender(0, false);
				mPlayerState->SetIsRender(1, false);
				mPlayerState->SetIsRender(2, false);
				mPlayerState->SetIsRender(3, false);
				mPlayerState->SetIsRender(4, true);
				mPlayerState->SetIsRender(5, false);
			}
		}
	}
	else
	{
		mPlayerState->SetIsRender(0, false);
		mPlayerState->SetIsRender(1, false);
		mPlayerState->SetIsRender(2, false);
		mPlayerState->SetIsRender(3, false);
		mPlayerState->SetIsRender(4, false);
	}

	// 영혼 파괴 UI
	if (isRetire)
	{
		mPlayerState->SetIsRender(5, true);
	}
	else
	{
		mPlayerState->SetIsRender(5, false);
	}
}

void fq::client::PlayerUI::SetPlayer(fq::client::GameManager* gameMgr)
{
	/*fq::game_module::Scene* scene = GetScene();
	for (auto& object : scene->GetComponentView<fq::client::Player>())
	{
		auto player = object.GetComponent<fq::client::Player>();

		if (GetPlayerID() == player->GetPlayerID())
		{
			mPlayer = player;
		}
	}*/

	// 플레이어 연결 로직 수정 
	for (auto& playerObject : gameMgr->GetPlayers())
	{
		auto id = playerObject->GetComponent<game_module::CharacterController>()->GetControllerID();

		if (GetPlayerID() == id)
		{
			if (playerObject->HasComponent<Soul>())
			{
				mPlayer = nullptr;
				mSoul = playerObject->GetComponent<Soul>();
			}
			else if (playerObject->HasComponent<Player>())
			{
				mPlayer = playerObject->GetComponent<Player>();
				mSoul = nullptr;
			}
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
	float radius = mSoulGauge->GetUIInfomation(0).Width / 2;// *myTransform->GetLocalScale().x; // world나 local 이나 똑같은 값

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

void fq::client::PlayerUI::setSkillCoolTime()
{
	float aCool = mPlayer->GetASkillCoolTimeRatio();
	float rCool = mPlayer->GetRSkillCoolTimeRatio();
	float xCool = mPlayer->GetXSkillCoolTimeRatio();;

	float coolX = -25;
	float coolY = -25;

	auto uiInfo = mACoolTimeImage->GetUIInfomation(0);
	uiInfo.Height = mCoolTimeHeight * aCool;
	mACoolTimeImage->SetUIInfomation(0, uiInfo);
	mACoolTimeImage->GetTransform()->SetLocalPosition({ coolX, coolY + (50 - uiInfo.Height) , 0 });

	uiInfo = mRCoolTimeImage->GetUIInfomation(0);
	uiInfo.Height = mCoolTimeHeight * rCool;
	mRCoolTimeImage->SetUIInfomation(0, uiInfo);
	mRCoolTimeImage->GetTransform()->SetLocalPosition({ coolX, coolY + (50 - uiInfo.Height) , 0 });

	uiInfo = mXCoolTimeImage->GetUIInfomation(0);
	uiInfo.Height = mCoolTimeHeight * xCool;
	mXCoolTimeImage->SetUIInfomation(0, uiInfo);
	mXCoolTimeImage->GetTransform()->SetLocalPosition({ coolX, coolY + (50 - uiInfo.Height) , 0 });
}

void fq::client::PlayerUI::resetSkillCoolTime()
{
	float aCool = 0.f;
	float rCool = 0.f;
	float xCool = 0.f;

	float coolX = -25;
	float coolY = -25;

	auto uiInfo = mACoolTimeImage->GetUIInfomation(0);
	uiInfo.Height = mCoolTimeHeight * aCool;
	mACoolTimeImage->SetUIInfomation(0, uiInfo);
	mACoolTimeImage->GetTransform()->SetLocalPosition({ coolX, coolY + (50 - uiInfo.Height) , 0 });

	uiInfo = mRCoolTimeImage->GetUIInfomation(0);
	uiInfo.Height = mCoolTimeHeight * rCool;
	mRCoolTimeImage->SetUIInfomation(0, uiInfo);
	mRCoolTimeImage->GetTransform()->SetLocalPosition({ coolX, coolY + (50 - uiInfo.Height) , 0 });

	uiInfo = mXCoolTimeImage->GetUIInfomation(0);
	uiInfo.Height = mCoolTimeHeight * xCool;
	mXCoolTimeImage->SetUIInfomation(0, uiInfo);
	mXCoolTimeImage->GetTransform()->SetLocalPosition({ coolX, coolY + (50 - uiInfo.Height) , 0 });
}

void fq::client::PlayerUI::setSoulSkillIcon()
{
	int soulType = -1;
	if (mPlayer != nullptr)
	{
		soulType = static_cast<int>(mPlayer->GetSoulType());
	}
	if (mSoul != nullptr)
	{
		soulType = static_cast<int>(mSoul->GetSoulType());
	}

	for (int i = 0; i < 4; i++)
	{
		mSoulSkillIcons[i]->SetIsRender(0, false);
	}

	if (soulType != -1)
	{
		mSoulSkillIcons[soulType]->SetIsRender(0, true);
	}
}

