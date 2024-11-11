#include "KDInfo.h"

#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"

#include "PlayerInfoVariable.h"
#include "PlayerSoulVariable.h"
#include "ClientEvent.h"

fq::client::KDInfo::~KDInfo()
{

}

fq::client::KDInfo::KDInfo()
	:mScreenManager(nullptr)
	, mPlayers()
{
}

fq::client::KDInfo::KDInfo(const KDInfo& other)
{

}

fq::client::KDInfo& fq::client::KDInfo::operator=(const KDInfo& other)
{
	return *this;
}

std::shared_ptr<fq::game_module::Component> fq::client::KDInfo::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneKDInfo = std::dynamic_pointer_cast<KDInfo>(clone);

	if (cloneKDInfo == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneKDInfo = game_module::ObjectPool::GetInstance()->Assign<KDInfo>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneKDInfo = *this;
	}

	return cloneKDInfo;
}

void fq::client::KDInfo::OnUpdate(float dt)
{
	setScaleScreen();

	int alivePlayerCount = 0;
	for (int i = 0; i < 4; i++)
	{
		if (mIsAlives[i])
		{
			alivePlayerCount++;
		}
		mKillText[i]->SetText(std::to_string(mKillCount[i]) + " KILL");
		mDeathText[i]->SetText(std::to_string(mDeathCount[i]) + " Death");
	}
	setPlayerPosition(alivePlayerCount);
}

void fq::client::KDInfo::OnStart()
{
	eventProcessPlayerArmourDeath();

	mScreenManager = GetScene()->GetScreenManager();

	mPlayers.clear();
	mKillCount.clear();
	mDeathCount.clear();
	mKillText.clear();
	mDeathText.clear();
	mIsAlives.clear();
	mSoulColors.clear();

	mSoulColors.push_back(PlayerSoulVariable::SwordSoulColor);
	mSoulColors.push_back(PlayerSoulVariable::StaffSoulColor);
	mSoulColors.push_back(PlayerSoulVariable::AxeSoulColor);
	mSoulColors.push_back(PlayerSoulVariable::BowSoulColor);

	auto children = GetGameObject()->GetChildren();
	mPlayers.push_back(children[0]->GetComponent<game_module::ImageUI>());
	mPlayers.push_back(children[1]->GetComponent<game_module::ImageUI>());
	mPlayers.push_back(children[2]->GetComponent<game_module::ImageUI>());
	mPlayers.push_back(children[3]->GetComponent<game_module::ImageUI>());

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			mPlayers[i]->SetIsRender(j, false);
		}
	}

	for (int i = 0; i < 4; i++)
	{
		mKillText.push_back(mPlayers[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>());
		mDeathText.push_back(mPlayers[i]->GetGameObject()->GetChildren()[1]->GetComponent<game_module::TextUI>());
		mKillText[i]->SetIsRender(false);
		mDeathText[i]->SetIsRender(false);

		mKillCount.push_back(0);
		mDeathCount.push_back(0);
	}

	if (PlayerInfoVariable::Player1SoulType != -1)
	{
		mIsAlives.push_back(true);
		//mPlayers[0]->SetIsRender(PlayerInfoVariable::Player1SoulType, true);
		mKillText[0]->SetIsRender(true);
		mDeathText[0]->SetIsRender(true);

		auto textInfo = mKillText[0]->GetTextInfo();
		textInfo.FontColor = mSoulColors[PlayerInfoVariable::Player1SoulType];
		mKillText[0]->SetTextInfo(textInfo);

		textInfo = mDeathText[0]->GetTextInfo();
		textInfo.FontColor = mSoulColors[PlayerInfoVariable::Player1SoulType];
		mDeathText[0]->SetTextInfo(textInfo);
	}
	else
	{
		mIsAlives.push_back(false);
	}
	if (PlayerInfoVariable::Player2SoulType != -1)
	{
		mIsAlives.push_back(true);
		//mPlayers[1]->SetIsRender(PlayerInfoVariable::Player2SoulType, true);
		mKillText[1]->SetIsRender(true);
		mDeathText[1]->SetIsRender(true);

		auto textInfo = mKillText[1]->GetTextInfo();
		textInfo.FontColor = mSoulColors[PlayerInfoVariable::Player2SoulType];
		mKillText[1]->SetTextInfo(textInfo);

		textInfo = mDeathText[1]->GetTextInfo();
		textInfo.FontColor = mSoulColors[PlayerInfoVariable::Player2SoulType];
		mDeathText[1]->SetTextInfo(textInfo);
	}
	else
	{
		mIsAlives.push_back(false);
	}
	if (PlayerInfoVariable::Player3SoulType != -1)
	{
		mIsAlives.push_back(true);
		//mPlayers[2]->SetIsRender(PlayerInfoVariable::Player3SoulType, true);
		mKillText[2]->SetIsRender(true);
		mDeathText[2]->SetIsRender(true);

		auto textInfo = mKillText[2]->GetTextInfo();
		textInfo.FontColor = mSoulColors[PlayerInfoVariable::Player3SoulType];
		mKillText[2]->SetTextInfo(textInfo);

		textInfo = mDeathText[2]->GetTextInfo();
		textInfo.FontColor = mSoulColors[PlayerInfoVariable::Player3SoulType];
		mDeathText[2]->SetTextInfo(textInfo);
	}
	else
	{
		mIsAlives.push_back(false);
	}
	if (PlayerInfoVariable::Player4SoulType != -1)
	{
		mIsAlives.push_back(true);
		//mPlayers[3]->SetIsRender(PlayerInfoVariable::Player4SoulType, true);
		mKillText[3]->SetIsRender(true);
		mDeathText[3]->SetIsRender(true);

		auto textInfo = mKillText[3]->GetTextInfo();
		textInfo.FontColor = mSoulColors[PlayerInfoVariable::Player4SoulType];
		mKillText[3]->SetTextInfo(textInfo);

		textInfo = mDeathText[3]->GetTextInfo();
		textInfo.FontColor = mSoulColors[PlayerInfoVariable::Player4SoulType];
		mDeathText[3]->SetTextInfo(textInfo);
	}
	else
	{
		mIsAlives.push_back(false);
	}
}

