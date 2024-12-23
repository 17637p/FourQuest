#include "SoulSelectUI.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/PrefabManager.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/TextUI.h"
#include "../FQGameModule/SpriteAnimationUI.h"

#include "Soul.h"
#include "PlayerInfoVariable.h"
#include "PlayerSoulVariable.h"

#include <boost/locale.hpp>

std::shared_ptr<fq::game_module::Component> fq::client::SoulSelectUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto soulSelectUI = std::dynamic_pointer_cast<SoulSelectUI>(clone);

	if (soulSelectUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		soulSelectUI = game_module::ObjectPool::GetInstance()->Assign<SoulSelectUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*soulSelectUI = *this;
	}

	return soulSelectUI;
}

void fq::client::SoulSelectUI::OnStart()
{
	// screenManager 등록
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();

	// 영혼 이름 & 숙련 무기 & 설명 TextUI 등록
	auto mPlayers = GetGameObject()->GetChildren();
	for (int i = 0; i < 4; i++)
	{
		auto curPlayerSoulBox = mPlayers[i]->GetChildren()[2];
		mSoulNameTexts.push_back(curPlayerSoulBox->GetChildren()[0]->GetComponent<game_module::TextUI>());
		mWeaponNameTexts.push_back(curPlayerSoulBox->GetChildren()[1]->GetComponent<game_module::TextUI>());
		mContentTexts.push_back(curPlayerSoulBox->GetChildren()[3]->GetComponent<game_module::TextUI>());
	}

	mSoulNames.clear();
	mSoulNameColors.clear();
	mWeapons.clear();
	mContents.clear();

	mSoulNames.push_back(wstringToString(L"루카스"));
	mSoulNames.push_back(wstringToString(L"아르카나"));
	mSoulNames.push_back(wstringToString(L"발더"));
	mSoulNames.push_back(wstringToString(L"실버"));

	mSoulNameColors.push_back(PlayerSoulVariable::SwordSoulColor);
	mSoulNameColors.push_back(PlayerSoulVariable::StaffSoulColor);
	mSoulNameColors.push_back(PlayerSoulVariable::AxeSoulColor);
	mSoulNameColors.push_back(PlayerSoulVariable::BowSoulColor);

	mWeapons.push_back(wstringToString(L"숙련 무기 : 검"));
	mWeapons.push_back(wstringToString(L"숙련 무기 : 완드"));
	mWeapons.push_back(wstringToString(L"숙련 무기 : 워해머"));
	mWeapons.push_back(wstringToString(L"숙련 무기 : 활"));

	mContents.push_back(wstringToString(L"영혼의 검을 소환하여 휘두릅니다.\n발산한 검기는 전방의 적을 강하게 밀치며 피해를 입힙니다."));
	mContents.push_back(wstringToString(L"영혼의 완드를 소환하여 마법을 시전합니다.\n자신의 주변에 바람 장막을 형성하여, 다가오는 적을 밀쳐냅니다."));
	mContents.push_back(wstringToString(L"영혼의 해머를 소환하여 휘두릅니다.\n소용돌이치는 해머의 움직임으로 적 사이를 돌파합니다."));
	mContents.push_back(wstringToString(L"영혼의 활을 소환하여 화살을 난사합니다.\n엄청난 속도의 무차별 사격을 가합니다."));

	// Init Ready 
	for (int i = 0; i < 4; i++)
	{
		// 1번 플레이어는 시작부터 선택창
		if (i == 0)
		{
			mIsSelects.push_back(true);
		}
		else
		{
			mIsSelects.push_back(false);
		}

		// Ready 설정
		mIsReadys.push_back(false);
		mSelectSouls.push_back(0);
	}

	for (int i = 0; i < 4; i++)
	{
		mSoulNameTexts[i]->SetText(mSoulNames[mSelectSouls[i]]);
		auto textInfo = mSoulNameTexts[i]->GetTextInfo();
		textInfo.FontColor = mSoulNameColors[mSelectSouls[i]];
		mSoulNameTexts[i]->SetTextInfo(textInfo);
		mWeaponNameTexts[i]->SetText(mWeapons[mSelectSouls[i]]);
		mContentTexts[i]->SetText(mContents[mSelectSouls[i]]);
	}

	mCurTime = 0;

	for (int i = 0; i < 4; i++)
	{
		auto soulIcons = GetGameObject()->GetChildren()[i]->GetChildren()[1]->GetChildren();
		for (int j = 0; j < 4; j++)
		{
			mPlayerSoulIcons.push_back(soulIcons[j]->GetComponent<game_module::SpriteAnimationUI>());
		}
	}

	// SelectPoints 설정
	for (int i = 0; i < 4; i++)
	{
		auto selectPoints = GetGameObject()->GetChildren()[i]->GetChildren()[5]->GetChildren();
		for (int j = 0; j < 5; j++)
		{
			mSelectPoints.push_back(selectPoints[j]->GetComponent<game_module::Transform>());
		}
	}

	// 기본 설정
	setSpawnButton(0, false);
	setSoulBox(0, true);
	setReadyUI(0, false);
	for (int i = 1; i < 4; i++)
	{
		setSpawnButton(i, true);
		setSoulBox(i, false);
		setReadyUI(i, false);
	}
}

