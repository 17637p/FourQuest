#pragma once
#include "Component.h"

#include "../FQCommon/FQCommonPhysics.h"

namespace fq::game_module
{
	class CharacterController :	public Component
	{
	public:
		CharacterController();
		~CharacterController();

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 캐릭터 컨트롤러의 정보를 반환합니다 
		/// </summary>
		fq::physics::CharacterControllerInfo GetControllerInfo() const { return mControllerInfo; }
		
		/// <summary>
		/// 캐릭터 컨트롤러의 정보를 설정합니다 
		/// </summary>
		void SetControllerInfo(fq::physics::CharacterControllerInfo val) { mControllerInfo = val; }
	
		/// <summary>
		/// 캐릭터 이동 정보를 반환합니다
		/// </summary>
		fq::physics::CharacterMovementInfo GetMoventInfo() const { return mMoventInfo; }
		
		/// <summary>
		/// 캐릭터 이동 정보를 설정합니다 
		/// </summary>
		void SetMoventInfo(fq::physics::CharacterMovementInfo val) { mMoventInfo = val; }
	
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		fq::physics::CharacterControllerInfo mControllerInfo;
		fq::physics::CharacterMovementInfo mMoventInfo;

	};

}
