#pragma once

#include "IEditorWindow.h"

#include <filesystem>
#include <unordered_map>
#include <imgui.h>
#include <d3d11.h>

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;
	class ImageSystem;

	/// <summary>
	/// ���� �ý����� �����ϴ� â
	/// </summary>
	class FileDialog : public IEditorWindow
	{
		using Path = std::filesystem::path;

	public:
		FileDialog();
		~FileDialog();

		/// <summary>
		/// ���� ���ҽ� ��θ� �ʱ�ȭ�մϴ�
		/// </summary>
		void Initialize(GameProcess* game, EditorProcess* editor);

		/// <summary>
		/// �ε��� ���ҽ� ����
		/// </summary>
		void Finalize();

		/// <summary>
		/// ���� ���� �ý����� �������մϴ�
		/// </summary>
		void Render() override;

		/// <summary>
		/// �����쿡�� ����� ���ϵ��� ó���մϴ�
		/// </summary>
		void ProcessWindowDropFile();

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		/// ���� ���丮 ���� â
		void beginWindow_FilePathWindow();
		void beginDirectory(const Path& path);
		void beginDragDrop_Directory(const Path& directoryPath);
		void beginFavorite();

		/// ���� ���丮�ȿ� ���ҽ� â
		void beginWindow_FileList();
		void beginPopupContextWindow_FileList();
		void beginDragDropTarget_FileList();

		void beginDragDrop_File(const Path& path);
		void beginPopupContextItem_File(const Path& path); 
		void drawFile(const Path& path);

		void selectPath(Path path);
		void setIconSize();

		void clearGarbage();

		bool isMouseHoveringRect(const ImVec2& min, const ImVec2& max);

		void changeDirectoryPath(Path prev, Path current);	

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;
		ImageSystem* mImageSystem;

		Path mResourcePath;
		Path mSelectPath;
		Path mDragDropPath;
		Path mSearchBar;
		std::string mFileSearch;

		ID3D11Device* mDevice;
		ImVec2 mIconSize;

		bool mbIsFindAllDirectory;
		bool mbIsOpen;

		std::vector<Path> mQueryExtension;
	};


}