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

	cloneUI->mImageObjects.clear();

	return cloneUI;
}

void fq::game_module::ImageUI::setUIInfomations(std::vector<fq::graphics::UIInfo> infomations)
{
	mUIInfomations = std::move(infomations);

	auto scene = GetScene();
	if (scene != nullptr)
	{
		scene->GetEventManager()->FireEvent<fq::event::SetUIInfomations>({GetGameObject()});
	}
}

void fq::game_module::ImageUI::SetUIInfomations(std::vector<fq::graphics::UIInfo> infomations)
{
	for (size_t i = 0; i < mImageObjects.size(); ++i)
	{
		SetUIInfomation(i, infomations[i]);
	}
}



void fq::game_module::ImageUI::SetUIInfomation(size_t index, fq::graphics::UIInfo infomation)
{
	if (mImageObjects[index] == nullptr) return;

	mImageObjects[index]->SetStartX(infomation.StartX);
	mImageObjects[index]->SetStartY(infomation.StartY);
	mImageObjects[index]->SetWidth(infomation.Width);
	mImageObjects[index]->SetHeight(infomation.Height);
	mImageObjects[index]->SetAlpha(infomation.Alpha);
	mImageObjects[index]->SetLayer(infomation.Layer);
	mImageObjects[index]->SetXRatio(infomation.XRatio);
	mImageObjects[index]->SetYRatio(infomation.YRatio);
	mImageObjects[index]->SetScaleX(infomation.ScaleX);
	mImageObjects[index]->SetScaleY(infomation.ScaleY);

	mUIInfomations[index] = infomation;
}
