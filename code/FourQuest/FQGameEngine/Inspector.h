#pragma once
#include "IEditorWindow.h"

#include <memory>

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

	private:
		void beginGameObject(std::shared_ptr<fq::game_module::GameObject> object);

		void beginComponent(entt::id_type id, fq::game_module::Component* component);

		void beginClass(entt::meta_type type, entt::meta_any& object);

		void beginMember(entt::meta_data type, entt::meta_any& member);

		void beginCombo_EnumClass(entt::meta_type type, entt::meta_any& enumToAny);

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		std::shared_ptr<fq::game_module::GameObject> mSelectObject;
		fq::game_module::EventHandler mSelectObjectHandle;
	};


}