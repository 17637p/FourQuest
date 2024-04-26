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
	/// 
	/// Unity에서는 리소스에 대한 정보도 같이 표시해준다 여기서도 기능을 넣을까?
	/// </summary>
	class Inspector : public IEditorWindow
	{
	public:
		Inspector();
		~Inspector();

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Render() override;
		void Finalize();
		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		void getComponentTypes();

		void beginGameObject(std::shared_ptr<fq::game_module::GameObject> object);
		void beginClass(fq::reflect::IHandle* handle, bool bIsComponent =false);
		void beginMember(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginAddComponent();

		// 데이터 타입 관련
		void beginCombo_EnumClass(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputText_String(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputFloat2_Vector2(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputFloat3_Vector3(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputFloat4_Vector4(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginInputFloat3_Quaternion(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginColorEdit4_Color(entt::meta_data data, fq::reflect::IHandle* handle);
		void beginSequenceContainer(entt::meta_data data, fq::reflect::IHandle* handle);

		void beginPopupContextItem_Component(fq::reflect::IHandle* handle);
		void beginIsItemHovered_Comment(entt::meta_data data);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		fq::game_module::InputManager* mInputManager;
		
		bool mbIsOpen;

		std::shared_ptr<fq::game_module::GameObject> mSelectObject;
		fq::game_module::EventHandler mSelectObjectHandler;
		std::vector<entt::meta_type> mComponentTypes;
		UINT mCurrentAddComponentIndex;

		DirectX::SimpleMath::Color mPrevColor;
	};


}