void fq::client::KDInfo::setScaleScreen()
{
	// 화면 크기에 따른 Scale 자동 조정 
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}

void fq::client::KDInfo::setPlayerPosition(int playerCount)
{
	int screenWidth = mScreenManager->GetFixScreenWidth();
	int screenHeight = mScreenManager->GetFixScreenHeight();
	float mHeightRatio = 0.7f;
	if (playerCount == 1)
	{
		mPlayers[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)2,
			screenHeight * mHeightRatio, 0 });
	}
	else if (playerCount == 2)
	{
		mPlayers[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)3,
			screenHeight * mHeightRatio, 0 });
		mPlayers[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)3 * 2,
			screenHeight * mHeightRatio, 0 });
	}
	else if (playerCount == 3)
	{
		mPlayers[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)4,
			screenHeight * mHeightRatio, 0 });
		mPlayers[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)4 * 2,
			screenHeight * mHeightRatio, 0 });
		mPlayers[2]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)4 * 3,
			screenHeight * mHeightRatio, 0 });
	}
	else if (playerCount == 4)
	{
		mPlayers[0]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)5,
			screenHeight * mHeightRatio, 0 });
		mPlayers[1]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)5 * 2,
			screenHeight * mHeightRatio, 0 });
		mPlayers[2]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)5 * 3,
			screenHeight * mHeightRatio, 0 });
		mPlayers[3]->GetComponent<game_module::Transform>()->SetLocalPosition({
			screenWidth / (float)5 * 4,
			screenHeight * mHeightRatio, 0 });
	}
}

void fq::client::KDInfo::eventProcessPlayerArmourDeath()
{
	mPlayerArmourDeathHandler = GetScene()->GetEventManager()->RegisterHandle<client::event::PlayerArmourDeath>
		(
			[this](const client::event::PlayerArmourDeath& event)
			{
				mKillCount[event.killPlayerID]++;
				mDeathCount[event.deathPlayerID]++;
			}
		);
}

void fq::client::KDInfo::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mPlayerArmourDeathHandler);
}

