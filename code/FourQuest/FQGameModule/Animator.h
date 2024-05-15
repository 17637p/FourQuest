#pragma once
#include "Component.h"

namespace fq::game_module
{
	class AnimatorController;

	/// <summary>
	/// �ִϸ��̼��� �����ϴ� ������Ʈ
	/// </summary>
	class Animator :public Component
	{
		using ControllerPath = std::string;
		using ParameterID = unsigned int;

	public:
		Animator();
		~Animator();
	
		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// ��Ʈ�ѷ��� �����մϴ� 
		/// </summary>
		void SetController(std::shared_ptr<AnimatorController> controller);

		/// <summary>
		/// �ε��� ��Ʈ�ѷ� ��θ� ��ȯ�մϴ�
		/// </summary>
		fq::game_module::Animator::ControllerPath GetControllerPath() const { return mControllerPath; }
		
		/// <summary>
		/// �ε��� ��Ʈ�ѷ� ��θ� �����մϴ� 
		/// </summary>
		void SetControllerPath(fq::game_module::Animator::ControllerPath path) { mControllerPath = path; }

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