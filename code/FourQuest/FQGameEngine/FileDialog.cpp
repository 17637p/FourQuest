#include "FileDialog.h"

#include <imgui.h>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <regex>

#include "imgui_stdlib.h"
#include "../FQReflect/FQReflect.h"
#include "../FQCommon/FQPath.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGraphics/IFQGraphics.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "WindowSystem.h"
#include "ModelSystem.h"
#include "ImageSystem.h"

namespace fs = std::filesystem;

fq::game_engine::FileDialog::FileDialog()
	:mResourcePath{}
	, mSelectPath{}
	, mDragDropPath{}
	, mbIsOpen{ true }
	, mDevice(nullptr)
	, mIconSize{ 100.f,100.f }
	, mEditorProcess(nullptr)
	, mGameProcess(nullptr)
	, mbIsFindAllDirectory(false)
	, mImageSystem(nullptr)
{}

fq::game_engine::FileDialog::~FileDialog()
{
}

void fq::game_engine::FileDialog::Render()
{
	if (!mbIsOpen) return;

	ProcessWindowDropFile();

	if (ImGui::Begin("FileDialog", &mbIsOpen))
	{
		beginWindow_FilePathWindow();

		beginWindow_FileList();
	}
	ImGui::End();
}

void fq::game_engine::FileDialog::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mResourcePath = fq::path::GetResourcePath();
	mSelectPath = mResourcePath;
	mDevice = mGameProcess->mGraphics->GetDivice();
	mImageSystem = mEditorProcess->mImageSystem.get();

	clearGarbage();
}

void fq::game_engine::FileDialog::Finalize()
{
}

void fq::game_engine::FileDialog::beginWindow_FilePathWindow()
{
	if (ImGui::BeginChild("FilePathWinodw", ImVec2(200, 0), ImGuiChildFlags_Border | ImGuiChildFlags_ResizeX))
	{
		beginFavorite();
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
		selectPath(path);
		mbIsFindAllDirectory = false;
	}

	beginDragDrop_Directory(path);

	if (bIsSelect)
	{
		ImGui::PopStyleColor();
	}

	auto directoryList = fq::path::GetFileList(path);

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

		// 현재 선택경로가 자식경로인 경우 트리노드를 펼칩니다
		if (mSelectPath.parent_path() == path)
			ImGui::SetNextItemOpen(true);

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
		setIconSize();
		beginPopupContextWindow_FileList();

		// 현재 폴더 경로 
		ImGui::Text(mSelectPath.string().c_str());
		ImGui::Separator();

		// ignore 파일 제거
		std::vector<std::filesystem::path> directoryList;

		if (mbIsFindAllDirectory)
		{
			directoryList = fq::path::GetFileListRecursive(mResourcePath);

			directoryList.erase(std::remove_if(directoryList.begin(), directoryList.end(),
				[](const std::filesystem::path& path)
				{
					std::string pathString = path.string();
					// Check if the path contains ".svn" or "internal"
					if (pathString.find(".svn") == std::string::npos && pathString.find("internal") == std::string::npos)
					{
						return false;
					}

					return true;
				}), directoryList.end());

			directoryList.erase(std::remove_if(directoryList.begin(), directoryList.end(),
				[this](const auto path)
				{
					for (auto& extension : mQueryExtension)
					{
						if (path.extension() == extension)
						{
							return false;
						}
					}
					return true;
				}), directoryList.end());
		}
		else // 폴더 
		{
			directoryList = fq::path::GetFileList(mSelectPath.string().c_str());
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
		}

		float fileSpaceX = mIconSize.x * 1.25f;
		float fileSpaceY = mIconSize.x * 1.25f + ImGui::GetFontSize();

		int maxWidth = std::max(static_cast<int>(ImGui::GetWindowWidth() / fileSpaceX), 1);

		float startCusorPosY = ImGui::GetCursorPosY();

		// 현재 폴더 경로의 파일들을 그립니다
		for (int i = 0; i < directoryList.size(); ++i)
		{
			if (directoryList[i].filename() == ".svn")
			{
				continue;
			}

			ImVec2 cursorPos = { fileSpaceX * (i % maxWidth) , startCusorPosY + fileSpaceY * (i / maxWidth) };
			ImGui::SetCursorPos(cursorPos);
			drawFile(directoryList[i]);
		}
	}
	ImGui::EndChild();

	beginDragDropTarget_FileList();
}


