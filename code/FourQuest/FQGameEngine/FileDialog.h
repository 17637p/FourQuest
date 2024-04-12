#pragma once

#include "IEditorWindow.h"

#include <filesystem>
#include <unordered_map>
#include <imgui.h>
#include <d3d11.h>

namespace fq::game_engine
{
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
		void Initialize(ID3D11Device* device);

		/// <summary>
		/// 파일 관련 시스템을 랜더링합니다
		/// </summary>
		void Render() override;

	private:
		void beginLeftChildWindow();

		void beginRightChildWindow();

		void beginDirectory(const Path& path);

		void loadIcon();

		void DrawFile(const Path& path);

		ID3D11ShaderResourceView* getIcon(const std::wstring& name);

		ID3D11ShaderResourceView* loadTexture(const Path& path);

	private:
		Path mResourcePath;
		Path mSelectPath;
		ID3D11Device* mDevice;
		ImVec2 mIconSize;
		std::unordered_map <std::wstring, ID3D11ShaderResourceView*> mIconTexture;
		std::unordered_map <Path, ID3D11ShaderResourceView*> mTextures;

		bool mbIsOpen;
	};


}