void fq::client::SoulSelectUI::OnUpdate(float dt)
{
	setScaleScreen();

	// 입력 처리
	processInput();
	CheckAllReady(dt);

	MoveSoulDown(dt);

	mCurStickDelay += dt;

	updateSoulBox();
}

fq::client::SoulSelectUI::SoulSelectUI()
	:mScreenManager(nullptr),
	mSoulNameTexts(),
	mWeaponNameTexts(),
	mContentTexts(),
	mSoulNames(),
	mWeapons(),
	mContents(),
	mIsReadys(),
	mSoulPrefab(),
	mSouls(),
	mSelectSouls(),
	mCurTime(0),
	mChangeSceneTime(3),
	mSoulMoveSpeed(100),
	mSelectButtonID(0),
	mNextSceneName("Scene1"),
	mSoulNameColors()
{
}

fq::client::SoulSelectUI::~SoulSelectUI()
{
}

void fq::client::SoulSelectUI::setSpawnButton(int index, bool isSpawned)
{
	auto children = GetGameObject()->GetChildren()[index]->GetChildren();
	auto spawnButton = children[3];

	// SpawnButton
	spawnButton->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
}

void fq::client::SoulSelectUI::setSoulBox(int index, bool isSpawned)
{
	auto children = GetGameObject()->GetChildren()[index]->GetChildren();
	auto soulBox = children[2];

	// SoulBox
	for (int i = 0; i < 4; i++)
	{
		mPlayerSoulIcons[index * 4 + i]->SetIsRender(false);
	}

	if (isSpawned)
	{
		mPlayerSoulIcons[index * 4 + mSelectSouls[index]]->SetIsRender(true);
	}

	soulBox->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
	auto soulBoxChildren = soulBox->GetChildren();
	for (int i = 0; i < 4; i++)
	{
		auto textUI = soulBoxChildren[i]->GetComponent<game_module::TextUI>();
		auto textInfo = textUI->GetTextInfo();
		textInfo.IsRender = isSpawned;
		textUI->SetTextInfo(textInfo);
	}
	soulBoxChildren[4]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);

	for (int i = 0; i < 5; i++)
	{
		mSelectPoints[index * 5 + i]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
	}
}

void fq::client::SoulSelectUI::setScaleScreen()
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

std::string fq::client::SoulSelectUI::wstringToString(std::wstring wStr)
{
	return boost::locale::conv::from_utf(wStr, "UTF-8");
}

