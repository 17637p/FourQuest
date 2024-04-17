#include "FileDialog.h"

#include <imgui.h>
#include <string>
#include <algorithm>
#include <directxtk/WICTextureLoader.h>

#include "imgui_stdlib.h"
#include "../FQGameModule/GameModule.h"
#include "../FQReflect/FQReflect.h"
#include "../FQCommon/FQPath.h"
#include "GameProcess.h"
#include "EditorProcess.h"

namespace fs = std::filesystem;

fq::game_engine::FileDialog::FileDialog()
	:mResourcePath{}
	, mSelectPath{}
	, mDragDropPath{}
	, mbIsOpen{ true }
	, mTextures{}
	, mIconTexture{}
	, mDevice(nullptr)
	, mIconSize{ 100.f,100.f }
	, mEditorProcess(nullptr)
	, mGameProcess(nullptr)
{}

fq::game_engine::FileDialog::~FileDialog()
{
}

void fq::game_engine::FileDialog::Render()
{
	if (ImGui::Begin("FileDialog"), &mbIsOpen)
	{
		beginWindow_FilePathWindow();

		beginWindow_FileList();
	}
	ImGui::End();
}

void fq::game_engine::FileDialog::Initialize(GameProcess* game, EditorProcess* editor, ID3D11Device* device)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mResourcePath = fq::path::GetResourcePath();
	mSelectPath = mResourcePath;
	mDevice = device;

	loadIcon();
}

void fq::game_engine::FileDialog::Finalize()
{
	clearTexture();
	clearIconTexture();
}

void fq::game_engine::FileDialog::beginWindow_FilePathWindow()
{
	if (ImGui::BeginChild("FilePathWinodw", ImVec2(200, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX))
	{
		beginDirectory(mResourcePath);
	}
	ImGui::EndChild();

	ImGui::SameLine();
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
		SelectPath(path);
	}

	beginDragDrop_Directory(path);

	if (bIsSelect)
	{
		ImGui::PopStyleColor();
	}

	auto directoryList = fq::path::GetDirectoryList(path);

	// 디렉토리가 아닌경우 제거 
	directoryList.erase(std::remove_if(directoryList.begin(), directoryList.end(),
		[](const auto path)
		{
			if (fs::is_directory(path))
			{
				return false;
			}
			return true;
		}), directoryList.end());

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
				// ignore
				if (directory.filename() == ".svn"
					|| directory.filename() == "internal")
				{
					continue;
				}

				if (fs::is_directory(directory))
				{
					beginDirectory(directory);
				}
			}

			ImGui::TreePop();
		}
	}
}

void fq::game_engine::FileDialog::beginWindow_FileList()
{
	if (ImGui::BeginChild("FileListWindow"
		, ImVec2(0, -ImGui::GetFrameHeightWithSpacing())))
	{
		beginPopupContextWindow_FileList();

		// 현재 폴더 경로 
		ImGui::Text(mSelectPath.string().c_str());
		ImGui::Separator();

		// ignore 파일 제거
		auto directoryList = fq::path::GetDirectoryList(mSelectPath.string().c_str());
		directoryList.erase(std::remove_if(directoryList.begin(), directoryList.end(),
			[](const auto path)
			{
				if (path.filename() == ".svn" || path.filename() == "internal")
				{
					return true;
				}
				return false;
			}), directoryList.end());

		// 디렉토리가 파일보다 우선순위가 높음
		std::sort(directoryList.begin(), directoryList.end()
			, [](const Path& lfm, const Path& rfm)
			{
				if (fs::is_directory(lfm) && !fs::is_directory(rfm))
				{
					return true;
				}

				return false;
			});


		float fileSpace = mIconSize.x + 50.f;

		int maxWidth = max(static_cast<int>(ImGui::GetWindowWidth() / fileSpace), 1);

		float startCusorPosY = ImGui::GetCursorPosY();

		// 현재 폴더 경로의 파일들을 그립니다
		for (int i = 0; i < directoryList.size(); ++i)
		{
			if (directoryList[i].filename() == ".svn")
			{
				continue;
			}

			ImVec2 cursorPos = { fileSpace * (i % maxWidth) , startCusorPosY + fileSpace * (i / maxWidth) };
			ImGui::SetCursorPos(cursorPos);
			drawFile(directoryList[i]);
		}
	}
	ImGui::EndChild();

	beginDragDropTarget_FileList();
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

void fq::game_engine::FileDialog::drawFile(const Path& path)
{
	auto extension = path.extension();
	auto cursorPos = ImGui::GetCursorPos();

	if (fs::is_directory(path))
	{
		ImVec2 min = ImGui::GetCursorScreenPos();
		ImVec2 max = { min.x + mIconSize.x, min.y + mIconSize.y };
		ImGui::Image(getIcon(L"folder.png"), mIconSize);

		if (isMouseHoveringRect(min, max)
			&& ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			SelectPath(path);
		}

	}
	else if (extension == ".fbx")
	{
		ImGui::Image(getIcon(L"fbx.png"), mIconSize);
	}
	else if (extension == ".json")
	{
		ImGui::Image(getIcon(L"json.png"), mIconSize);
	}
	else if (extension == ".mp3")
	{
		ImGui::Image(getIcon(L"mp3.png"), mIconSize);
	}
	else if (extension == ".wav")
	{
		ImGui::Image(getIcon(L"wav.png"), mIconSize);
	}
	else if (extension == ".png" || extension == ".jpg")
	{
		drawTextureImage(path);
	}
	else
	{
		ImGui::Image(getIcon(L"error.png"), mIconSize);
	}

	beginPopupContextItem_File(path);
	beginDragDrop_File(path);

	ImGui::SetCursorPos({ cursorPos.x, cursorPos.y + mIconSize.y });

	ImGui::PushItemWidth(100);

	// 파일 이름 
	std::string fileName = path.filename().string();
	std::string textName = "##" + fileName;

	if (ImGui::InputText(textName.c_str(), &fileName)
		&& mEditorProcess->mInputManager->IsKeyState(Key::Enter, KeyState::Tap))
	{
		fs::path newFileName = path.parent_path();
		newFileName += "//" + fileName;

		fs::rename(path, newFileName);
	}

	ImGui::PopItemWidth();
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

void fq::game_engine::FileDialog::beginDragDrop_File(const Path& path)
{
	// Drag 처리
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		mDragDropPath = path;

		ImGui::Text(mDragDropPath.string().c_str());

		ImGui::SetDragDropPayload("Path", &mDragDropPath
			, sizeof(Path));
		ImGui::EndDragDropSource();
	}

	// Drop 처리
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* dropPath
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			// dropPath 파일을 path 안으로 넣는다.
			if (fs::is_directory(path))
			{
				// 드랍된 파일의 경로를 새로운 경로로 이동시킵니다.
				fs::path newPath = path / dropPath->filename();
				fs::rename(*dropPath, newPath);
			}
		}
		ImGui::EndDragDropTarget();
	}

}

