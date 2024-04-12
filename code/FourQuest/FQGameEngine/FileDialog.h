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
		void Initialize(EditorProcess* editor,ID3D11Device* device);

		/// <summary>
		/// 로드한 리소스 해제
		/// </summary>
		void Finalize();

		/// <summary>
		/// 파일 관련 시스템을 랜더링합니다
		/// </summary>
		void Render() override;

	private:

		/// <summary>
		/// 파일 디랙토리 관련 창
		/// </summary>
		void beginLeftChildWindow();
		void beginDirectory(const Path& path);


		/// <summary>
		/// 파일 디랙토리안에 리소스 창
		/// </summary>
		void beginRightChildWindow();

		void beginPopupContextWindow();

		/// <summary>
		/// 아이콘 로드
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
		/// 마우스가 min max 내부에 있는지 확인합니다
		/// </summary>
		/// <param name="min">왼쪽 상단</param>
		/// <param name="max">오른쪽 하단</param>
		/// <returns>있으면 true, 그렇지 않으면 false</returns>
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