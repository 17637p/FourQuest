#include "ResultUI.h"

#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"

#include "PlayerInfoVariable.h"
#include "ClientEvent.h"
#include "GameManager.h"

fq::client::ResultUI::ResultUI()
	:mScreenManager(nullptr),
	mMonsterBars(),
	mKnightBars(),
	mMagicBars(),
	mArcherBars(),
	mWarriorBars(),
	mIsPlayers(),
	mMonsterRatios(),
	mKnightRatios(),
	mMagicRatios(),
	mWarriorRatios(),
	mArcherRatios(),
	mBarWidth(0),
	mTotalTexts(),
	mNextScene("")
{
}

fq::client::ResultUI::~ResultUI()
{
}

void fq::client::ResultUI::OnStart()
{
	mScreenManager = GetScene()->GetScreenManager();
	mBarWidth = GetGameObject()->GetChildren()[1]->GetChildren()[1]->GetComponent<game_module::ImageUI>()->GetUIInfomation(0).Width;

	for (int i = 0; i < 4; i++)
	{
		mIsPlayers.push_back(false);
	}

	if (PlayerInfoVariable::Player1SoulType != -1)
	{
		mIsPlayers[0] = true;
	}
	if (PlayerInfoVariable::Player2SoulType != -1)
	{
		mIsPlayers[1] = true;
	}
	if (PlayerInfoVariable::Player3SoulType != -1)
	{
		mIsPlayers[2] = true;
	}
	if (PlayerInfoVariable::Player4SoulType != -1)
	{
		mIsPlayers[3] = true;
	}
	initBar();
	OnOffBar();
	setTotal();
	setBar();
}

void fq::client::ResultUI::OnUpdate(float dt)
{
	setScaleScreen();

	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		if (input->IsPadKeyState(i, EPadKey::A, EKeyState::Tap))
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Select", false , fq::sound::EChannel::SE });

			GetScene()->GetObjectByName("GameManager")->GetComponent<GameManager>()->SavePlayerState();
			GetScene()->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ mNextScene, true });
			//GetScene()->DestroyGameObject(GetGameObject());
		}
	}
}

std::shared_ptr<fq::game_module::Component> fq::client::ResultUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto resultUI = std::dynamic_pointer_cast<ResultUI>(clone);

	if (resultUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		resultUI = game_module::ObjectPool::GetInstance()->Assign<ResultUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*resultUI = *this;
	}

	return resultUI;
}

void fq::client::ResultUI::setScaleScreen()
{
	game_module::Transform* myTransform = GetComponent<game_module::Transform>();

	// Scale 자동 조정 
	UINT screenWidth = mScreenManager->GetFixScreenWidth();
	UINT screenHeight = mScreenManager->GetFixScreenHeight();
	float scaleX = screenWidth / (float)1920;
	float scaleY = screenHeight / (float)1080;
	{
		myTransform->SetLocalScale({ scaleX, scaleY , 1 });
	}
}

void fq::client::ResultUI::initBar()
{
	auto lines = GetGameObject()->GetChildren();
	auto bars = lines[1]->GetChildren()[1]->GetChildren();
	mTotalTexts.push_back(lines[1]->GetChildren()[2]->GetComponent<game_module::TextUI>());
	for (int i = 0; i < 4; i++)
	{
		auto bar = bars[i]->GetComponent<game_module::ImageUI>();
		mMonsterBars.push_back(bar);
		bar->SetIsRender(0, false);
	}

	bars = lines[2]->GetChildren()[1]->GetChildren();
	mTotalTexts.push_back(lines[2]->GetChildren()[2]->GetComponent<game_module::TextUI>());
	for (int i = 0; i < 4; i++)
	{
		auto bar = bars[i]->GetComponent<game_module::ImageUI>();
		mKnightBars.push_back(bar);
		bar->SetIsRender(0, false);
	}

	bars = lines[3]->GetChildren()[1]->GetChildren();
	mTotalTexts.push_back(lines[3]->GetChildren()[2]->GetComponent<game_module::TextUI>());
	for (int i = 0; i < 4; i++)
	{
		auto bar = bars[i]->GetComponent<game_module::ImageUI>();
		mMagicBars.push_back(bar);
		bar->SetIsRender(0, false);
	}

	bars = lines[4]->GetChildren()[1]->GetChildren();
	mTotalTexts.push_back(lines[4]->GetChildren()[2]->GetComponent<game_module::TextUI>());
	for (int i = 0; i < 4; i++)
	{
		auto bar = bars[i]->GetComponent<game_module::ImageUI>();
		mArcherBars.push_back(bar);
		bar->SetIsRender(0, false);
	}

	bars = lines[5]->GetChildren()[1]->GetChildren();
	mTotalTexts.push_back(lines[5]->GetChildren()[2]->GetComponent<game_module::TextUI>());
	for (int i = 0; i < 4; i++)
	{
		auto bar = bars[i]->GetComponent<game_module::ImageUI>();
		mWarriorBars.push_back(bar);
		bar->SetIsRender(0, false);
	}
}

