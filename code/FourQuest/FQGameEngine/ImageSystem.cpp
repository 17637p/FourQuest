#include "ImageSystem.h"


#include <directxtk/WICTextureLoader.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

#include "../FQCommon/FQPath.h"
#include "../FQGraphics/IFQGraphics.h"
#include "../FQGameModule/ThreadPool.h"
#include "GameProcess.h"

fq::game_engine::ImageSystem::ImageSystem()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mDevice(nullptr)
	, mResourcePath{}
	, mIconTexture{}
	, mTextures{}
{}

fq::game_engine::ImageSystem::~ImageSystem()
{

}

void fq::game_engine::ImageSystem::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mDevice = game->mGraphics->GetDivice();
	mResourcePath = fq::path::GetResourcePath();

	loadIcon();
}

void fq::game_engine::ImageSystem::Finalize()
{
	clearTexture();
	clearIconTexture();
}

void fq::game_engine::ImageSystem::loadIcon()
{
	Path iconPath = mResourcePath;
	iconPath += "\\internal\\icon";

	auto iconList = fq::path::GetFileList(iconPath);

	for (const Path& path : iconList)
	{
		auto* texture = loadTexture(path);
		mIconTexture.insert({ path.filename() ,texture });
	}
}

ID3D11ShaderResourceView* fq::game_engine::ImageSystem::loadTexture(const Path& path)const
{
	ID3D11ShaderResourceView* texture = nullptr;

	DirectX::CreateWICTextureFromFile(mDevice, path.c_str(),
		nullptr, &texture);

	return texture;
}

void fq::game_engine::ImageSystem::clearIconTexture()
{
	for (auto& [path, texture] : mIconTexture)
	{
		if (texture)
		{
			texture->Release();
			texture = nullptr;
		}
	}

	mIconTexture.clear();
}

void fq::game_engine::ImageSystem::clearTexture()
{
	for (auto& [path, texture] : mTextures)
	{
		if (texture)
		{
			texture->Release();
			texture = nullptr;
		}
	}

	mTextures.clear();
}

ID3D11ShaderResourceView* fq::game_engine::ImageSystem::GetIcon(const std::wstring& name)
{
	auto iter = mIconTexture.find(name);

	if (iter == mIconTexture.end())
	{
		return mIconTexture[L"error.png"];
	}

	return iter->second;
}

void fq::game_engine::ImageSystem::DrawTextureImage(const Path& path, ImVec2 size)
{
	auto iter = mTextures.find(path);

	ID3D11ShaderResourceView* texture = nullptr;

	if (iter == mTextures.end())
	{
		std::unique_lock<std::shared_mutex> lock(mTexturesMutex);
		mTextures.insert({ path, texture });

		// ThreadPool 사용해보기
		game_module::ThreadPool::GetInstance()->EnqueueJob([this, path]()
			{
				auto texture = loadTexture(path);

				if (texture)
				{
					std::unique_lock<std::shared_mutex> lock(mTexturesMutex);
					mTextures[path] = texture;
;				}
				else
				{
					spdlog::warn("image load failed -> {}" , path.string());
				}
			});
	}
	else
	{
		texture = iter->second;
	}


	if (texture)
	{
		ImGui::Image(texture, size);
	}
	else
	{
		ImGui::Image(GetIcon(L"load.png"), size);
	}
}
