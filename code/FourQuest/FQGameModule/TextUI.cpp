#include "TextUI.h"

#include "Scene.h"
#include "Event.h"
#include "EventManager.h"

#include "Transform.h"

#include "../FQCommon/IFQRenderObject.h"

std::shared_ptr<fq::game_module::Component> fq::game_module::TextUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto textUI = std::dynamic_pointer_cast<TextUI>(clone);

	if (textUI == nullptr) // ���� �����ؼ� ���纻�� �ش�
	{
		textUI = ObjectPool::GetInstance()->Assign<TextUI>(*this);
	}
	else // clone�� �����͸� �����Ѵ�.
	{
		// �⺻ ���� ������ ȣ���Ѵ�.
		*textUI = *this;
	}

	return textUI;
}

void fq::game_module::TextUI::SetTextInfo(fq::graphics::TextInfo val)
{
	mTextInfo = val;
	game_module::Scene* scene = GetScene();
	if (scene != nullptr)
	{
		scene->GetEventManager()->FireEvent<fq::event::SetTextInformation>({ GetGameObject() });
	}
}

fq::game_module::TextUI::TextUI()
{
}

fq::game_module::TextUI::~TextUI()
{
}

void fq::game_module::TextUI::SetTextPosition(float centerX, float centerY)
{
	mTextInfo.CenterX = centerX;
	mTextInfo.CenterY = centerY;

	mTextObject->SetTextInformation(mTextInfo);
}

void fq::game_module::TextUI::OnUpdate(float dt)
{
	Transform* myTransform = GetComponent<Transform>();

	SetTextPosition(myTransform->GetWorldPosition().x, myTransform->GetWorldPosition().y);
}