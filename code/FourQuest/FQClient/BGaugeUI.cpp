#include "BGaugeUI.h"

#include "../FQGameModule/Camera.h"
#include "../FQGameModule/ImageUI.h"
#include "../FQGameModule/Transform.h"

namespace fq::client
{
	BGaugeUI::BGaugeUI()
		: mbIsVisible(false)
		, mGaugeRatio()
		, mWorldOffset{}
		, mGaugeSize{100.f, 100.f}
		, mTransform(nullptr)
		, mImageUI(nullptr)
		, mMainCamera(nullptr)
	{
	}

	BGaugeUI::~BGaugeUI()
	{
	}

	void BGaugeUI::OnStart()
	{
		mTransform = GetComponent<game_module::Transform>();
		mImageUI = GetComponent<game_module::ImageUI>();

		// MainCamera 가져오기 
		auto view = GetScene()->GetComponentView<game_module::Camera>();
		for (auto& object : view)
		{
			auto camera = object.GetComponent<game_module::Camera>();

			if (object.GetName() == "MainCamera")
			{
				mMainCamera = camera;
			}
		}

		auto infomations = mImageUI->GetUIInfomations();

		for (int i = 0; i < infomations.size(); i++)
		{
			infomations[i].Width = mGaugeSize.x;
			infomations[i].Height = mGaugeSize.y;
		}

		mImageUI->SetUIInfomations(infomations);
	}

	void BGaugeUI::OnUpdate(float dt)
	{
		if (mbIsVisible)
		{
			setUIInfo();
		}
	}

	void BGaugeUI::setUIInfo()
	{
		if (GetGameObject()->IsDestroyed())
			return;

		using namespace DirectX::SimpleMath;

		Vector3 pos = mTransform->GetWorldPosition();
		pos.x += mWorldOffset.x;
		pos.y += mWorldOffset.y;

		float width = GetScene()->GetScreenManager()->GetFixScreenWidth();
		float height = GetScene()->GetScreenManager()->GetFixScreenHeight();

		auto viewProj = mMainCamera->GetViewProjection();
		Vector3 screenPos = Vector3::Transform(pos, viewProj);
		auto infomations = mImageUI->GetUIInfomations();

		for (int i = 0; i < infomations.size() - 1; i++)
		{
			infomations[i].StartX = width * 0.5f + (screenPos.x * width * 0.5f);
			infomations[i].StartY = height * 0.5f - (screenPos.y * height * 0.5f);
		}

		infomations[2].StartX = width * 0.5f + (screenPos.x * width * 0.5f) - mGaugeSize.x / 2.f;
		infomations[2].StartY = height * 0.5f - (screenPos.y * height * 0.5f) - mGaugeSize.y / 2.f;

		mImageUI->SetFillDegree(2, mGaugeRatio);
		mImageUI->SetUIInfomations(infomations);
	}

	void BGaugeUI::SetVisible(bool isVisible)
	{
		mbIsVisible = isVisible;

		auto infomations = mImageUI->GetUIInfomations();
		for (int i = 0; i < 3; ++i)
		{
			infomations[i].isRender = isVisible;
		}

		mImageUI->SetUIInfomations(infomations);
		setUIInfo();
	}

	void BGaugeUI::SetRatio(float ratio)
	{
		mGaugeRatio = std::clamp(ratio, 0.f, 359.9f);
	}

	std::shared_ptr<fq::game_module::Component> BGaugeUI::Clone(std::shared_ptr<Component> clone) const
	{
		auto cloneBGauge = std::dynamic_pointer_cast<BGaugeUI>(clone);

		if (cloneBGauge == nullptr) // 새로 생성해서 복사본을 준다
		{
			cloneBGauge = game_module::ObjectPool::GetInstance()->Assign<BGaugeUI>(*this);
		}
		else // clone에 데이터를 복사한다.
		{
			// 기본 대입 연산자 호출한다.
			*cloneBGauge = *this;
		}

		return cloneBGauge;
	}

}
