#pragma once
#include "Component.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	class AnimatorController;
	class SkinnedMeshRenderer;

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
		/// �ִϸ����� ���¸� ������Ʈ �մϴ�.
		/// </summary>
		void OnUpdate(float dt) override;

		/// <summary>
		/// ���纻�� ��ȯ�մϴ� 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// ��Ʈ�ѷ��� �����մϴ� 
		/// </summary>
		void SetController(std::shared_ptr<AnimatorController> controller);

		/// <summary>
		/// ��Ʈ�ѷ��� ��ȯ�մϴ� 
		/// </summary>
		AnimatorController& GetController() const { return *mController; }

		/// <summary>
		/// ��Ʈ�ѷ��� ��ȯ�մϴ�
		/// </summary>
		std::shared_ptr<AnimatorController> GetSharedController()const { return mController; }

		bool GetHasController() const { return mController != nullptr; }

		/// <summary>
		/// �ִϸ��̼� ���¸� ������Ʈ �մϴ� 
		/// </summary>
		void UpdateState(float dt);

		/// <summary>
		/// �ִϸ��̼��� �������� ������Ʈ �մϴ� 
		/// </summary>
		void UpdateAnimation(float dt);

		/// <summary>
		/// �ε��� ��Ʈ�ѷ� ��θ� ��ȯ�մϴ�
		/// </summary>
		ControllerPath GetControllerPath() const { return mControllerPath; }

		/// <summary>
		/// �ε��� ��Ʈ�ѷ� ��θ� �����մϴ� 
		/// </summary>
		void SetControllerPath(ControllerPath path) { mControllerPath = path; }

		void SetParameterInt(const std::string& id, int val);
		void SetParameterFloat(const std::string& id, float val);
		void SetParameterBoolean(const std::string& id, bool val);
		void SetParameterTrigger(const std::string& id);
		void SetParameterOffTrigger(const std::string& id);

		/// <summary>
		/// ��� ���� ���� �ν��Ͻ� Getter/Setter
		/// </summary>
		void SetNodeHierarchyInstance(std::shared_ptr<fq::graphics::INodeHierarchyInstance> nodeHierarchyInstance) { mNodeHierarchyInstance = nodeHierarchyInstance; }
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> GetSharedNodeHierarchyInstance() const { return mNodeHierarchyInstance; }
		fq::graphics::INodeHierarchyInstance& GetNodeHierarchyInstance() { return *mNodeHierarchyInstance; }
		bool GetHasNodeHierarchyInstance() { return mNodeHierarchyInstance != nullptr; }

		/// <summary>
		/// ��� ���� ���� ���ҽ�  Getter/Setter
		/// </summary>
		void SetNodeHierarchy(std::shared_ptr<fq::graphics::INodeHierarchy> nodeHierarchy) { mNodeHierarchy = nodeHierarchy; }
		std::shared_ptr<fq::graphics::INodeHierarchy> GetSharedNodeHierarchy() const { return mNodeHierarchy; }
		fq::graphics::INodeHierarchy& GetNodeHierarchy() { return *mNodeHierarchy; }
		bool GetHasNodeHierarchy() const { return mNodeHierarchy != nullptr; }

		/// <summary>
		/// ��� ���� ���� ������ ��� Getter/Setter
		/// </summary>
		std::string GetNodeHierarchyModelPath() const { return mNodeHierarchyModelPath; }
		void SetNodeHierarchyModelPath(const std::string& modelPath) { mNodeHierarchyModelPath = modelPath; }

		/// <summary>
		/// ���� �ִϸ��̼� ��ȯ������ ��ȯ�մϴ�.
		/// </summary>
		bool IsInTransition()const;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		ControllerPath mControllerPath;
		std::shared_ptr<AnimatorController> mController;

		std::string mNodeHierarchyModelPath;
		std::shared_ptr<fq::graphics::INodeHierarchy> mNodeHierarchy;
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> mNodeHierarchyInstance;
	};

}
