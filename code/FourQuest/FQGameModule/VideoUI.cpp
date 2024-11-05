#include "VideoUI.h"

#include "Scene.h"
#include "Transform.h"
#include "Event.h"
#include "EventManager.h"
#include "InputManager.h"

#include "../FQCommon/IFQRenderObject.h"

fq::game_module::VideoUI::VideoUI()
	:mVideoObject(nullptr)
	, mVIdeoInfo{}
{
}

fq::game_module::VideoUI::~VideoUI()
{
}

std::shared_ptr<fq::game_module::Component> fq::game_module::VideoUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto videoUI = std::dynamic_pointer_cast<VideoUI>(clone);

	if (videoUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		videoUI = ObjectPool::GetInstance()->Assign<VideoUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*videoUI = *this;
	}

	return videoUI;
}

void fq::game_module::VideoUI::SetIsRender(bool isRender)
{
	mVIdeoInfo.isRender = isRender;

	mVideoObject->SetVideoInfo(mVIdeoInfo);
}

void fq::game_module::VideoUI::OnUpdate(float dt)
{
	mVIdeoInfo = mVideoObject->GetVideoInfo();
	Transform* myTransform = GetComponent<Transform>();

	SetTextPosition(myTransform->GetWorldPosition().x, myTransform->GetWorldPosition().y);
	mVIdeoInfo.PlayTime += dt;

	auto input = GetScene()->GetInputManager();
	for (int i = 0; i < 4; i++)
	{
		if (input->GetPadKeyState(i, EPadKey::A) == EKeyState::Hold)
		{
			mVIdeoInfo.PlayTime += dt;
		}
	}

	mVideoObject->SetVideoInfo(mVIdeoInfo);
}

void fq::game_module::VideoUI::SetVideoInfo(fq::graphics::VideoInfo val)
{
	mVIdeoInfo = val;
	game_module::Scene* scene = GetScene();
	if (scene != nullptr)
	{
		scene->GetEventManager()->FireEvent<fq::event::SetVideoInformation>({ GetGameObject() });
	}
}

void fq::game_module::VideoUI::SetTextPosition(float startX, float startY)
{
	mVIdeoInfo.StartX = startX;
	mVIdeoInfo.StartY = startY;

	mVideoObject->SetVideoInfo(mVIdeoInfo);
}

void fq::game_module::VideoUI::OnStart()
{
	mVIdeoInfo.isReset = true;

	mVideoObject->SetVideoInfo(mVIdeoInfo);
}

