#include "ImageUI.h"

#include "Scene.h"
#include "EventManager.h"
#include "Event.h"

fq::game_module::ImageUI::ImageUI()
	:mUIInfomations{}
{}

fq::game_module::ImageUI::~ImageUI()
{

}

std::shared_ptr<fq::game_module::Component> fq::game_module::ImageUI::Clone(std::shared_ptr<Component> clone /* = nullptr */) const
{
	auto cloneUI = std::dynamic_pointer_cast<ImageUI>(clone);

	if (cloneUI == nullptr) // 새로 생성해서 복사본을 준다
	{
		cloneUI = ObjectPool::GetInstance()->Assign<ImageUI>(*this);
	}
	else // clone에 데이터를 복사한다.
	{
		// 기본 대입 연산자 호출한다.
		*cloneUI = *this;
	}

	return cloneUI;
}

void fq::game_module::ImageUI::SetUIInfomations(std::vector<fq::graphics::UIInfo> val)
{
	mUIInfomations = std::move(val);

	auto scene = GetScene();
	if (scene != nullptr)
	{
		scene->GetEventManager()->FireEvent<fq::event::SetUIInfomations>({GetGameObject()});
	}
}