void fq::client::ResultUI::setTotal()
{
	mKillMonsterCount.push_back(0);
	mKillMonsterCount.push_back(0);
	mKillMonsterCount.push_back(0);
	mKillMonsterCount.push_back(0);

	mUseKnightCount.push_back(0);
	mUseKnightCount.push_back(0);
	mUseKnightCount.push_back(0);
	mUseKnightCount.push_back(0);

	mUseMagicCount.push_back(0);
	mUseMagicCount.push_back(0);
	mUseMagicCount.push_back(0);
	mUseMagicCount.push_back(0);

	mUseArcherCount.push_back(0);
	mUseArcherCount.push_back(0);
	mUseArcherCount.push_back(0);
	mUseArcherCount.push_back(0);

	mUseWarriorCount.push_back(0);
	mUseWarriorCount.push_back(0);
	mUseWarriorCount.push_back(0);
	mUseWarriorCount.push_back(0);

	int playerSoulType = PlayerInfoVariable::Player1SoulType;
	if (playerSoulType >= 0)
	{
		mKillMonsterCount[playerSoulType] = PlayerInfoVariable::Player1Monster;
		mUseKnightCount[playerSoulType] = PlayerInfoVariable::Player1Knight;
		mUseMagicCount[playerSoulType] = PlayerInfoVariable::Player1Magic;
		mUseArcherCount[playerSoulType] = PlayerInfoVariable::Player1Archer;
		mUseWarriorCount[playerSoulType] = PlayerInfoVariable::Player1Warrior;
	}

	playerSoulType = PlayerInfoVariable::Player2SoulType;
	if (playerSoulType >= 0)
	{
		mKillMonsterCount[playerSoulType] = PlayerInfoVariable::Player2Monster;
		mUseKnightCount[playerSoulType] = PlayerInfoVariable::Player2Knight;
		mUseMagicCount[playerSoulType] = PlayerInfoVariable::Player2Magic;
		mUseArcherCount[playerSoulType] = PlayerInfoVariable::Player2Archer;
		mUseWarriorCount[playerSoulType] = PlayerInfoVariable::Player2Warrior;
	}

	playerSoulType = PlayerInfoVariable::Player3SoulType;
	if (playerSoulType >= 0)
	{
		mKillMonsterCount[playerSoulType] = PlayerInfoVariable::Player3Monster;
		mUseKnightCount[playerSoulType] = PlayerInfoVariable::Player3Knight;
		mUseMagicCount[playerSoulType] = PlayerInfoVariable::Player3Magic;
		mUseArcherCount[playerSoulType] = PlayerInfoVariable::Player3Archer;
		mUseWarriorCount[playerSoulType] = PlayerInfoVariable::Player3Warrior;
	}

	playerSoulType = PlayerInfoVariable::Player4SoulType;
	if (playerSoulType >= 0)
	{
		mKillMonsterCount[playerSoulType] = PlayerInfoVariable::Player4Monster;
		mUseKnightCount[playerSoulType] = PlayerInfoVariable::Player4Knight;
		mUseMagicCount[playerSoulType] = PlayerInfoVariable::Player4Magic;
		mUseArcherCount[playerSoulType] = PlayerInfoVariable::Player4Archer;
		mUseWarriorCount[playerSoulType] = PlayerInfoVariable::Player4Warrior;
	}

	for (int i = 0; i < 4; i++)
	{
		mTotalKillMonster += mKillMonsterCount[i];
		mTotalUseKnight += mUseKnightCount[i];
		mTotalUseMagic += mUseMagicCount[i];
		mTotalUseWarrior += mUseWarriorCount[i];
		mTotalUseArcher += mUseArcherCount[i];
	}

	auto textInfo = mTotalTexts[0]->GetTextInfo();
	textInfo.Text = std::to_string(mTotalKillMonster);
	mTotalTexts[0]->SetTextInfo(textInfo);

	textInfo = mTotalTexts[1]->GetTextInfo();
	textInfo.Text = std::to_string(mTotalUseKnight);
	mTotalTexts[1]->SetTextInfo(textInfo);

	textInfo = mTotalTexts[2]->GetTextInfo();
	textInfo.Text = std::to_string(mTotalUseMagic);
	mTotalTexts[2]->SetTextInfo(textInfo);

	textInfo = mTotalTexts[3]->GetTextInfo();
	textInfo.Text = std::to_string(mTotalUseArcher);
	mTotalTexts[3]->SetTextInfo(textInfo);

	textInfo = mTotalTexts[4]->GetTextInfo();
	textInfo.Text = std::to_string(mTotalUseWarrior);
	mTotalTexts[4]->SetTextInfo(textInfo);

	if (mTotalKillMonster == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			mMonsterBars[i]->SetIsRender(0, false);
			mMonsterBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(false);
		}
	}
	if (mTotalUseKnight == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			mKnightBars[i]->SetIsRender(0, false);
			mKnightBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(false);
		}
	}
	if (mTotalUseMagic == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			mMagicBars[i]->SetIsRender(0, false);
			mMagicBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(false);
		}
	}
	if (mTotalUseArcher == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			mArcherBars[i]->SetIsRender(0, false);
			mArcherBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(false);
		}
	}
	if (mTotalUseWarrior == 0)
	{
		for (int i = 0; i < 4; i++)
		{
			mWarriorBars[i]->SetIsRender(0, false);
			mWarriorBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(false);
		}
	}
}

