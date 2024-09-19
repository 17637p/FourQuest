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

#include "Soul.h"
#include "PlayerInfoVariable.h"

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

	// ��ȥ �̸� & ���� ���� & ���� TextUI ���
	auto mPlayers = GetGameObject()->GetChildren();
	for (int i = 0; i < 4; i++)
	{
		auto curPlayerSoulBox = mPlayers[i]->GetChildren()[2];
		mSoulNameTexts.push_back(curPlayerSoulBox->GetChildren()[0]->GetComponent<game_module::TextUI>());
		mWeaponNameTexts.push_back(curPlayerSoulBox->GetChildren()[1]->GetComponent<game_module::TextUI>());
		mContentTexts.push_back(curPlayerSoulBox->GetChildren()[3]->GetComponent<game_module::TextUI>());
	}

	mSoulNames.push_back(wstringToString(L"�Ͼ� ��ȥ"));
	mSoulNames.push_back(wstringToString(L"���� ��ȥ"));
	mSoulNames.push_back(wstringToString(L"���� ��ȥ"));
	mSoulNames.push_back(wstringToString(L"��� ��ȥ"));

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
}

void fq::client::SoulSelectUI::OnUpdate(float dt)
{
	setScaleScreen();

	// �Է� ó��
	processInput();
	CheckAllReady(dt);
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
	mChangeSceneTime(3)
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
	children[1]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
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
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Tap
			|| input->GetKeyState(EKey::A) == EKeyState::Tap)
		{
			if (!mIsReadys[i])
			{
				// ��ȥ ����â�̶��
				if (mIsSelects[i])
				{
					// ����
					mIsReadys[i] = true;
					mIsSelects[i] = false;
					setReadyUI(i, true);
					setSoulBox(i, false);

					// Soul �߰�
					std::shared_ptr<game_module::GameObject> newSoul;
					auto instance = GetScene()->GetPrefabManager()->InstantiatePrefabResoure(mSoulPrefab);
					newSoul = *(instance.begin());

					float xPos = -3.75f + (2.5f * i);
					newSoul->GetComponent<game_module::Transform>()->SetLocalPosition({ xPos, 0, 0 });
					newSoul->GetComponent<game_module::CharacterController>()->SetControllerID(i);
					mSouls.push_back(newSoul);

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

					SaveSoulType();
					GetScene()->AddGameObject(newSoul);
				}
				// �ƴϸ� ��ȥ ����
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
			}
		}

		if (input->GetPadKeyState(i, EPadKey::LeftShoulder) == EKeyState::Tap
			|| input->GetKeyState(EKey::Left) == EKeyState::Tap)
		{
			// ��ȥ ����â�̶��
			if (mIsSelects[i])
			{
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
			}
		}

		if (input->GetPadKeyState(i, EPadKey::RightShoulder) == EKeyState::Tap
			|| input->GetKeyState(EKey::Right) == EKeyState::Tap)
		{
			// ��ȥ ����â�̶��
			if (mIsSelects[i])
			{
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
	readyUI[1]->GetComponent<game_module::ImageUI>()->SetIsRender(0, isSpawned);
}

void fq::client::SoulSelectUI::SaveSoulType()
{
	PlayerInfoVariable::Player1SoulType = mSelectSouls[0];
	PlayerInfoVariable::Player2SoulType = mSelectSouls[1];
	PlayerInfoVariable::Player3SoulType = mSelectSouls[2];
	PlayerInfoVariable::Player4SoulType = mSelectSouls[3];

	PlayerInfoVariable::Player1State = 0;
	PlayerInfoVariable::Player2State = 0;
	PlayerInfoVariable::Player3State = 0;
	PlayerInfoVariable::Player4State = 0;
}

void fq::client::SoulSelectUI::CheckAllReady(float dt)
{
	bool isAllReady = true;
	for (int i = 0; i < 4; i++)
	{
		if (mIsSelects[i] == true)
		{
			isAllReady = false;
			mCurTime = 0;
			break;
		}
	}

	if (isAllReady)
	{
		mCurTime += dt;
		if (mCurTime > mChangeSceneTime)
		{
			GetScene()->GetEventManager()->FireEvent < fq::event::RequestChangeScene>({ "Scene1", true });
		}
	}
}
