#pragma once
#include "Component.h"
#include "../FQCommon/IFQRenderResource.h"

namespace fq::game_module
{
	class AnimatorController;
	class SkinnedMeshRenderer;

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
		/// 애니메이터 상태를 업데이트 합니다.
		/// </summary>
		void OnUpdate(float dt) override;

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 컨트롤러를 설정합니다 
		/// </summary>
		void SetController(std::shared_ptr<AnimatorController> controller);

		/// <summary>
		/// 컨트롤러를 반환합니다 
		/// </summary>
		AnimatorController& GetController() const { return *mController; }

		/// <summary>
		/// 컨트롤러를 반환합니다
		/// </summary>
		std::shared_ptr<AnimatorController> GetSharedController()const { return mController; }

		bool GetHasController() const { return mController != nullptr; }

		/// <summary>
		/// 애니메이션 상태를 업데이트 합니다 
		/// </summary>
		void UpdateState(float dt);

		/// <summary>
		/// 애니메이션의 프레임을 업데이트 합니다 
		/// </summary>
		void UpdateAnimation(float dt);

		/// <summary>
		/// 로드할 컨트롤러 경로를 반환합니다
		/// </summary>
		ControllerPath GetControllerPath() const { return mControllerPath; }

		/// <summary>
		/// 로드할 컨트롤러 경로를 설정합니다 
		/// </summary>
		void SetControllerPath(ControllerPath path) { mControllerPath = path; }

		void SetParameterInt(const std::string& id, int val);
		void SetParameterFloat(const std::string& id, float val);
		void SetParameterBoolean(const std::string& id, bool val);
		void SetParameterTrigger(const std::string& id);
		void SetParameterOffTrigger(const std::string& id);

		/// <summary>
		/// 노드 계층 구조 인스턴스 Getter/Setter
		/// </summary>
		void SetNodeHierarchyInstance(std::shared_ptr<fq::graphics::INodeHierarchyInstance> nodeHierarchyInstance) { mNodeHierarchyInstance = nodeHierarchyInstance; }
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> GetSharedNodeHierarchyInstance() const { return mNodeHierarchyInstance; }
		fq::graphics::INodeHierarchyInstance& GetNodeHierarchyInstance() { return *mNodeHierarchyInstance; }
		bool GetHasNodeHierarchyInstance() { return mNodeHierarchyInstance != nullptr; }

		/// <summary>
		/// 노드 계층 구조 리소스  Getter/Setter
		/// </summary>
		void SetNodeHierarchy(std::shared_ptr<fq::graphics::INodeHierarchy> nodeHierarchy) { mNodeHierarchy = nodeHierarchy; }
		std::shared_ptr<fq::graphics::INodeHierarchy> GetSharedNodeHierarchy() const { return mNodeHierarchy; }
		fq::graphics::INodeHierarchy& GetNodeHierarchy() { return *mNodeHierarchy; }
		bool GetHasNodeHierarchy() const { return mNodeHierarchy != nullptr; }

		/// <summary>
		/// 노드 계층 구조 리스소 경로 Getter/Setter
		/// </summary>
		std::string GetNodeHierarchyModelPath() const { return mNodeHierarchyModelPath; }
		void SetNodeHierarchyModelPath(const std::string& modelPath) { mNodeHierarchyModelPath = modelPath; }

		/// <summary>
		/// 현재 애니메이션 전환중인지 반환합니다.
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