void fq::client::ResultUI::setBar()
{
	for (int i = 0; i < 4; i++)
	{
		mMonsterRatios.push_back(mKillMonsterCount[i] / (float)mTotalKillMonster);
		auto text = mMonsterBars[i]->GetTransform()->GetChildren()[0]->GetComponent<game_module::TextUI>();
		auto textInfo = text->GetTextInfo();

		// 숫자 0인경우에는 Text를 표시하지 않습니다
		textInfo.Text = mKillMonsterCount[i] == 0 ? "" : std::to_string(mKillMonsterCount[i]);
		text->SetTextInfo(textInfo);

		mKnightRatios.push_back(mUseKnightCount[i] / (float)mTotalUseKnight);
		text = mKnightBars[i]->GetTransform()->GetChildren()[0]->GetComponent<game_module::TextUI>();
		textInfo = text->GetTextInfo();
		textInfo.Text = mUseKnightCount[i] == 0 ? "" : std::to_string(mUseKnightCount[i]);
		text->SetTextInfo(textInfo);

		mMagicRatios.push_back(mUseMagicCount[i] / (float)mTotalUseMagic);
		text = mMagicBars[i]->GetTransform()->GetChildren()[0]->GetComponent<game_module::TextUI>();
		textInfo = text->GetTextInfo();
		textInfo.Text = mUseMagicCount[i] == 0 ? "" : std::to_string(mUseMagicCount[i]);
		text->SetTextInfo(textInfo);

		mWarriorRatios.push_back(mUseWarriorCount[i] / (float)mTotalUseWarrior);
		text = mWarriorBars[i]->GetTransform()->GetChildren()[0]->GetComponent<game_module::TextUI>();
		textInfo = text->GetTextInfo();
		textInfo.Text = mUseWarriorCount[i] == 0 ? "" : std::to_string(mUseWarriorCount[i]);
		text->SetTextInfo(textInfo);

		mArcherRatios.push_back(mUseArcherCount[i] / (float)mTotalUseArcher);
		text = mArcherBars[i]->GetTransform()->GetChildren()[0]->GetComponent<game_module::TextUI>();
		textInfo = text->GetTextInfo();
		textInfo.Text = mUseArcherCount[i] == 0 ? "" : std::to_string(mUseArcherCount[i]);
		text->SetTextInfo(textInfo);
	}

	float curBarPos = -550;
	if (mTotalKillMonster != 0)
	{
		for (int i = 0; i < mMonsterRatios.size(); i++)
		{
			auto uiInfo = mMonsterBars[i]->GetUIInfomation(0);
			uiInfo.Width = mBarWidth * mMonsterRatios[i];
			mMonsterBars[i]->SetUIInfomation(0, uiInfo);
			// 위치도 다시 설정 해줘야 함

			float xPos = 0;
			if (i == 0)
			{
				xPos = ((mBarWidth / 2) - (uiInfo.Width / 2)) * -1;
				curBarPos += uiInfo.Width;
			}
			else
			{
				xPos = curBarPos + (uiInfo.Width / 2);
				curBarPos += uiInfo.Width;
			}
			mMonsterBars[i]->GetTransform()->SetLocalPosition({ xPos, 0, 0 });
		}
	}

	curBarPos = -550;
	if (mTotalUseKnight != 0)
	{
		for (int i = 0; i < mKnightRatios.size(); i++)
		{
			auto uiInfo = mKnightBars[i]->GetUIInfomation(0);
			uiInfo.Width = mBarWidth * mKnightRatios[i];
			mKnightBars[i]->SetUIInfomation(0, uiInfo);
			// 위치도 다시 설정 해줘야 함

			float xPos = 0;
			if (i == 0)
			{
				xPos = ((mBarWidth / 2) - (uiInfo.Width / 2)) * -1;
				curBarPos += uiInfo.Width;
			}
			else
			{
				xPos = curBarPos + (uiInfo.Width / 2);
				curBarPos += uiInfo.Width;
			}
			mKnightBars[i]->GetTransform()->SetLocalPosition({ xPos, 0, 0 });
		}
	}

	curBarPos = -550;
	if (mTotalUseMagic != 0)
	{
		for (int i = 0; i < mMagicRatios.size(); i++)
		{
			auto uiInfo = mMagicBars[i]->GetUIInfomation(0);
			uiInfo.Width = mBarWidth * mMagicRatios[i];
			mMagicBars[i]->SetUIInfomation(0, uiInfo);
			// 위치도 다시 설정 해줘야 함

			float xPos = 0;
			if (i == 0)
			{
				xPos = ((mBarWidth / 2) - (uiInfo.Width / 2)) * -1;
				curBarPos += uiInfo.Width;
			}
			else
			{
				xPos = curBarPos + (uiInfo.Width / 2);
				curBarPos += uiInfo.Width;
			}
			mMagicBars[i]->GetTransform()->SetLocalPosition({ xPos, 0, 0 });
		}
	}

	curBarPos = -550;
	if (mTotalUseWarrior != 0)
	{
		for (int i = 0; i < mWarriorRatios.size(); i++)
		{
			auto uiInfo = mWarriorBars[i]->GetUIInfomation(0);
			uiInfo.Width = mBarWidth * mWarriorRatios[i];
			mWarriorBars[i]->SetUIInfomation(0, uiInfo);
			// 위치도 다시 설정 해줘야 함

			float xPos = 0;
			if (i == 0)
			{
				xPos = ((mBarWidth / 2) - (uiInfo.Width / 2)) * -1;
				curBarPos += uiInfo.Width;
			}
			else
			{
				xPos = curBarPos + (uiInfo.Width / 2);
				curBarPos += uiInfo.Width;
			}
			mWarriorBars[i]->GetTransform()->SetLocalPosition({ xPos, 0, 0 });
		}
	}

	curBarPos = -550;
	if (mTotalUseArcher != 0)
	{
		for (int i = 0; i < mArcherRatios.size(); i++)
		{
			auto uiInfo = mArcherBars[i]->GetUIInfomation(0);
			uiInfo.Width = mBarWidth * mArcherRatios[i];
			mArcherBars[i]->SetUIInfomation(0, uiInfo);
			// 위치도 다시 설정 해줘야 함

			float xPos = 0;
			if (i == 0)
			{
				xPos = ((mBarWidth / 2) - (uiInfo.Width / 2)) * -1;
				curBarPos += uiInfo.Width;
			}
			else
			{
				xPos = curBarPos + (uiInfo.Width / 2);
				curBarPos += uiInfo.Width;
			}
			mArcherBars[i]->GetTransform()->SetLocalPosition({ xPos, 0, 0 });
		}
	}
}

