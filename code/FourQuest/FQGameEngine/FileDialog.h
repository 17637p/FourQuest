#pragma once

#include "IEditorWindow.h"

#include <filesystem>
#include <unordered_map>
#include <imgui.h>
#include <d3d11.h>

namespace fq::game_engine
{
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
		void Initialize(EditorProcess* editor,ID3D11Device* device);

		/// <summary>
		/// �ε��� ���ҽ� ����
		/// </summary>
		void Finalize();

		/// <summary>
		/// ���� ���� �ý����� �������մϴ�
		/// </summary>
		void Render() override;

	private:

		/// <summary>
		/// ���� ���丮 ���� â
		/// </summary>
		void beginLeftChildWindow();
		void beginDirectory(const Path& path);


		/// <summary>
		/// ���� ���丮�ȿ� ���ҽ� â
		/// </summary>
		void beginRightChildWindow();

		void beginPopupContextWindow();

		/// <summary>
		/// ������ �ε�
		/// </summary>
		void loadIcon();

		void beginDragDrop(const Path& path);

		void drawFile(const Path& path);

		void drawTextureImage(const Path& path);

		ID3D11ShaderResourceView* getIcon(const std::wstring& name);

		ID3D11ShaderResourceView* loadTexture(const Path& path);

		void clearTexture();
		void clearIconTexture();

		/// <summary>
		/// ���콺�� min max ���ο� �ִ��� Ȯ���մϴ�
		/// </summary>
		/// <param name="min">���� ���</param>
		/// <param name="max">������ �ϴ�</param>
		/// <returns>������ true, �׷��� ������ false</returns>
		bool isMouseHoveringRect(const ImVec2& min, const ImVec2& max);

	private:
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