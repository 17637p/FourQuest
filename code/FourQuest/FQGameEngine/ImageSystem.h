#pragma once

#include "../FQGameModule/EventHandler.h"

#include <filesystem>
#include <unordered_map>
#include <d3d11.h>
#include <imgui.h>
#include <mutex>
#include <shared_mutex>
#include <future>

namespace fq::game_engine
{
	class GameProcess;
	class EditorProcess;

	/// <summary>
	/// Editor에서 사용하는 이미지를 관리합니다
	/// </summary>
	class ImageSystem
	{
		using Path = std::filesystem::path;

	public:
		ImageSystem();
		~ImageSystem();

		void Initialize(GameProcess* game, EditorProcess* editor);
		void Finalize();

		ID3D11ShaderResourceView* GetIcon(const std::wstring& name);
		void DrawTextureImage(const Path& path, ImVec2 size);

	private:
		ID3D11ShaderResourceView* loadTexture(const Path& path)const ;
		void loadIcon();
		void clearTexture();
		void clearIconTexture();

	private:
		GameProcess* mGameProcess;
		EditorProcess* mEditorProcess;

		ID3D11Device* mDevice;
		Path mResourcePath;
		std::unordered_map<std::wstring, ID3D11ShaderResourceView*> mIconTexture;
		std::unordered_map<Path, ID3D11ShaderResourceView*> mTextures;

		std::shared_mutex mTexturesMutex;

		fq::game_module::EventHandler mOnLoadSceneHandler;
	};
}