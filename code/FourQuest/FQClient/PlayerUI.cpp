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
	// 플레이어한테 HP 받아와서 Ratio 조절하기
	if (mPlayer != nullptr)
	{
		float hpRatio = mPlayer->GetHPRatio();
		std::vector<fq::graphics::UIInfo> uiInfos = mHPBarGauge->GetUIInfomations();
		uiInfos[0].XRatio = hpRatio;
		uiInfos[0].Width = mHPWidth * hpRatio;
		mHPBarGauge->SetUIInfomations(uiInfos);
	}

	// 갑옷 타입 받아오기 
	// 무기 아이콘, 스킬 아이콘 변화
}

// HP 줄어들면 반응하기
// 갑옷에 따라 무기 아이콘, 스킬 아이콘 변화 
