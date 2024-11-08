#include "GameOverHandler.h"
#include "../FQGameModule/Scene.h"
#include "../FQGameModule/EventManager.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/CharacterController.h"

fq::client::GameOverHandler::GameOverHandler()
	: mbIsSelect(false)
	, mInputStartTIme(0.5f)
	, mElapsed(0.f)
{
}

std::shared_ptr<fq::game_module::Component> fq::client::GameOverHandler::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	std::shared_ptr<GameOverHandler> cloned = std::dynamic_pointer_cast<GameOverHandler>(clone);

	if (cloned == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloned = game_module::ObjectPool::GetInstance()->Assign<GameOverHandler>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloned = *this;
	}

	return cloned;
}

void fq::client::GameOverHandler::OnStart()
{
	mbIsSelect = false;
	mElapsed = 0.f;
	GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ false });
}

void fq::client::GameOverHandler::OnUpdate(float dt)
{
	GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ false });

	if (mbIsSelect)
	{
		return;
	}

	mElapsed += dt;

	if (mInputStartTIme > mElapsed)
	{
		return;
	}

	auto currentScene = GetScene();

	for (unsigned int i = 0; i < 4; ++i)
	{
		auto input = currentScene->GetInputManager();

		// 다시하기
		if (input->IsPadKeyState(i, EPadKey::Y, EKeyState::Tap))
		{
			currentScene->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ currentScene->GetSceneName(), true });
			mbIsSelect = true;
			return;
		}
		else if (input->IsPadKeyState(i, EPadKey::Start, EKeyState::Tap))
		{
			currentScene->GetEventManager()->FireEvent<fq::event::RequestChangeScene>({ "TitleUI", true });
			mbIsSelect = true;
			return;
		}
	}
}

void fq::client::GameOverHandler::OnDestroy()
{
	GetScene()->GetEventManager()->FireEvent<fq::event::UIRender>({ true });
}