void fq::client::ResultUI::SetNextScene(std::string nextScene)
{
	mNextScene = nextScene;
}

void fq::client::ResultUI::OnOffBar()
{
	std::vector<bool> isPlayers(4, false);
	if (PlayerInfoVariable::Player1SoulType != -1)
	{
		isPlayers[PlayerInfoVariable::Player1SoulType] = true;
	}
	if (PlayerInfoVariable::Player2SoulType != -1)
	{
		isPlayers[PlayerInfoVariable::Player2SoulType] = true;
	}
	if (PlayerInfoVariable::Player3SoulType != -1)
	{
		isPlayers[PlayerInfoVariable::Player3SoulType] = true;
	}
	if (PlayerInfoVariable::Player4SoulType != -1)
	{
		isPlayers[PlayerInfoVariable::Player4SoulType] = true;
	}

	for (int i = 0; i < isPlayers.size(); i++)
	{
		bool isPlayer = isPlayers[i];
		mMonsterBars[i]->SetIsRender(0, isPlayer);
		mMonsterBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(isPlayer);

		mKnightBars[i]->SetIsRender(0, isPlayer);
		mKnightBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(isPlayer);

		mMagicBars[i]->SetIsRender(0, isPlayer);
		mMagicBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(isPlayer);

		mArcherBars[i]->SetIsRender(0, isPlayer);
		mArcherBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(isPlayer);

		mWarriorBars[i]->SetIsRender(0, isPlayer);
		mWarriorBars[i]->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(isPlayer);
	}
}