void fq::game_engine::FileDialog::drawFile(const Path& path)
{
	auto extension = path.extension();
	auto cursorPos = ImGui::GetCursorPos();

	if (fs::is_directory(path))
	{
		ImVec2 min = ImGui::GetCursorScreenPos();
		ImVec2 max = { min.x + mIconSize.x, min.y + mIconSize.y };
		ImGui::Image(mImageSystem->GetIcon(L"folder.png"), mIconSize);

		if (isMouseHoveringRect(min, max)
			&& ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			selectPath(path);
		}
	}
	else if (extension == ".fbx")
	{
		ImGui::Image(mImageSystem->GetIcon(L"fbx.png"), mIconSize);
	}
	else if (extension == ".json")
	{
		ImGui::Image(mImageSystem->GetIcon(L"json.png"), mIconSize);
	}
	else if (extension == ".mp3")
	{
		ImGui::Image(mImageSystem->GetIcon(L"mp3.png"), mIconSize);
	}
	else if (extension == ".wav")
	{
		ImGui::Image(mImageSystem->GetIcon(L"wav.png"), mIconSize);
	}
	else if (extension == ".prefab")
	{
		ImGui::Image(mImageSystem->GetIcon(L"prefab.png"), mIconSize);
	}
	else if (extension == ".png" || extension == ".jpg")
	{
		mImageSystem->DrawTextureImage(path, mIconSize);
	}
	else if (extension == ".model")
	{
		ImGui::Image(mImageSystem->GetIcon(L"model.png"), mIconSize);
	}
	else if (extension == ".controller")
	{
		ImGui::Image(mImageSystem->GetIcon(L"controller.png"), mIconSize);
	}
	else
	{
		ImGui::Image(mImageSystem->GetIcon(L"error.png"), mIconSize);
	}

	beginPopupContextItem_File(path);
	beginDragDrop_File(path);

	ImGui::SetCursorPos({ cursorPos.x, cursorPos.y + mIconSize.y });

	ImGui::PushItemWidth(mIconSize.x);

	// 파일 이름 
	std::string fileName = path.filename().string();
	std::string textName = "##" + path.string();

	if (ImGui::InputText(textName.c_str(), &fileName)
		&& mEditorProcess->mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
	{
		fs::path newFileName = path.parent_path();
		newFileName /= fileName;

		changeDirectoryPath(path, newFileName);
	}

	ImGui::PopItemWidth();
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

				changeDirectoryPath(*dropPath, newPath);
			}
		}
		ImGui::EndDragDropTarget();
	}

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

		if (ImGui::MenuItem("Create Controller"))
		{
			auto controllerPath = mSelectPath;
			controllerPath /= "NewController.controller";

			fq::game_module::AnimatorController controller;
			fq::game_module::AnimatorControllerLoader loader;

			int index = 0;
			while (fs::exists(controllerPath))
			{
				++index;
				Path newFileName = "NewController";
				newFileName += std::to_string(index) + ".controller";
				controllerPath.replace_filename(newFileName);
			}

			loader.Save(controller, controllerPath);
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

			mGameProcess->mPrefabManager->SavePrefab(dropObject, mSelectPath);
		}

		ImGui::EndDragDropTarget();
	}
}

