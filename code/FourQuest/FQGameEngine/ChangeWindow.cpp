#include "ChangeWindow.h"

#include "GameProcess.h"
#include "EditorProcess.h"
#include "../FQGameModule/GameModule.h"
#include "../FQGameModule/Transform.h"
#include "imgui_stdlib.h"

void fq::game_engine::ChangeWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
}

void fq::game_engine::ChangeWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("ChangeWindow", &mbIsOpen))
	{
		beginPrefabWindow();
		beginSearchWindow();
		beginChangeWindow();
	}
	ImGui::End();
}

std::vector<std::shared_ptr<fq::game_module::GameObject>> fq::game_engine::ChangeWindow::loadClonePrefabResource()
{
	return mGameProcess->mPrefabManager->LoadPrefab(mPrefabPath.GetPrefabPath());
}

void fq::game_engine::ChangeWindow::beginPrefabWindow()
{
	if (ImGui::BeginChild("Prefab", ImVec2(1000, 100), ImGuiChildFlags_Border))
	{
		std::string path = mPrefabPath.GetPrefabPath();
		ImGui::InputText("PrefabPath", &path);

		if (ImGui::BeginDragDropTarget())
		{
			const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");
			if (pathPayLoad)
			{
				std::filesystem::path* dropPath
					= static_cast<std::filesystem::path*>(pathPayLoad->Data);

				if (dropPath->extension() == ".prefab")
				{
					mPrefabPath.SetPrefabPath(dropPath->string());
				}
			}
		}

		// Button 
		ImGui::Checkbox("UsePrevObjectScale", &mbUsePrevObjectScale);

		// Button 
		if (ImGui::Button("Change"))
		{
			change();
		}
	}
	ImGui::EndChild();
}


void fq::game_engine::ChangeWindow::beginSearchWindow()
{
	if (ImGui::BeginChild("Search", ImVec2(500, 0), ImGuiChildFlags_Border))
	{
		// SearchBar 
		ImGui::InputText("##Serach", &mSearchName);

		mSearchList.clear();
		auto scene = mGameProcess->mSceneManager->GetCurrentScene();

		for (auto& object : scene->GetObjectView())
		{
			if (object.GetName().find(mSearchName) != std::string::npos)
			{
				mSearchList.push_back(object.shared_from_this());
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Add All"))
		{
			for (auto& object : mSearchList)
			{
				if (mChangeList.find(object->GetID()) == mChangeList.end())
				{
					mChangeList.insert({ object->GetID(), object });
				}
			}
		}

		// SearchList
		for (auto& object : mSearchList)
		{
			bool isInChangeList = mChangeList.find(object->GetID()) != mChangeList.end();

			if (isInChangeList)
			{
				ImGui::PushStyleColor(0, ImVec4{ 0.f,0.f,1.f,1.f });
			}

			std::string plus = "+##" + object->GetName();

			if (ImGui::Button(plus.c_str()))
			{
				if (isInChangeList)
					mChangeList.erase(object->GetID());
				else
					mChangeList.insert({ object->GetID(), object });
			}

			if (isInChangeList)
				ImGui::PopStyleColor();

			ImGui::SameLine();

			ImGui::Text(object->GetName().c_str());
		}
	}

	ImGui::EndChild();
	ImGui::SameLine();
}


void fq::game_engine::ChangeWindow::beginChangeWindow()
{
	if (ImGui::BeginChild("Change", ImVec2(500, 0), ImGuiChildFlags_Border))
	{
		if (ImGui::Button("Reset"))
		{
			mChangeList.clear();
		}

		for (auto iter = mChangeList.begin(); iter != mChangeList.end();)
		{
			bool isErase = false;

			if (ImGui::Button("-"))
			{
				isErase = true;
			}
			ImGui::SameLine();
			ImGui::Text(iter->second->GetName().c_str());

			if (isErase)
			{
				iter = mChangeList.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
	ImGui::EndChild();
}

void fq::game_engine::ChangeWindow::change()
{
	if (mChangeList.empty() || !std::filesystem::exists(mPrefabPath.GetPrefabPath()))
		return;

	auto scene = mGameProcess->mSceneManager->GetCurrentScene();
	for (auto& [id, object] : mChangeList)
	{
		if (object->IsDestroyed()) continue;

		// 1. 씬에서 오브젝트 삭제
		auto parent = object->GetParent();
		scene->DestroyGameObject(object.get());

		// 2. 씬에 클론 오브젝트 추가 
		auto prefabInstance = loadClonePrefabResource();
		auto& rootObject = prefabInstance[0];
		auto rootT = rootObject->GetTransform();

		// position
		auto transform = object->GetTransform();
		auto pos = transform->GetWorldPosition();
		auto scale = mbUsePrevObjectScale ? transform->GetWorldScale()
			: rootT->GetWorldScale();
		auto rotation = transform->GetWorldRotation();

		rootT->GenerateWorld(pos,rotation, scale);

		// 이름 
		rootObject->SetName(object->GetName());
		if (parent)
			rootT->SetParent(parent->GetTransform());

		scene->AddGameObject(rootObject);
	}

	mChangeList.clear();
	spdlog::info("[ChangeWindow] Changed Objects");
}
