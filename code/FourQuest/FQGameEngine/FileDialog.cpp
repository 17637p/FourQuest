#include "FileDialog.h"

#include <imgui.h>
#include <string>
#include <iostream>
#include <directxtk/WICTextureLoader.h>

#include "../FQReflect/FQReflect.h"

namespace fs = std::filesystem;

fq::game_engine::FileDialog::FileDialog()
	:mResourcePath{}
	, mSelectPath{}
	, mbIsOpen{ true }
	, mTextures{}
	, mIconTexture{}
	, mDevice(nullptr)
	, mIconSize{}
{}

fq::game_engine::FileDialog::~FileDialog()
{
}

void fq::game_engine::FileDialog::Render()
{
	if (ImGui::Begin("FileDialog"), &mbIsOpen)
	{
		beginLeftChildWindow();

		beginRightChildWindow();

		ImGui::End();
	}
}

void fq::game_engine::FileDialog::beginLeftChildWindow()
{
	if (ImGui::BeginChild("FileSystem", ImVec2(200, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX))
	{
		beginDirectory(mResourcePath);

		ImGui::EndChild();
	}

	ImGui::SameLine();
}

void fq::game_engine::FileDialog::Initialize(ID3D11Device* device)
{
	mResourcePath = fq::path::GetResourcePath();
	mSelectPath = mResourcePath;
	mDevice = device;
	mIconSize = { 100.f,100.f };

	loadIcon();
}

void fq::game_engine::FileDialog::beginDirectory(const Path& path)
{
	std::string fileName = path.filename().string();

	// 다음 버튼 위치 설정
	float cursorPosX = ImGui::GetCursorPosX();
	ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + cursorPosX);

	bool bIsSelect = path == mSelectPath;

	if (bIsSelect)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.0f, 0.5f, 1.f));
	}

	if (ImGui::Button(fileName.c_str()))
	{
		mSelectPath = path;
	}

	if (bIsSelect)
	{
		ImGui::PopStyleColor();
	}


	const auto directoryList = fq::path::GetDirectoryList(path);
	// 자식 디텍토리가 있는경우 
	if (!directoryList.empty())
	{
		std::string treeName = "##" + fileName;
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosX);

		if (ImGui::TreeNode(treeName.c_str()))
		{
			for (const auto& directory : directoryList)
			{
				beginDirectory(directory);
			}

			ImGui::TreePop();
		}

	}

}

void fq::game_engine::FileDialog::beginRightChildWindow()
{
	if (ImGui::BeginChild("directory view"
		, ImVec2(0, -ImGui::GetFrameHeightWithSpacing())))
	{
		// 현재 폴더 경로
		ImGui::Text(mSelectPath.string().c_str());
		ImGui::Separator();

		auto directoryList = fq::path::GetDirectoryList(mSelectPath.string().c_str());

		float windowWidth = ImGui::GetWindowWidth();

	 	float iconInterval =  windowWidth / mIconSize.x;

		for (int i = 0; i < directoryList.size(); ++i)
		{
			ImGui::SetCursorPos({ iconInterval * i ,0 });
			DrawFile(directoryList[i]);
		}


		ImGui::EndChild();
	}
}

ID3D11ShaderResourceView* fq::game_engine::FileDialog::loadTexture(const Path& path)
{
	ID3D11ShaderResourceView* texture = nullptr;

	DirectX::CreateWICTextureFromFile(mDevice, path.c_str(),
		nullptr, &texture);

	return texture;
}

void fq::game_engine::FileDialog::loadIcon()
{
	Path iconPath = mResourcePath;
	iconPath += "\\internal\\icon";

	auto iconList = fq::path::GetDirectoryList(iconPath);

	for (const Path& path : iconList)
	{
		auto* texture = loadTexture(path);
		mIconTexture.insert({ path.filename() ,texture });
	}
}

void fq::game_engine::FileDialog::DrawFile(const Path& path)
{
	auto extension = path.extension();

	auto cursorPos = ImGui::GetCursorPos();


	std::string fileName = path.filename().string();

	ImGui::Text(fileName.c_str());
	
	ImGui::SameLine();

	ImGui::SetCursorPos({ cursorPos.x , cursorPos.y });

	if (fs::is_directory(path))
	{
		ImGui::Image(getIcon(L"folder.png"), mIconSize);
	}
	ImGui::SetCursorPosX(cursorPos.x);
}

ID3D11ShaderResourceView* fq::game_engine::FileDialog::getIcon(const std::wstring& name)
{
	auto iter = mIconTexture.find(name);

	if (iter == mIconTexture.end())
	{
		return mIconTexture[L"error.png"];
	}

	return iter->second;
}

