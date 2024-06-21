#pragma once
#pragma once
#include "IEditorWindow.h"

#include <memory>
#include <vector>

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// 게임오브젝트에 대한 정보를 표시한다.
	/// </summary>
	class Inspector : public IEditorWindow
	{
		enum class ViewType
		{
			GameObject,
			AnimationController,
			None,
		};

	public:
		Inspector();
		~Inspector();

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Render() override;
		void Finalize();
		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void getScriptTypes();

		void beginGameObject(std::shared_ptr<fq::game_module::GameObject> object);
		void beginClass(fq::reflect::IHandle* handle, bool bIsComponent = false);
		void beginMember(entt::meta_data data, fq::reflect::IHandle* handle);
		bool beginPOD(entt::meta_any& pod, unsigned int index = -1);
		void beginAddComponent();

		// Animator
		void beginAnimationController(const std::shared_ptr<fq::game_module::AnimatorController>& controller);
		void beginTransitionCondition(fq::game_module::TransitionCondition& condition, int index);
		void beginAnimationStateNode(fq::game_module::AnimationStateNode& stateNode);
		void beginStateBehaviour(fq::game_module::AnimationStateNode& stateNode);

		// 멤버변수 인스펙터 창
		void beginCombo_EnumClass(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputText_String(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputFloat2_Vector2(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputFloat3_Vector3(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputFloat4_Vector4(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputFloat3_Quaternion(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginColorEdit4_Color(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginSequenceContainer(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputText_PrefabResource(entt::meta_data data, fq::reflect::IHandle* handle);

		void beginPopupContextItem_Component(fq::reflect::IHandle* handle);
		void beginIsItemHovered_Comment(entt::meta_data data);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		bool mbIsOpen;

		ViewType mViewType;

		std::shared_ptr<fq::game_module::GameObject> mSelectObject;
		UINT mCurrentAddComponentIndex;
		UINT mCurrentAddBehaviourIndex;
		std::multimap< std::string, entt::meta_type> mComponentTypes;
		std::vector<entt::meta_type> mStateBehaviourTypes;
		DirectX::SimpleMath::Color mPrevColor;

		std::shared_ptr<fq::game_module::AnimatorController> mSelectController;
		std::string mSelectAnimationStateName;

		fq::game_module::EventHandler mSelectObjectHandler;
		fq::game_module::EventHandler mSelectAnimationController;
	};


}