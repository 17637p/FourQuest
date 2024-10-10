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
#include "SettingVariable.h"

#include <boost/locale.hpp>

std::shared_ptr<fq::game_module::Component> fq::client::SoulSelectUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto soulSelectUI = std::dynamic_pointer_cast<SoulSelectUI>(clone);

	if (soulSelectUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		soulSelectUI = game_module::ObjectPool::GetInstance()->Assign<SoulSelectUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*soulSelectUI = *this;
	}

	return soulSelectUI;
}

void fq::client::SoulSelectUI::OnStart()
{
	// screenManager ���
	fq::game_module::Scene* scene = GetScene();
	mScreenManager = GetScene()->GetScreenManager();

	// ��ȥ �̸� & ���� ���� & ���� TextUI ���
	auto mPlayers = GetGameObject()->GetChildren();
	for (int i = 0; i < 4; i++)
	{
		auto curPlayerSoulBox = mPlayers[i]->GetChildren()[2];
		mSoulNameTexts.push_back(curPlayerSoulBox->GetChildren()[0]->GetComponent<game_module::TextUI>());
		mWeaponNameTexts.push_back(curPlayerSoulBox->GetChildren()[1]->GetComponent<game_module::TextUI>());
		mContentTexts.push_back(curPlayerSoulBox->GetChildren()[3]->GetComponent<game_module::TextUI>());
	}

	mSoulNames.push_back(wstringToString(L"��� ��ȥ"));
	mSoulNames.push_back(wstringToString(L"�Ķ� ��ȥ"));
	mSoulNames.push_back(wstringToString(L"���� ��ȥ"));
	mSoulNames.push_back(wstringToString(L"�ʷ� ��ȥ"));

	mWeapons.push_back(wstringToString(L"���� ���� : ��"));
	mWeapons.push_back(wstringToString(L"���� ���� : �ϵ�"));
	mWeapons.push_back(wstringToString(L"���� ���� : ���ظ�"));
	mWeapons.push_back(wstringToString(L"���� ���� : Ȱ"));

	mContents.push_back(wstringToString(L"��ȥ�� ���� ��ȯ�Ͽ� �ֵθ��ϴ�.\n�߻��� �˱�� ������ ���� ���ϰ� ��ġ�� ���ظ� �����ϴ�."));
	mContents.push_back(wstringToString(L"��ȥ�� �ϵ带 ��ȯ�Ͽ� ������ �����մϴ�.\n�ڽ��� �ֺ��� �ٶ� �帷�� �����Ͽ�, �ٰ����� ���� ���ĳ��ϴ�."));
	mContents.push_back(wstringToString(L"��ȥ�� �ظӸ� ��ȯ�Ͽ� �ֵθ��ϴ�.\n�ҿ뵹��ġ�� �ظ��� ���������� �� ���̸� �����մϴ�."));
	mContents.push_back(wstringToString(L"��ȥ�� Ȱ�� ��ȯ�Ͽ� ȭ���� �����մϴ�.\n��û�� �ӵ��� ������ ����� ���մϴ�."));

	// Init Ready 
	for (int i = 0; i < 4; i++)
	{
		// 1�� �÷��̾�� ���ۺ��� ����â
		if (i == 0)
		{
			mIsSelects.push_back(true);
		}
		else
		{
			mIsSelects.push_back(false);
		}
		
		// Ready ����
		mIsReadys.push_back(false);
		mSelectSouls.push_back(0);
	}

	for (int i = 0; i < 4; i++)
	{
		mSoulNameTexts[i]->SetText(mSoulNames[mSelectSouls[i]]);
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

	// SelectPoints ����
	for (int i = 0; i < 4; i++)
	{
		auto selectPoints = GetGameObject()->GetChildren()[i]->GetChildren()[5]->GetChildren();
		for (int j = 0; j < 5; j++)
		{
			mSelectPoints.push_back(selectPoints[j]->GetComponent<game_module::Transform>());
		}
	}

	// �⺻ ����
	setSpawnButton(0, false);
	setSoulBox(0, true);
	setReadyUI(0, false);
	for (int i = 1; i < 4; i++)
	{
		setSpawnButton(i, true);
		setSoulBox(i, false);
		setReadyUI(i, false);
	}

	// SelectLevel ó��
	mButtons.clear();
	mExplanationTexts.clear();

	mLevelSelectBackground = GetGameObject()->GetChildren()[6];

	auto levelButtons = mLevelSelectBackground->GetChildren()[1]->GetChildren();
	for (int i = 0; i < 3; i++)
	{
		mButtons.push_back(levelButtons[i]);
	}

	mExplanationTextUI = mLevelSelectBackground->GetChildren()[2]->GetChildren()[0]->GetComponent<game_module::TextUI>();
	mExplanationTexts.push_back(wstringToString(L"�׼ǰ��ӿ� �ͼ����� ���� �÷��̾�� ������ ���̵��Դϴ�."));
	mExplanationTexts.push_back(wstringToString(L"4Quest�� ó�� ���� �÷��̾�� ��õ�ϴ� ���̵��Դϴ�."));
	mExplanationTexts.push_back(wstringToString(L"�����ϰ� �÷����ؾ� �ϴ� �������� ���̵��Դϴ�."));
	setSelectLevelPopup(false);

	mSelectBackground = mLevelSelectBackground->GetChildren()[1]->GetChildren()[3];
}

void fq::client::SoulSelectUI::OnUpdate(float dt)
{
	setScaleScreen();
	setSelectBoxPosition(dt);

	// �Է� ó��
	processInput();
	CheckAllReady(dt);

	MoveSoulDown(dt);

	mCurStickDelay += dt;
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
	mLevelSelectBackground(nullptr),
	mButtons(),
	mExplanationTexts(),
	mExplanationTextUI(nullptr),
	mIsSelectedLevel(false),
	mIsOnSelectLevel(false),
	mStickDelay(0.2f),
	mCurStickDelay(0),
	mUIAnimSpeed(1000),
	mSelectButtonID(0),
	mNextSceneName("Scene1")
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

	// Scale �ڵ� ���� 
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
		if (mIsOnSelectLevel)
		{
			// Stick Y ��
			float isLeftStickY = input->GetStickInfomation(i, EPadStick::leftY);

			// �Ʒ���
			bool isDpadDownTap = input->GetPadKeyState(i, EPadKey::DpadDown) == EKeyState::Tap;
			// Stick ó��
			bool isLeftStickDownTap = false;
			if (isLeftStickY < -0.9f)
			{
				if (mCurStickDelay > mStickDelay)
				{
					mCurStickDelay = 0;
					isLeftStickDownTap = true;
				}
			}

			if (isDpadDownTap || isLeftStickDownTap)
			{
				if (mSelectButtonID < 2)
				{
					mSelectButtonID++;
					mExplanationTextUI->SetText(mExplanationTexts[mSelectButtonID]);
					GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Moving", false , fq::sound::EChannel::SE });
				}
			}

			// ����
			bool isDpadUpTap = input->GetPadKeyState(i, EPadKey::DpadUp) == EKeyState::Tap;
			// Stick ó��
			bool isLeftStickUpTap = false;
			if (isLeftStickY > 0.9f)
			{
				if (mCurStickDelay > mStickDelay)
				{
					mCurStickDelay = 0;
					isLeftStickUpTap = true;
				}
			}

			if (isDpadUpTap || isLeftStickUpTap)
			{
				if (mSelectButtonID > 0)
				{
					mSelectButtonID--;
					mExplanationTextUI->SetText(mExplanationTexts[mSelectButtonID]);
					GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Moving", false , fq::sound::EChannel::SE });
				}
			}
		}

		// Button A
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Tap
			|| input->GetKeyState(EKey::A) == EKeyState::Tap)
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Select", false , fq::sound::EChannel::SE });

			if (mIsOnSelectLevel)
			{
				SettingVariable::SelectLevel = mSelectButtonID;
				setSelectLevelPopup(false);
				mIsSelectedLevel = true;

				continue;
			}
			if (!mIsReadys[i])
			{
				// ��ȥ ����â�̶��
				if (mIsSelects[i])
				{
					bool isSeletedOther = false;
					for (int j = 0; j < 4; j++)
					{
						if (j == i)
						{
							continue;
						}
						// �ٸ� �÷��̾�� ���� �� �����ϸ� �ƹ��ϵ� �߻����� ����
						if (mSelectSouls[i] == mSelectSouls[j] && mIsReadys[j])
						{
							isSeletedOther = true;
							break;
						}
					}
					if (isSeletedOther)
					{
						break;
					}

					// ����
					mIsReadys[i] = true;
					mIsSelects[i] = false;
					setReadyUI(i, true);
					setSoulBox(i, false);

					// Soul �߰�
					std::shared_ptr<game_module::GameObject> newSoul;
					auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSoulPrefab);
					newSoul = *(instance.begin());

					float xPos = -3.4f + (2.26f * i);
					newSoul->GetComponent<game_module::Transform>()->SetLocalPosition({ xPos, 0.5f, 0.5f });
					newSoul->GetComponent<game_module::CharacterController>()->SetControllerID(i);
					newSoul->GetComponent<game_module::CharacterController>()->SetOnRotation(false);
					mSouls.push_back(newSoul);

					// MagicSymbol �߰�
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
				// �ƴϸ� ��ȥ ����
				else
				{
					if (!mIsOnSelectLevel && !mIsSelectedLevel)
					{
						mIsSelects[i] = true;
						setSpawnButton(i, false);
						setSoulBox(i, true);
					}
				}
			}
		}

		if (input->GetPadKeyState(i, EPadKey::B) == EKeyState::Tap
			|| input->GetKeyState(EKey::B) == EKeyState::Tap)
		{
			GetScene()->GetEventManager()->FireEvent<fq::event::OnPlaySound>({ "UI_Cancle", false , fq::sound::EChannel::SE });

			if (mIsSelectedLevel)
			{
				mIsSelectedLevel = false;
				continue;
			}
			if (mIsOnSelectLevel)
			{
				setSelectLevelPopup(false);
				//mIsReadys[i] = false;
				//setReadyUI(i, false);

				//continue;
			}
			// ��ȥ ����â�̶��
			if (mIsSelects[i])
			{
				// 0 �� �÷��̾�� ����â ����
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

				// soul ����
				int selectSoulIndex = 0;
				auto soul = std::find_if(mSouls.begin(), mSouls.end(),
					[this, i, &selectSoulIndex](std::shared_ptr<game_module::GameObject> soul)
					{
						selectSoulIndex++;
						return soul->GetComponent<game_module::CharacterController>()->GetControllerID() == i;
					});
				GetScene()->DestroyGameObject(soul->get());

				mSouls.erase(mSouls.begin() + (selectSoulIndex - 1));

				// magicSymbol ����
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

			// ��ȥ ����â�̶��
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

			// ��ȥ ����â�̶��
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

	SettingVariable::SelectLevel = mSelectButtonID;
}

void fq::client::SoulSelectUI::CheckAllReady(float dt)
{
	auto countUI = GetGameObject()->GetChildren()[5];

	game_module::ImageUI* countBackground = countUI->GetComponent<game_module::ImageUI>();
	game_module::TextUI* questStartText = countUI->GetChildren()[0]->GetComponent<game_module::TextUI>();
	game_module::TextUI* countText = countUI->GetChildren()[1]->GetComponent<game_module::TextUI>();
	game_module::TextUI* levelText = countUI->GetChildren()[2]->GetComponent<game_module::TextUI>();

	bool isAllReady = true;
	for (int i = 0; i < 4; i++)
	{
		if (mIsSelects[i] == true)
		{
			isAllReady = false;
			mCurTime = 0;

			countBackground->SetIsRender(0, false);
			countText->SetIsRender(false);
			levelText->SetIsRender(false);
			questStartText->SetIsRender(false);

			return;
		}
	}

	// ��� �غ���
	if (isAllReady && !mIsSelectedLevel)
	{
		setSelectLevelPopup(true);

		isAllReady = false;
		mCurTime = 0;

		countBackground->SetIsRender(0, false);
		countText->SetIsRender(false);
		levelText->SetIsRender(false);
		questStartText->SetIsRender(false);
	}

	if (mIsSelectedLevel)
	{
		SaveSoulType();
		countBackground->SetIsRender(0, true);
		countText->SetIsRender(true);
		levelText->SetIsRender(true);
		questStartText->SetIsRender(true);

		if (mSelectButtonID == 0)
		{
			levelText->SetText(wstringToString(L"����"));
		}
		else if (mSelectButtonID == 1)
		{
			levelText->SetText(wstringToString(L"����"));
		}
		else if (mSelectButtonID == 2)
		{
			levelText->SetText(wstringToString(L"�����"));
		}

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

void fq::client::SoulSelectUI::setSelectLevelPopup(bool isOn)
{
	mIsOnSelectLevel = isOn;

	mLevelSelectBackground->GetComponent<game_module::ImageUI>()->SetIsRender(0, isOn);
	mLevelSelectBackground->GetChildren()[0]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isOn);
	auto levelBackground = mLevelSelectBackground->GetChildren()[1];
	levelBackground->GetComponent<game_module::ImageUI>()->SetIsRender(0, isOn);

	auto levelBackgroundChildren = levelBackground->GetChildren();
	levelBackgroundChildren[0]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isOn);
	levelBackgroundChildren[1]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isOn);
	levelBackgroundChildren[2]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isOn);
	levelBackgroundChildren[3]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isOn);
	levelBackgroundChildren[3]->GetChildren()[0]->GetComponent<game_module::SpriteAnimationUI>()->SetIsRender(isOn);

	mLevelSelectBackground->GetChildren()[2]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isOn);
	mLevelSelectBackground->GetChildren()[2]->GetChildren()[0]->GetComponent<game_module::TextUI>()->SetIsRender(isOn);
}

void fq::client::SoulSelectUI::setSelectBoxPosition(float dt)
{
	// ����UI ��ġ�� SelectBox �ű�� 
	game_module::Transform* selectTransform = mButtons[mSelectButtonID]->GetComponent<game_module::Transform>();
	DirectX::SimpleMath::Vector3 selectPosition = selectTransform->GetLocalPosition();

	DirectX::SimpleMath::Vector3 curPosition = mSelectBackground->GetComponent<game_module::Transform>()->GetLocalPosition();

	float dist = selectPosition.y - curPosition.y;
	if (std::abs(dist) < mUIAnimSpeed * dt)
	{
		curPosition = selectPosition;
	}
	else if (dist > 0)
	{
		curPosition.y += mUIAnimSpeed * dt;
	}
	else
	{
		curPosition.y -= mUIAnimSpeed * dt;
	}

	mSelectBackground->GetComponent<game_module::Transform>()->SetLocalPosition(curPosition);
}
