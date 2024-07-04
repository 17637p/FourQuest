#pragma once

#include <memory>

#include "IEditorWindow.h"

#include "../FQReflect/FQReflect.h"
#include "../FQGameModule/GameModule.h"

namespace fq::game_module
{
	class ArticulationData;
	class LinkData;
}

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class ArticulationHierarchy : public IEditorWindow
	{
	public:
		ArticulationHierarchy();
		~ArticulationHierarchy();

		/// <summary>
		/// �������� â�� �ʱ�ȭ�մϴ�
		/// </summary>
		/// <param name="process">�������μ���</param>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// ���������� â�� �����մϴ�
		/// </summary>
		void Finalize();

		/// <summary>
		/// ���� ������ �������� â�� �׸��ϴ�.
		/// </summary>
		virtual void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		/// <summary>
		/// �巡�� ��� ���ӿ�����Ʈ ó��
		/// </summary>
		void dragDropGameWindow();

		/// <summary>
		/// ���� �� ������Ʈ���� ǥ���ϴ� â
		/// </summary>
		void beginBoneHierarchy();

		/// <summary>
		/// ���ӿ�����Ʈ�� �̸��� ��Ÿ���� ��
		/// </summary>
		void beginGameObjectNameBar(fq::game_module::GameObject& object);

		/// <summary>
		/// ���ӿ�����Ʈ�� ���³��� ��
		/// </summary>
		/// <param name="object">������ ǥ���ϴ� ������Ʈ</param>
		void beginGameObjectBar(fq::game_module::GameObject& object);

		/// <summary>
		/// ������ ���������� ǥ���ϴ� â
		/// </summary>
		void beginArticulationHierarchy();

		/// <summary>
		/// 
		/// </summary>
		void beginPopupContextWindow_HierarchyChild();

		/// <summary>
		/// ������ ���� �������� ��Ŭ�� �ϸ� ��ũ�� �������� ��Ÿ���� â
		/// </summary>
		void beginPopupContextItem_Link();



	private:
		GameProcess*	mGameProcess;
		EditorProcess*	mEditorProcess;

		bool			mbIsOpen;
		bool			mbIsBoneHierarchy;
		bool			mbIsArticulationHierarchy;

		fq::game_module::Scene* mScene;
		fq::game_module::EventManager* mEventManager;
		fq::game_module::InputManager* mInputManager;
		
		std::filesystem::path* mPath;
		std::shared_ptr<fq::game_module::GameObject> mObject;
		fq::game_module::SkinnedMeshRenderer* mSkinnedMeshRenderer;
		std::vector<fq::common::Bone> mBones;

		std::shared_ptr<fq::game_module::ArticulationData> mArticulationData;
		std::vector<std::shared_ptr<fq::game_module::LinkData>> mLinkData;
		std::shared_ptr<fq::game_module::LinkData> mSelectLinkData;

		// �̺�Ʈ����
		fq::game_module::EventHandler mSelectObjectHandle;
	};
}