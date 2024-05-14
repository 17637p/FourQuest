#pragma once
#include "Component.h"

namespace fq::game_module
{
	class AnimatorController;

	/// <summary>
	/// 애니메이션을 관리하는 컴포넌트
	/// </summary>
	class Animator :public Component
	{
		using ControllerPath = std::string;
		using ParameterID = unsigned int;

	public:
		Animator();
		~Animator();
	
		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 컨트롤러를 설정합니다 
		/// </summary>
		void SetController(std::shared_ptr<AnimatorController> controller);

		/// <summary>
		/// 로드할 컨트롤러 경로를 반환합니다
		/// </summary>
		fq::game_module::Animator::ControllerPath GetControllerPath() const { return mControllerPath; }
		
		/// <summary>
		/// 로드할 컨트롤러 경로를 설정합니다 
		/// </summary>
		void SetControllerPath(fq::game_module::Animator::ControllerPath path) { mControllerPath = path; }


		void SetParameterInt(ParameterID id, int val);
		void SetParameterFloat(ParameterID id, float val);
		void SetParameterBoolean(ParameterID id, bool val);
		void SetParameterTrigger(ParameterID id);
		void SetParameterInt(const std::string& id, int val);
		void SetParameterFloat(const std::string& id, float val);
		void SetParameterBoolean(const std::string& id, bool val);
		void SetParameterTrigger(const std::string& id);
		
	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		ControllerPath mControllerPath;
		std::shared_ptr<AnimatorController> mController;


	};

}
