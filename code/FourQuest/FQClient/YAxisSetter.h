#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class YAxisSetter : public fq::game_module::Component
	{
	public:
		YAxisSetter();
		~YAxisSetter();

		void SetOffsetYAxis(float offsetYAxis) { mOffsetYAxis = offsetYAxis; }
		float GetOffsetYAxis() const { return mOffsetYAxis; }
		void SetDistance(float distance) { mDistance = distance; }
		float GetDistance() const { return mDistance; }

	private:
		virtual void OnFixedUpdate(float dt) override;

		virtual void OnCollisionEnter(const fq::game_module::Collision& collision) override;

		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;
		entt::meta_handle GetHandle() override;

	private:
		float mOffsetYAxis;
		float mDistance;
	};

}