void fq::client::SoulSelectUI::processInput()
{
	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Tap
			|| input->GetKeyState(EKey::A) == EKeyState::Tap)
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Select", false , fq::sound::EChannel::SE });

			if (!mIsReadys[i])
			{
				// 영혼 선택창이라면
				if (mIsSelects[i])
				{
					bool isSeletedOther = false;
					for (int j = 0; j < 4; j++)
					{
						if (j == i)
						{
							continue;
						}
						// 다른 플레이어와 같은 걸 선택하면 아무일도 발생하지 않음
						if (mSelectSouls[i] == mSelectSouls[j] && mIsReadys[j])
						{
							isSeletedOther = true;
							break;
						}
					}
					if (isSeletedOther)
					{
						input->SetVibration(i, EVibrationMode::Left, 30000.f, 0.5f);
						break;
					}

					// 레디
					GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Soul", false , fq::sound::EChannel::SE });

					mIsReadys[i] = true;
					mIsSelects[i] = false;
					setReadyUI(i, true);
					setSoulBox(i, false);

					// Soul 추가
					std::shared_ptr<game_module::GameObject> newSoul;
					auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSoulPrefab);
					newSoul = *(instance.begin());

					float xPos = -3.4f + (2.26f * i);
					newSoul->GetComponent<game_module::Transform>()->SetLocalPosition({ xPos, 0.5f, 0.5f });
					newSoul->GetComponent<game_module::CharacterController>()->SetControllerID(i);
					newSoul->GetComponent<game_module::CharacterController>()->SetOnRotation(false);
					newSoul->GetComponent<Soul>()->SetInvincible();
					mSouls.push_back(newSoul);

					// MagicSymbol 추가
					std::shared_ptr<game_module::GameObject> newMagicSymbol;
					instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mMagicSymbolPrefab);
					newMagicSymbol = *(instance.begin());

					xPos = -3.75f + (2.5f * i);
					newMagicSymbol->GetTransform()->SetLocalPosition({ xPos, 0, -0.8 });
					mMagicSymbols.push_back(newMagicSymbol);

					switch (mSelectSouls[i])
					{
						case 0:
							newSoul->GetComponent<Soul>()->SetSoulType(ESoulType::Sword);
							break;
						case 1:
							newSoul->GetComponent<Soul>()->SetSoulType(ESoulType::Staff);
							break;
						case 2:
							newSoul->GetComponent<Soul>()->SetSoulType(ESoulType::Axe);
							break;
						case 3:
							newSoul->GetComponent<Soul>()->SetSoulType(ESoulType::Bow);
							break;
						default:
							break;
					}

					GetScene()->AddGameObject(newSoul);
					GetScene()->AddGameObject(newMagicSymbol);
				}
				// 아니면 영혼 선택
				else
				{
					mIsSelects[i] = true;
					setSpawnButton(i, false);
					setSoulBox(i, true);
				}
			}
		}

		if (input->GetPadKeyState(i, EPadKey::B) == EKeyState::Tap
			|| input->GetKeyState(EKey::B) == EKeyState::Tap)
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Cancle", false , fq::sound::EChannel::SE });

			// 영혼 선택창이라면
			if (mIsSelects[i])
			{
				// 0 번 플레이어는 선택창 고정
				if (i != 0)
				{
					mIsSelects[i] = false;
					setSpawnButton(i, true);
					setSoulBox(i, false);
				}
			}
			if (mIsReadys[i])
			{
				mIsReadys[i] = false;
				mIsSelects[i] = true;
				setReadyUI(i, false);
				setSoulBox(i, true);

				// soul 삭제
				int selectSoulIndex = 0;
				auto soul = std::find_if(mSouls.begin(), mSouls.end(),
					[this, i, &selectSoulIndex](std::shared_ptr<game_module::GameObject> soul)
					{
						selectSoulIndex++;
						return soul->GetComponent<game_module::CharacterController>()->GetControllerID() == i;
					});
				GetScene()->DestroyGameObject(soul->get());

				mSouls.erase(mSouls.begin() + (selectSoulIndex - 1));

				// magicSymbol 삭제
				GetScene()->DestroyGameObject(mMagicSymbols[selectSoulIndex - 1].get());
				mMagicSymbols.erase(mMagicSymbols.begin() + (selectSoulIndex - 1));
			}
		}

		if (input->GetPadKeyState(i, EPadKey::LeftShoulder) == EKeyState::Tap
			|| input->GetKeyState(EKey::Left) == EKeyState::Tap)
		{
			for (int j = 0; j < 4; j++)
			{
				mPlayerSoulIcons[i * 4 + j]->SetIsRender(false);
			}

			// 영혼 선택창이라면
			if (mIsSelects[i])
			{
				GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Moving", false , fq::sound::EChannel::SE });

				if (mSelectSouls[i] == 0)
				{
					mSelectSouls[i] = 3;
				}
				else
				{
					mSelectSouls[i]--;
				}

				mSoulNameTexts[i]->SetText(mSoulNames[mSelectSouls[i]]);
				auto textInfo = mSoulNameTexts[i]->GetTextInfo();
				textInfo.FontColor = mSoulNameColors[mSelectSouls[i]];
				mSoulNameTexts[i]->SetTextInfo(textInfo);
				mWeaponNameTexts[i]->SetText(mWeapons[mSelectSouls[i]]);
				mContentTexts[i]->SetText(mContents[mSelectSouls[i]]);

				mPlayerSoulIcons[i * 4 + mSelectSouls[i]]->SetIsRender(true);
				SetSelectPoints(i, mSelectSouls[i]);
			}
		}

		if (input->GetPadKeyState(i, EPadKey::RightShoulder) == EKeyState::Tap
			|| input->GetKeyState(EKey::Right) == EKeyState::Tap)
		{
			for (int j = 0; j < 4; j++)
			{
				mPlayerSoulIcons[i * 4 + j]->SetIsRender(false);
			}

			// 영혼 선택창이라면
			if (mIsSelects[i])
			{
				GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Moving", false , fq::sound::EChannel::SE });

				if (mSelectSouls[i] == 3)
				{
					mSelectSouls[i] = 0;
				}
				else
				{
					mSelectSouls[i]++;
				}

				mSoulNameTexts[i]->SetText(mSoulNames[mSelectSouls[i]]);
				auto textInfo = mSoulNameTexts[i]->GetTextInfo();
				textInfo.FontColor = mSoulNameColors[mSelectSouls[i]];
				mSoulNameTexts[i]->SetTextInfo(textInfo);
				mWeaponNameTexts[i]->SetText(mWeapons[mSelectSouls[i]]);
				mContentTexts[i]->SetText(mContents[mSelectSouls[i]]);

				mPlayerSoulIcons[i * 4 + mSelectSouls[i]]->SetIsRender(true);
				SetSelectPoints(i, mSelectSouls[i]);
			}
		}
	}
}

