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
	/// ���ӿ�����Ʈ�� ���� ������ ǥ���Ѵ�.
	/// 
	/// Unity������ ���ҽ��� ���� ������ ���� ǥ�����ش� ���⼭�� ����� ������?
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

		// ������ Ÿ�� ����
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