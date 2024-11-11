#include "SpeechBubbleUI.h"

#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/ScreenManager.h"

#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/Event.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Camera.h"

#include <spdlog/spdlog.h>

fq::client::SpeechBubbleUI::SpeechBubbleUI()
	:mName(""),
	mSequenceSpeechEnterHandler(),
	mSequenceSpeechExitHandler(),
	mImageUIs(),
	mScreenManager(nullptr),
	mWorldOffset(),
	mScreenOffset(),
	mCurTime(0),
	mPlaytime(0.2f),
	mCurIndex(0),
	mIsOn(false)
{
}

fq::client::SpeechBubbleUI::SpeechBubbleUI(const SpeechBubbleUI& other)
	:mName(other.mName),
	mWorldOffset(other.mWorldOffset),
	mScreenOffset(other.mScreenOffset),
	mPlaytime(other.mPlaytime)
{
}

fq::client::SpeechBubbleUI& fq::client::SpeechBubbleUI::operator=(const SpeechBubbleUI& other)
{
	mName = other.mName;
	mWorldOffset = other.mWorldOffset;
	mScreenOffset = other.mScreenOffset;
	mPlaytime = other.mPlaytime;

	return *this;
}

fq::client::SpeechBubbleUI::~SpeechBubbleUI()
{
}

void fq::client::SpeechBubbleUI::OnStart()
{
	mCurTime = 0;
	mCurIndex = 0;
	mIsOn = false;

	mImageUIs.clear();

	auto bubbles = GetGameObject()->GetChildren();
	for (const auto& bubble : bubbles)
	{
		mImageUIs.push_back(bubble->GetComponent<game_module::ImageUI>());
	}

	mScreenManager = GetScene()->GetScreenManager();

	eventProcessSequenceEnterSpeech();
	eventProcessSequenceExitSpeech();
	eventProcessOnUIRender();
}

void fq::client::SpeechBubbleUI::OnUpdate(float dt)
{
	if (mIsOn)
	{
		mCurTime += dt;

		if (mCurTime > mPlaytime)
		{
			mCurTime = 0;
			mCurIndex++;
			if (mCurIndex >= mImageUIs.size())
			{
				mCurIndex = 0;
			}
		}
		DirectX::SimpleMath::Vector3 pos = GetTransform()->GetWorldPosition();
		pos += mWorldOffset;

		float width = mScreenManager->GetFixScreenWidth();
		float height = mScreenManager->GetFixScreenHeight();

		fq::game_module::Camera* mainCamera = nullptr;
		GetScene()->GetEventManager()->FireEvent<fq::event::GetMainCamera>({ &mainCamera });
		auto viewProj = mainCamera->GetViewProjection();
		DirectX::SimpleMath::Vector3 screenPos = DirectX::SimpleMath::Vector3::Transform(pos, viewProj);

		//GetTransform()->SetLocalPosition({ screenPos.x * width * 0.5f, screenPos.y * height * 0.5f, 0 });
		float posX = width * 0.5f + (screenPos.x * width * 0.5f);
		float posY = height * 0.5f - (screenPos.y * height * 0.5f);
		for (const auto& image : mImageUIs)
		{
			image->GetTransform()->SetWorldPosition({ posX, posY, 0 });
		}

		if (mIsRenderingUI)
		{
			play();
		}
		else
		{
			for (int i = 0; i < mImageUIs.size(); i++)
			{
				mImageUIs[i]->SetIsRender(0, false);
			}
		}
	}
}

void fq::client::SpeechBubbleUI::OnDestroy()
{
	GetScene()->GetEventManager()->RemoveHandle(mSequenceSpeechEnterHandler);
	GetScene()->GetEventManager()->RemoveHandle(mSequenceSpeechExitHandler);
	GetScene()->GetEventManager()->RemoveHandle(mUIRenderHandler);
}

std::shared_ptr<fq::game_module::Component> fq::client::SpeechBubbleUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneSpeechBubbleUI = std::dynamic_pointer_cast<SpeechBubbleUI>(clone);

	if (cloneSpeechBubbleUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneSpeechBubbleUI = game_module::ObjectPool::GetInstance()->Assign<SpeechBubbleUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneSpeechBubbleUI = *this;
	}

	return cloneSpeechBubbleUI;
}

void fq::client::SpeechBubbleUI::eventProcessSequenceEnterSpeech()
{
	mSequenceSpeechEnterHandler = GetScene()->GetEventManager()->RegisterHandle<fq::event::SequenceEnterSpeech>
		(
			[this](const fq::event::SequenceEnterSpeech& event)
			{
				if (event.name == mName)
				{
					mIsOn = true;
				}
			}
		);
}

void fq::client::SpeechBubbleUI::eventProcessSequenceExitSpeech()
{
	mSequenceSpeechExitHandler = GetScene()->GetEventManager()->RegisterHandle<fq::event::SequenceExitSpeech>
		(
			[this](const fq::event::SequenceExitSpeech& event)
			{
				for (const auto& image : mImageUIs)
				{
					if (event.name == mName)
					{
						mIsOn = false;

						image->SetIsRender(0, false);
					}
				}
			}
		);
}

void fq::client::SpeechBubbleUI::play()
{
	for (int i = 0; i < mImageUIs.size(); i++)
	{
		if (i == mCurIndex)
		{
			mImageUIs[i]->SetIsRender(0, true);
		}
		else
		{
			mImageUIs[i]->SetIsRender(0, false);
		}
	}
}

void fq::client::SpeechBubbleUI::eventProcessOnUIRender()
{
	mUIRenderHandler = GetScene()->GetEventManager()->RegisterHandle<fq::event::UIRender>(
		[this](const fq::event::UIRender& event)
		{
			mIsRenderingUI = event.bIsRenderingUI;
		}
	);
}