void fq::client::SoulSelectUI::setReadyUI(int index, bool isSpawned)
{
	auto children = GetGameObject()->GetChildren()[index]->GetChildren();
	auto readyUI = children[4]->GetChildren();

	// SoulBox
	readyUI[0]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
	auto cancelButton = readyUI[1]->GetComponent<game_module::ImageUI>();
	cancelButton->SetIsRender(0, isSpawned);
	cancelButton->GetGameObject()->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(isSpawned);
}

void fq::client::SoulSelectUI::SaveSoulType()
{
	if (mIsReadys[0])
	{
		PlayerInfoVariable::Player1SoulType = mSelectSouls[0];
		PlayerInfoVariable::Player1State = 0;
	}
	else
	{
		PlayerInfoVariable::Player1SoulType = -1;
		PlayerInfoVariable::Player1State = -1;
	}

	if (mIsReadys[1])
	{
		PlayerInfoVariable::Player2SoulType = mSelectSouls[1];
		PlayerInfoVariable::Player2State = 0;
	}
	else
	{
		PlayerInfoVariable::Player2SoulType = -1;
		PlayerInfoVariable::Player2State = -1;
	}

	if (mIsReadys[2])
	{
		PlayerInfoVariable::Player3SoulType = mSelectSouls[2];
		PlayerInfoVariable::Player3State = 0;
	}
	else
	{
		PlayerInfoVariable::Player3SoulType = -1;
		PlayerInfoVariable::Player3State = -1;
	}

	if (mIsReadys[3])
	{
		PlayerInfoVariable::Player4SoulType = mSelectSouls[3];
		PlayerInfoVariable::Player4State = 0;
	}
	else
	{
		PlayerInfoVariable::Player4SoulType = -1;
		PlayerInfoVariable::Player4State = -1;
	}
}