void fq::game_engine::FileDialog::drawTextureImage(const Path& path)
{
	auto iter = mTextures.find(path);

	ID3D11ShaderResourceView* texture = nullptr;

	if (iter == mTextures.end())
	{
		texture = loadTexture(path);
		mTextures.insert({ path, texture });
	}
	else
	{
		texture = iter->second;
	}
	ImGui::Image(texture, mIconSize);
}

void fq::game_engine::FileDialog::clearTexture()
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


void fq::game_engine::FileDialog::clearIconTexture()
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

bool fq::game_engine::FileDialog::isMouseHoveringRect(const ImVec2& min, const ImVec2& max)
{
	const ImVec2 mouse_pos = ImGui::GetMousePos();
	return mouse_pos.x >= min.x && mouse_pos.y >= min.y && mouse_pos.x <= max.x && mouse_pos.y <= max.y;
}

void fq::game_engine::FileDialog::beginPopupContextWindow_FileList()
{
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create Directory"))
		{
			auto directory = mSelectPath;

			directory += "\\dir";

			while (!fs::create_directory(directory))
			{
				directory += "!";
			}
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::FileDialog::beginDragDropTarget_FileList()
{
	if (ImGui::BeginDragDropTarget())
	{
		// GameObject 저장
		const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload("GameObject");

		if (payLoad)
		{
			fq::game_module::GameObject* dropObject
				= static_cast<fq::game_module::GameObject*>(payLoad->Data);

			mGameProcess->mObjectManager->SavePrefab(dropObject, mSelectPath);
		}

		ImGui::EndDragDropTarget();
	}
}

void fq::game_engine::FileDialog::SelectPath(Path path)
{
	mSelectPath = path;
	clearTexture();
}

void fq::game_engine::FileDialog::beginDragDrop_Directory(const Path& directoryPath)
{
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* dropPath
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			// dropPath 파일을 path 안으로 넣는다.
			// 드랍된 파일의 경로를 새로운 경로로 이동시킵니다.
			fs::path newPath = directoryPath / dropPath->filename();
			fs::rename(*dropPath, newPath);
		}
		ImGui::EndDragDropTarget();
	}
}

void fq::game_engine::FileDialog::beginPopupContextItem_File(const Path& path)
{

	std::string contextName = "context##" + path.string();

	if (ImGui::BeginPopupContextItem(contextName.c_str()))
	{
		if (ImGui::MenuItem("Delete"))
		{
			fs::path garbagePath = fq::path::GetGarbagePath() / path.filename();

			// 쓰레기통에 같은 파일이 있으면 삭제합니다
			if (fs::exists(garbagePath))
			{
				fs::remove_all(garbagePath);
			}	

			fs::rename(path, garbagePath);
		}

		if (ImGui::MenuItem("Create Directory"))
		{
			auto directory = mSelectPath;

			directory += "\\dir";

			while (!fs::create_directory(directory))
			{
				directory += "!";
			}
		}

		ImGui::EndPopup();
	}
}

