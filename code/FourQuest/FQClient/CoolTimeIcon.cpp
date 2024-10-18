#include "CoolTimeIcon.h"

#include "../FQGameModule/ScreenManager.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/CharacterController.h"

#include "ClientEvent.h"
#include "Player.h"
std::shared_ptr<fq::game_module::Component> fq::client::CoolTimeIcon::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneCoolTimeIcon = std::dynamic_pointer_cast<CoolTimeIcon>(clone);

	if (cloneCoolTimeIcon == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneCoolTimeIcon = game_module::ObjectPool::GetInstance()->Assign<CoolTimeIcon>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneCoolTimeIcon = *this;
	}

	return cloneCoolTimeIcon;
}

fq::client::CoolTimeIcon::CoolTimeIcon()
	:mPlayerID(0),
	mPlayingIcon(0),
	mWorldOffset(),
	mScreenOffset(),
	mCurTime(0),
	mPlaytime(1),
	mSpeed(1),
	mScreenManager(nullptr),
	mMainCamera(nullptr)
{
}

fq::client::CoolTimeIcon::CoolTimeIcon(const CoolTimeIcon& other)
	:mWorldOffset(other.mWorldOffset),
	mScreenOffset(other.mScreenOffset),
	mSpeed(other.mSpeed),
	mPlaytime(other.mPlaytime)
{
}

fq::client::CoolTimeIcon::~CoolTimeIcon()
{
}

fq::client::CoolTimeIcon& fq::client::CoolTimeIcon::operator=(const CoolTimeIcon& other)
{
	mWorldOffset = other.mWorldOffset;
	mScreenOffset = other.mScreenOffset;
	mSpeed = other.mSpeed;
	mPlaytime = other.mPlaytime;

	return *this;
}

void fq::client::CoolTimeIcon::OnStart()
{
	mPlayingIcon = 0;
	mPlayerID = GetGameObject()->GetParent()->GetComponent<game_module::CharacterController>()->GetControllerID();

	mSkillIconImages.clear();
	auto children = GetGameObject()->GetChildren();
	for (const auto& child : children)
	{
		mSkillIconImages.push_back(child->GetComponent<game_module::ImageUI>());
	}

	// MainCamera 가져오기 
	auto view = GetScene()->GetComponentView<game_module::Camera>();
	for (auto& object : view)
	{
		auto camera = object.GetComponent<game_module::Camera>();

		if (camera->IsMain())
		{
			mMainCamera = camera;
		}
	}

	mScreenManager = GetScene()->GetScreenManager();

	eventProcesInitCoolTime();
}

void fq::client::CoolTimeIcon::OnUpdate(float dt)
{
	if (mCurTime < mPlaytime)
	{
		mCurTime += dt;

		DirectX::SimpleMath::Vector3 pos = GetTransform()->GetWorldPosition();
		pos += mWorldOffset;
		pos.y += mSpeed * (mCurTime - (mPlaytime / 2) + 1);

		float width = mScreenManager->GetFixScreenWidth();
		float height = mScreenManager->GetFixScreenHeight();

		auto viewProj = mMainCamera->GetViewProjection();
		DirectX::SimpleMath::Vector3 screenPos = DirectX::SimpleMath::Vector3::Transform(pos, viewProj);

		//GetTransform()->SetLocalPosition({ screenPos.x * width * 0.5f, screenPos.y * height * 0.5f, 0 });
		float posX = width * 0.5f + (screenPos.x * width * 0.5f);
		float posY = height * 0.5f - (screenPos.y * height * 0.5f);

		mSkillIconImages[mPlayingIcon]->GetTransform()->SetWorldPosition({ posX, posY, 0 });
		auto uiInfo = mSkillIconImages[mPlayingIcon]->GetUIInfomation(0);
		uiInfo.Alpha = 1 - mCurTime / mPlaytime;
		uiInfo.isRender = true;
		mSkillIconImages[mPlayingIcon]->SetUIInfomation(0, uiInfo);
	}
	else
	{
		setIcon(false);
	}
}

void fq::client::CoolTimeIcon::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mInitCoolTimeHandler);
}

void fq::client::CoolTimeIcon::eventProcesInitCoolTime()
{
	mInitCoolTimeHandler = GetScene()->GetEventManager()->RegisterHandle<event::InitCoolTime>
		(
			[this](const event::InitCoolTime& event)
			{
				setIcon(false);
				if (event.playerID == mPlayerID)
				{
					mCurTime = 0;

					if (event.mArmourType == EArmourType::Knight && event.skillKey == 'A')
					{
						mPlayingIcon = 0;
					}
					else if (event.mArmourType == EArmourType::Knight && event.skillKey == 'R')
					{
						mPlayingIcon = 1;
					}
					else if (event.mArmourType == EArmourType::Magic && event.skillKey == 'A')
					{
						mPlayingIcon = 2;
					}
					else if (event.mArmourType == EArmourType::Magic && event.skillKey == 'R')
					{
						mPlayingIcon = 3;
					}
					else if (event.mArmourType == EArmourType::Warrior && event.skillKey == 'A')
					{
						mPlayingIcon = 4;
					}
					else if (event.mArmourType == EArmourType::Warrior && event.skillKey == 'R')
					{
						mPlayingIcon = 5;
					}
					else if (event.mArmourType == EArmourType::Archer && event.skillKey == 'X')
					{
						mPlayingIcon = 6;
					}
					else if (event.mArmourType == EArmourType::Archer && event.skillKey == 'A')
					{
						mPlayingIcon = 7;
					}
					setIcon(true);
				}
			}
		);
}

void fq::client::CoolTimeIcon::setIcon(bool isOn)
{
	mSkillIconImages[mPlayingIcon]->SetIsRender(0, isOn);
}
