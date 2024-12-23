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
		/// 파괴되는 경우 ExitState 함수를 호출합니다.
		/// </summary>
		void OnDestroy() override;

		/// <summary>
		/// 복사본을 반환합니다 
		/// </summary>
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

		/// <summary>
		/// 컨트롤러를 설정합니다 
		/// </summary>
		void SetController(std::shared_ptr<AnimatorController> controller);
		
		/// <summary>
		/// 하부 컨트롤러를 설정합니다 
		/// </summary>
		void SetLowerController(std::shared_ptr<AnimatorController> lowerController);

		/// <summary>
		/// 컨트롤러를 반환합니다 
		/// </summary>
		AnimatorController& GetController() const { return *mController; }

		/// <summary>
		/// 컨트롤러를 반환합니다
		/// </summary>
		std::shared_ptr<AnimatorController> GetSharedController() const { return mController; }

		/// <summary>
		/// 하부 컨트롤러를 반환합니다 
		/// </summary>
		std::shared_ptr<AnimatorController> GetSharedLowerController() const { return mLowerController; }

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
		/// 로드할 아래쪽 컨트롤러 경로를 반환합니다.
		/// </summary>
		ControllerPath GetLowerControllerPath() const { return mLowerControllerPath; }

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
		std::string GetNodeHierarchyPath() const { return mNodeHierarchyPath; }
		void SetNodeHierarchyPath(const std::string& path);

		/// <summary>
		/// 현재 애니메이션 전환중인지 반환합니다.
		/// </summary>S
		bool IsInTransition()const;

		/// <summary>
		/// 현재 하부 컨트롤러의 애니메이션이 전환중인지 반환합니다.
		/// </summary>
		bool IsLowerInTransition()const;

		/// <summary>
		/// 애니메이션 기본 재생속도를 반환합니다
		/// </summary>
		float GetPlaySpeed() const { return mDefaultPlaySpeed; }

		/// <summary>
		/// 애니메이션 기본 재생속도를 설정합니다 
		/// </summary>
		void SetPlaySpeed(float speed) { mDefaultPlaySpeed = speed; }

		/// <summary>
		/// 애니메이션이 정지 상태인지 반환합니다 
		/// </summary>
		bool IsStopAnimation() const { return mbIsStopAnimation; }

		/// <summary>
		/// 애니메이션을 정지 여부를 설정합니다 
		/// </summary>
		void SetStopAnimation(bool isStop) { mbIsStopAnimation = isStop; }

		void ProcessAnimationEvent(class GameObject* gameObject, class EventManager* eventManager);

		/// <summary>
		/// 애니메이션을 CPU 정보를 갱신할지 여부를 설정합니다. 
		/// </summary>
		void SetUpdateAnimationCPUData(bool bUpdateAnimationCPUData) { mbUpdateAnimationCPUData = bUpdateAnimationCPUData; }
		bool GetUpdateAnimationCPUData() const { return mbUpdateAnimationCPUData; }

		/// <summary>
		/// 애니메이션을 GPU 정보를 갱신할지 여부를 설정합니다. 
		/// </summary>
		void SetUpdateAnimationGPUData(bool bUpdateAnimationGPUData) { mbUpdateAnimationGPUData = bUpdateAnimationGPUData; }
		bool GetUpdateAnimationGPUData() const { return mbUpdateAnimationGPUData; }
		
		void SetCreateAnimationTexture(bool bCreateAnimationTexture) {	mbCreateAnimationTexture = bCreateAnimationTexture;	}
		bool GetCreateAnimationTexture() const { return mbCreateAnimationTexture; }

		/// <summary>
		/// 하부 애니메이션을 적용여부를 설정합니다 
		/// </summary>
		void SetUseLowerController(bool bUseLower);

		/// <summary>
		/// 하부 애니메이션 적용여부를 반환합니다.
		/// </summary>
		bool UseLowerController()const { return mbUseLowerController; }

		/// <summary>
		/// 하부 컨트롤러를 반환합니다 
		/// </summary>
		AnimatorController& GetLowerController() const;

	private:
		entt::meta_handle GetHandle() override { return *this; }

	private:
		ControllerPath mControllerPath;
		std::shared_ptr<AnimatorController> mController;

		bool mbUseLowerController;
		ControllerPath mLowerControllerPath;
		std::shared_ptr<AnimatorController> mLowerController;

		std::string mNodeHierarchyPath;
		std::shared_ptr<fq::graphics::INodeHierarchy> mNodeHierarchy;
		std::shared_ptr<fq::graphics::INodeHierarchyInstance> mNodeHierarchyInstance;

		float mDefaultPlaySpeed;
		bool mbIsStopAnimation;

		// 동적 인스턴싱 사용 유무 체크
		bool mbUpdateAnimationCPUData;
		bool mbUpdateAnimationGPUData;
		bool mbCreateAnimationTexture;

		friend void RegisterMetaData();
	};
}