void fq::client::SoulSelectUI::CheckAllReady(float dt)
{
	auto countUI = GetGameObject()->GetChildren()[5];

	game_module::ImageUI* countBackground = countUI->GetComponent<game_module::ImageUI>();
	game_module::TextUI* questStartText = countUI->GetChildren()[0]->GetComponent<game_module::TextUI>();
	game_module::TextUI* countText = countUI->GetChildren()[1]->GetComponent<game_module::TextUI>();

	bool isAllReady = true;
	for (int i = 0; i < 4; i++)
	{
		if (mIsSelects[i] == true)
		{
			isAllReady = false;
			mCurTime = 0;

			countBackground->SetIsRender(0, false);
			countText->SetIsRender(false);
			questStartText->SetIsRender(false);

			return;
		}
	}

	// 모두 준비라면
	if (isAllReady)
	{
		SaveSoulType();
		countBackground->SetIsRender(0, true);
		countText->SetIsRender(true);
		questStartText->SetIsRender(true);

		int count = mChangeSceneTime - mCurTime + 1;
		countText->SetText(std::to_string(count));

		mCurTime += dt;
		if (mCurTime > mChangeSceneTime)
		{
			GetScene()->GetEventManager()->FireEvent < fq::event::RequestChangeScene>({ mNextSceneName, true });
		}
	}
}

void fq::client::SoulSelectUI::SetSelectPoints(int playerID, int selectNum)
{
	auto selectPos = mSelectPoints[playerID * 5 + selectNum]->GetLocalPosition();
	mSelectPoints[playerID * 5 + 4]->SetLocalPosition(selectPos);
}

void fq::client::SoulSelectUI::MoveSoulDown(float dt)
{
	for (int i = 0; i < mSouls.size(); i++)
	{
		if (!mSouls[i]->IsDestroyed())
		{
			game_module::Transform* soulT = mSouls[i]->GetTransform();
			DirectX::SimpleMath::Vector3 soulV = soulT->GetLocalPosition();
			float soulZ = soulV.z;
			if (soulZ > -0.5f)
			{
				soulZ -= dt * mSoulMoveSpeed;
			}
			soulT->SetLocalPosition({ soulV.x, soulV.y, soulZ });
		}
	}
}

void fq::client::SoulSelectUI::updateSoulBox()
{
	for (int playerIndex = 0; playerIndex < 4; ++playerIndex)
	{
		// 영혼을 선택하고 있는 경우
		if (!mIsReadys[playerIndex] && mIsSelects[playerIndex])
		{
			auto children = GetGameObject()->GetChildren()[playerIndex]->GetChildren();
			auto soulBox = children[2];
			auto soulBoxChildren = soulBox->GetChildren();

			bool isSeletedOther = false;
			for (int j = 0; j < 4; j++)
			{
				if (j == playerIndex)
				{
					continue;
				}
				// 다른 플레이어와 같은 걸 선택하면 아무일도 발생하지 않음
				if (mSelectSouls[playerIndex] == mSelectSouls[j] && mIsReadys[j])
				{
					isSeletedOther = true;
					break;
				}
			}

			for (int i = 0; i < 4; i++)
			{
				auto textUI = soulBoxChildren[i]->GetComponent<game_module::TextUI>();
				auto textInfo = textUI->GetTextInfo();

				constexpr DirectX::SimpleMath::Color White = { 1.f,1.f,1.f,1.f };
				constexpr DirectX::SimpleMath::Color Gray = { 0.5f,0.5f,0.5f,1.f };

				if (isSeletedOther)
				{
					textInfo.FontColor = Gray;
				}
				else if (i == 0)
				{
					auto colorIndex = mSelectSouls[playerIndex];
					textInfo.FontColor = mSoulNameColors[colorIndex];
				}
				else
				{
					textInfo.FontColor = White;
				}

				textUI->SetTextInfo(textInfo);
			}
		}
	}

}