void fq::game_engine::FileDialog::selectPath(Path path)
{
	mSelectPath = path;
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

			changeDirectoryPath(*dropPath, newPath);
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

		if (path.extension() == ".fbx")
		{
			if (ImGui::MenuItem("Convert"))
			{
				std::wstring fileName = path.filename();
				fileName = fileName.substr(0, fileName.size() - 4);

				fs::path directory = path.parent_path() / fileName;

				auto modelData = mGameProcess->mGraphics->ConvertModel(path.string());
				mGameProcess->mGraphics->WriteModel(directory.string(), modelData);
			}
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

void fq::game_engine::FileDialog::ProcessWindowDropFile()
{
	auto& dropFiles = WindowSystem::DropFiles;

	if (dropFiles.empty())
	{
		return;
	}

	// 드랍된 파일들을 리소스폴더에 복사합니다
	for (const auto& filePath : dropFiles)
	{
		fs::path target = mSelectPath;
		target /= fs::path(filePath).filename();
		// 중복 파일과 폴더 경로는 무시합니다 

		fs::copy(filePath, target, fs::copy_options::recursive
			| fs::copy_options::overwrite_existing);

		if (target.extension() == ".fbx")
		{
			std::wstring fileName = target.filename();
			fileName = fileName.substr(0, fileName.size() - 4);

			fs::path directory = target.parent_path() / fileName;
			auto modelData = mGameProcess->mGraphics->ConvertModel(target.string());
			mGameProcess->mGraphics->WriteModel(directory.string(), modelData);
		}
	}

	dropFiles.clear();
}

void fq::game_engine::FileDialog::clearGarbage()
{
	fq::path::ClearDirectory(fq::path::GetGarbagePath());
}

void fq::game_engine::FileDialog::setIconSize()
{
	ImVec2 min = ImGui::GetWindowPos();
	ImVec2 max = ImGui::GetWindowSize();
	max.x += min.x;
	max.y += min.y;

	auto& input = mGameProcess->mInputManager;

	if (ImGui::IsMouseHoveringRect(min, max)
		&& input->IsKeyState(EKey::Ctrl, EKeyState::Hold))
	{
		auto deltaWheel = input->GetDeltaMouseWheel();

		if (deltaWheel > 0)
		{
			constexpr float maxSize = 150.f;
			mIconSize.x = std::min(mIconSize.x + 2.f, maxSize);
			mIconSize.y = std::min(mIconSize.y + 2.f, maxSize);
		}
		else if (deltaWheel < 0)
		{
			constexpr float minSize = 50.f;
			mIconSize.x = std::max(mIconSize.x - 2.f, minSize);
			mIconSize.y = std::max(mIconSize.y - 2.f, minSize);
		}

	}
}

void fq::game_engine::FileDialog::beginFavorite()
{
	if (ImGui::TreeNode("Favorite"))
	{
		if (ImGui::Button("All Model"))
		{
			mbIsFindAllDirectory = true;
			mQueryExtension.clear();
			mQueryExtension.push_back(".model");
		}
		if (ImGui::Button("All Texture"))
		{
			mbIsFindAllDirectory = true;
			mQueryExtension.clear();
			mQueryExtension.push_back(".png");
			mQueryExtension.push_back(".jpg");
			mQueryExtension.push_back(".dds");
		}
		if (ImGui::Button("All Prefab"))
		{
			mbIsFindAllDirectory = true;
			mQueryExtension.clear();
			mQueryExtension.push_back(".prefab");
		}

		ImGui::TreePop();
	}

}

void fq::game_engine::FileDialog::changeDirectoryPath(Path prev, Path current)
{
	mEditorProcess->mMainMenuBar->SaveScene();

	fs::rename(prev, current);

	// 리소스 경로의 prefab을 가져옵니다
	auto fileList = fq::path::GetFileListRecursive(fq::path::GetResourcePath());

	fileList.erase(std::remove_if(fileList.begin(), fileList.end(),
		[](Path path)
		{
			if (path.extension() == ".prefab"
				|| path.extension() == ".controller")
			{
				return false;
			}
			return true;

		}), fileList.end());

	auto prevRelativePath = fq::path::GetRelativePath(prev).string();
	auto currntRelativePath = fq::path::GetRelativePath(current).string();

	// 문자열 변환
	size_t pos = 0;

	while ((pos = prevRelativePath.find("\\", pos)) != std::string::npos)
	{
		prevRelativePath.replace(pos, std::string("\\").length(), "\\\\");
		pos += std::string("\\\\").length();
	}
	pos = 0;

	while ((pos = currntRelativePath.find("\\", pos)) != std::string::npos)
	{
		currntRelativePath.replace(pos, std::string("\\").length(), "\\\\");
		pos += std::string("\\\\").length();
	}

	// 파일 쓰기
	for (auto& file : fileList)
	{
		// 파일 읽기
		std::ifstream inFille(file);

		std::stringstream buffer;
		buffer << inFille.rdbuf();
		std::string fileContent = buffer.str();

		inFille.close();

		// 문자열 변환
		size_t pos = 0;

		while ((pos = fileContent.find(prevRelativePath, pos)) != std::string::npos)
		{
			fileContent.replace(pos, prevRelativePath.length(), currntRelativePath);
			pos += currntRelativePath.length();
		}

		// 파일 쓰기
		std::ofstream outFile(file);
		outFile << fileContent;
		outFile.close();
	}

	auto currentSceneName = mGameProcess->mSceneManager->GetCurrentScene()->GetSceneName();
	mGameProcess->mEventManager->FireEvent<fq::event::RequestChangeScene>({ currentSceneName, false });
}

