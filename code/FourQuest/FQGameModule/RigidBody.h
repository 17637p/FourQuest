#pragma once
#include "Component.h"

namespace fq::game_module
{
	class RigidBody : public Component
	{
	public:
		RigidBody();
		~RigidBody();

		/// <summary>	
		/// 복사본을 할당해서 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone = nullptr)const override;


		/// <summary>
		/// true면 static body, false 는 dynamic body 입니다
		/// </summary>
		bool IsStatic() const { return mbIsStatic; }

		/// <summary>
		/// true면 static body, false 는 dynamic body 입니다
		/// </summary>
		void SetStatic(bool isStatic) { mbIsStatic = isStatic; }

	private:
		entt::meta_handle GetHandle() override;

	private:
		bool mbIsStatic;
	};


}