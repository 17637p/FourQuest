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

	/// <summary>
	/// 파일 시스템을 관리하는 창
	/// </summary>
	class FileDialog : public IEditorWindow
	{
		using Path = std::filesystem::path;

	public:
		FileDialog();
		~FileDialog();

		/// <summary>
		/// 현재 리소스 경로를 초기화합니다
		/// </summary>
		void Initialize(GameProcess* game, EditorProcess* editor,ID3D11Device* device);

		/// <summary>
		/// 로드한 리소스 해제
		/// </summary>
		void Finalize();

		/// <summary>
		/// 파일 관련 시스템을 랜더링합니다
		/// </summary>
		void Render() override;

		/// <summary>
		/// 윈도우에서 드랍된 파일들을 처리합니다
		/// </summary>
		void ProcessWindowDropFile();

	private:

		/// <summary>
		/// 파일 디랙토리 관련 창
		/// </summary>
		void beginWindow_FilePathWindow();
		void beginDirectory(const Path& path);
		void beginDragDrop_Directory(const Path& directoryPath);

		/// <summary>
		/// 파일 디랙토리안에 리소스 창
		/// </summary>
		void beginWindow_FileList();
		void beginPopupContextWindow_FileList();
		void beginDragDropTarget_FileList();

		void loadIcon();
		void beginDragDrop_File(const Path& path);
		void beginPopupContextItem_File(const Path& path);
		void drawFile(const Path& path);

		void drawTextureImage(const Path& path);
		ID3D11ShaderResourceView* getIcon(const std::wstring& name);
		ID3D11ShaderResourceView* loadTexture(const Path& path);

		void SelectPath(Path path);

		void clearTexture();
		void clearIconTexture();

		bool isMouseHoveringRect(const ImVec2& min, const ImVec2& max);
	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		Path mResourcePath;
		Path mSelectPath;
		Path mDragDropPath;

		ID3D11Device* mDevice;
		ImVec2 mIconSize;
		std::unordered_map <std::wstring, ID3D11ShaderResourceView*> mIconTexture;
		std::unordered_map <Path, ID3D11ShaderResourceView*> mTextures;

		bool mbIsOpen;
	};


}