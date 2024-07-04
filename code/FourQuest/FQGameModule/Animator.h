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
		/// 애니메이션 정보를 관리하는 계층구조 인스턴스를 반환합니다.
		/// </summary>
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> GetNodeHierarchyInstance() const { return mNodeHierarchyInstance; }
		void SetNodeHierarchyInstance(std::shared_ptr<fq::graphics::INodeHierarchyInstance> nodeHierarchyInstance)  { mNodeHierarchyInstance = nodeHierarchyInstance; }

		void SetNodeHierarchy(std::shared_ptr<fq::graphics::INodeHierarchy> nodeHierarchy) { mNodeHierarchy = nodeHierarchy; }
		std::shared_ptr<fq::graphics::INodeHierarchy> GetNodeHierarchy() const { return mNodeHierarchy; }

		const std::string& GetModelPath() const { return mModelPath; }
		void SetModelPath(const std::string& modelPath) { mModelPath = modelPath; }

		/// <summary>
		/// 현재 애니메이션 전환중인지 반환합니다.
		/// </summary>
		bool IsInTransition()const;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		ControllerPath mControllerPath;
		std::shared_ptr<AnimatorController> mController;
		//std::vector<SkinnedMeshRenderer*> mSkinnedMeshs;

		std::string mModelPath;
		std::shared_ptr<fq::graphics::INodeHierarchy> mNodeHierarchy;
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> mNodeHierarchyInstance;
	};

}
