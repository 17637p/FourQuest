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

		/// <summary>
		/// �������� �̹����� ��ȯ�մѴ� 
		/// </summary>
		/// <param name="name">������ �̸�</param>
		/// <returns>�Ɵ�� �̹���</returns>
		ID3D11ShaderResourceView* GetIcon(const std::wstring& name);

		bool& IsWindowOpen() { return mbIsOpen; }

	private:
		/// ���� ���丮 ���� â
		void beginWindow_FilePathWindow();
		void beginDirectory(const Path& path);
		void beginDragDrop_Directory(const Path& directoryPath);

		/// ���� ���丮�ȿ� ���ҽ� â
		
		void beginWindow_FileList();
		void beginPopupContextWindow_FileList();
		void beginDragDropTarget_FileList();

		void loadIcon();
		void beginDragDrop_File(const Path& path);
		void beginPopupContextItem_File(const Path& path);
		void drawFile(const Path& path);

		void drawTextureImage(const Path& path);
	
		ID3D11ShaderResourceView* loadTexture(const Path& path);

		void selectPath(Path path);
		void setIconSize();

		void clearTexture();
		void clearIconTexture();
		void clearGarbage();

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