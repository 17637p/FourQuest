#pragma once
#include "IEditorWindow.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <filesystem>

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	class Setting : public IEditorWindow
	{
	public:
		Setting();

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Render() override;

		bool& IsWindowOpen() { return mbIsOpen; }

		bool UseSnap() const { return mbUseSnap; }

		bool UseRootPicking()const { return mbUseRootPicking; }

		float* GetSnap() { return &mSnap[0]; }

		ImGuizmo::MODE GetMode();

		bool CanEditPath() const { return mbCanEditPath; }
	private:
		void beginChild_GizumoSetting();
		void beginChild_GraphicsSetting();
		void beginChild_InspectorSetting();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		bool mbUseSnap;
		float mSnap[3];
		ImGuizmo::MODE mMode;

		bool mbIsOpen;
		bool mbUseGrayScale;

		//  ��ŷ �ɼ�
		bool mbUseRootPicking;

		// ���͸��� ���� ���
		std::filesystem::path mMaterialDirectory;

		// �ν�����
		bool mbCanEditPath;// �巡�� ����� �ƴ� �Է��� ���� �� ����
	};
}