#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	/// <summary>
	/// 스케일에 따른 포지션 자동 변환을 위한 클래스
	/// </summary>
	class AutoPositionToScale : public game_module::Component
	{
	public:
		AutoPositionToScale();
		~AutoPositionToScale();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		float mPosX;
		float mPosY;
	};
}
