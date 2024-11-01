#include "SpriteAnimationUI.h"

#include "../FQCommon/IFQRenderObject.h"

#include "Transform.h"
#include "Scene.h"
#include "Event.h"
#include "EventManager.h"

#include <spdlog/spdlog.h>

fq::game_module::SpriteAnimationUI::SpriteAnimationUI()
	:mSpriteAnimationObject(nullptr),
	mSpriteAnimationInfo()
{
}

fq::game_module::SpriteAnimationUI::~SpriteAnimationUI()
{
}

void fq::game_module::SpriteAnimationUI::OnUpdate(float dt)
{
	if (mSpriteAnimationObject == nullptr)
	{
		return;
	}

	Transform* myTransform = GetComponent<Transform>();

	SetPosition(myTransform->GetWorldPosition().x, myTransform->GetWorldPosition().y);
	SetScale(myTransform->GetWorldScale().x, myTransform->GetWorldScale().y);

	mSpriteAnimationInfo.CurTime += dt;
	if (mSpriteAnimationInfo.CurTime > mSpriteAnimationInfo.Speed)
	{
		mSpriteAnimationInfo.CurImage++;
		mSpriteAnimationInfo.CurTime = 0;
		if (mSpriteAnimationInfo.CurImage >= mSpriteAnimationInfo.ImageNum)
		{
			mSpriteAnimationInfo.CurImage = 0;
		}
	}
}

void fq::game_module::SpriteAnimationUI::SetIsRender(bool isRender)
{
	mSpriteAnimationInfo.isRender = isRender;

	// 임시추가_홍지환
	GetSpriteAnimationObject()->SetSpriteInfo(mSpriteAnimationInfo);
}

void fq::game_module::SpriteAnimationUI::SetPosition(float startX, float startY)
{
	mSpriteAnimationInfo.StartX = startX;
	mSpriteAnimationInfo.StartY = startY;

	GetSpriteAnimationObject()->SetSpriteInfo(mSpriteAnimationInfo);
}

void fq::game_module::SpriteAnimationUI::SetScale(float scaleX, float scaleY)
{
	mSpriteAnimationInfo.ScaleX = scaleX;
	mSpriteAnimationInfo.ScaleY = scaleY;
}

std::shared_ptr<fq::game_module::Component> fq::game_module::SpriteAnimationUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto spriteUI = std::dynamic_pointer_cast<SpriteAnimationUI>(clone);

	if (spriteUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		spriteUI = ObjectPool::GetInstance()->Assign<SpriteAnimationUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*spriteUI = *this;
	}

	return spriteUI;
}

void fq::game_module::SpriteAnimationUI::SetSpriteInfo(fq::graphics::SpriteInfo val)
{
	mSpriteAnimationInfo = val;
	game_module::Scene* scene = GetScene();
	if (scene != nullptr)
	{
		scene->GetEventManager()->FireEvent<fq::event::SetSpriteAnimationInformation>({ GetGameObject() });
	}
}
