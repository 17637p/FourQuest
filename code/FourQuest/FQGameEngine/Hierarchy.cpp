#include "Hierarchy.h"

#include <queue>
#include <map>
#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/Transform.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "CommandSystem.h"
#include "Command.h"
#include "ModelSystem.h"
#include "EditorEvent.h"
#include "GamePlayWindow.h"

fq::game_engine::Hierarchy::Hierarchy()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mScene(nullptr)
	, mInputManager(nullptr)
	, mEventManager(nullptr)
	, mSelectObject(nullptr)
	, mCopyObject(nullptr)
	, mSelectObjectHandle{}
	, mbIsOpen(true)
	, mbIsFocused(false)
{}

fq::game_engine::Hierarchy::~Hierarchy()
{}


void fq::game_engine::Hierarchy::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mSelectObject = nullptr;

	mScene = mGameProcess->mSceneManager->GetCurrentScene();
	mEventManager = mGameProcess->mEventManager.get();
	mInputManager = mEditorProcess->mInputManager.get();

	// 이벤트 핸들 등록
	mSelectObjectHandle = mEventManager->RegisterHandle<editor_event::SelectObject>
		([this](editor_event::SelectObject event) 
			{
		mSelectObject = event.object;
			});

	mSelectAnimationState = mEventManager->RegisterHandle<editor_event::SelectAnimationState>
		([this](editor_event::SelectAnimationState event)
			{
				mSelectObject = nullptr;
			});
}

void fq::game_engine::Hierarchy::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("Hierarchy", &mbIsOpen))
	{
		mbIsFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

		beginSearchBar();

		if (ImGui::BeginChild("HierarchyChild"))
		{
			beginPopupContextWindow_HierarchyChild();
			if (mSearchString.empty())
			{
				beginHierarchy();
			}
			else
			{
				beginHierarchyOfSearch();
			}
		}
		ImGui::EndChild();
		dragDropWindow();
	}
	ImGui::End();
}

void fq::game_engine::Hierarchy::beginPopupContextWindow_HierarchyChild()
{
	if (ImGui::BeginPopupContextWindow())
	{
		// GameObject를 생성합니다
		if (ImGui::MenuItem("CreateEmpty"))
		{
			mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene
				, std::make_shared<fq::game_module::GameObject>());
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::Hierarchy::beginSearchBar()
{
	ImGui::SetNextItemWidth(150.f);
	ImGui::InputText("Search", &mSearchString);
	ImGui::Separator();
}

void fq::game_engine::Hierarchy::beginHierarchy()
{
	// GameObject를 순회하면서 Bar를 생성합니다
	for (auto& object : mScene->GetObjectView(true))
	{
		// Root들만 정보를 표시합니다
		if (object.GetParent() != nullptr || object.IsDestroyed())
		{
			continue;
		}

		beginGameObjectBar(object);
	}
}


void fq::game_engine::Hierarchy::beginHierarchyOfSearch()
{
	// GameObject를 순회하면서 Bar를 생성합니다
	for (auto& object : mScene->GetObjectView(true))
	{
		std::string objectName = object.GetName();

		if (objectName.find(mSearchString) == std::string::npos 
			|| object.IsDestroyed())
		{
			continue;
		}

		begineGameObectSelectButton(object);
		beginGameObjectNameBar(object);
	}
}


void fq::game_engine::Hierarchy::beginGameObjectNameBar(fq::game_module::GameObject& object)
{
	std::string id = "##" + std::to_string(object.GetID());
	std::string objectName = object.GetName();

	ImGui::SetNextItemWidth(150.f);

	if (ImGui::InputText(id.c_str(), &objectName)
		&& mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
	{
		std::shared_ptr<fq::game_module::GameObject> sObject
			= object.shared_from_this();

		// 이름이 수정된 경우 Set 명령어 전달합니다
		// 수명관리를 위해서 shared_ptr로 람다에 전달합니다
		mEditorProcess->mCommandSystem->Push<SetValueCommand<std::string>>(
			SetValueCommand<std::string>{ [sObject](std::string name)
			{
				sObject->SetName(name);
			}, sObject->GetName(), objectName }
		);
	}
}

void fq::game_engine::Hierarchy::beginGameObjectBar(fq::game_module::GameObject& object)
{
	auto* objectT = object.GetComponent<fq::game_module::Transform>();
	float cursorPosX = 0.f;

	//  위치 설정 
	if (!objectT->IsLeaf() || objectT->HasParent())
	{
		cursorPosX = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + cursorPosX);
	}

	if (!objectT->HasParent() && objectT->IsLeaf())
	{
		ImGui::SetCursorPosX(ImGui::GetTreeNodeToLabelSpacing() + ImGui::GetCursorPos().x);
	}

	// 선택버튼 관련 처리
	begineGameObectSelectButton(object);
	beginPopupContextItem_GameObject(object);

	// 드래그앤드랍 처리 
	dragDropGameObjectBar(object);

	// 오브젝트 이름
	beginGameObjectNameBar(object);

	// Hierarchy 토글
	auto children = object.GetChildren();

	// 자식계층의 토글을 생성합니다
	if (!children.empty())
	{
		ImGui::SameLine();
		ImGui::SetCursorPosX(cursorPosX);

		std::string treeNodeName = "##Tree" + std::to_string(object.GetID());

		// 선택한 오브젝트가 자식 계층인 경우에서 TreeNode를 펼칩니다
		if (mSelectObject && objectT->IsDescendant(mSelectObject->GetComponent<fq::game_module::Transform>()))
		{
			ImGui::SetNextItemOpen(true);
		}

		if (ImGui::TreeNode(treeNodeName.c_str()))
		{
			for (auto& child : children)
			{
				if (!child->IsDestroyed())
					beginGameObjectBar(*child);
			}

			ImGui::TreePop();
		}
	}
}

void fq::game_engine::Hierarchy::dragDropGameObjectBar(fq::game_module::GameObject& object)
{
	std::string objectName = object.GetName();

	// Drag 처리
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
	{
		ImGui::Text(objectName.c_str());
		ImGui::SetDragDropPayload("GameObject", &object
			, sizeof(fq::game_module::GameObject));
		ImGui::EndDragDropSource();
	}

	// Drop 처리
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* PathPayLoad = ImGui::AcceptDragDropPayload("GameObject");

		if (PathPayLoad)
		{
			fq::game_module::GameObject* dropObject
				= static_cast<fq::game_module::GameObject*>(PathPayLoad->Data);

			auto* parentT = object.GetComponent<fq::game_module::Transform>();
			auto* childT = dropObject->GetComponent<fq::game_module::Transform>();

			if (childT->IsDescendant(parentT))
			{
				ImGui::EndDragDropTarget();
				return;
			}

			auto removeChild = [parentT, childT]()
				{
					parentT->RemoveChild(childT);
				};

			auto addChild = [parentT, childT]()
				{
					parentT->AddChild(childT);
				};

			if (childT->GetParentTransform() == parentT)
			{
				mEditorProcess->mCommandSystem->Push<BindFunctionCommand>
					(removeChild, addChild);
			}
			else
			{
				mEditorProcess->mCommandSystem->Push<BindFunctionCommand>
					(addChild, removeChild);
			}
		}
		ImGui::EndDragDropTarget();
	}

}

void fq::game_engine::Hierarchy::begineGameObectSelectButton(fq::game_module::GameObject& object)
{
	// 선택버튼
	std::string buttonName = " ##buttonName" + std::to_string(object.GetID());

	bool isSelected = mSelectObject != nullptr
		&& mSelectObject->GetID() == object.GetID();

	if (isSelected)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.0f, 0.f, 1.f));
	}

	if (ImGui::Button(buttonName.c_str()))
	{
		if (mSelectObject.get() == &object)
		{
			auto pos = object.GetComponent<fq::game_module::Transform>()->GetWorldPosition();
			mEditorProcess->mGamePlayWindow->LookAtTarget(pos);
		}

		mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
				mGameProcess->mEventManager.get(), object.shared_from_this(), mSelectObject
			});

	}

	if (isSelected)
	{
		ImGui::PopStyleColor();
	}

	ImGui::SameLine();
}

void fq::game_engine::Hierarchy::beginPopupContextItem_GameObject(fq::game_module::GameObject& object)
{
	std::string ContextItemName = object.GetName() + "ContextItem";

	if (ImGui::BeginPopupContextItem(ContextItemName.c_str()))
	{
		// GameObject를 생성합니다
		if (ImGui::MenuItem("Destroy"))
		{
			mEditorProcess->mCommandSystem->Push<DestroyObjectCommand>(mScene
				, object.shared_from_this());
		}

		// GameObject를 생성합니다
		if (ImGui::MenuItem("CreateEmpty"))
		{
			mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene
				, std::make_shared<fq::game_module::GameObject>());
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::Hierarchy::dragDropWindow()
{
	// Drop 처리
	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* pathPayLoad = ImGui::AcceptDragDropPayload("Path");

		if (pathPayLoad)
		{
			std::filesystem::path* path
				= static_cast<std::filesystem::path*>(pathPayLoad->Data);

			// 일단은 Prefab은 json형식입니다
			if (path->extension() == ".prefab")
			{
				auto prefab = mGameProcess->mPrefabManager->LoadPrefab(*path);

				assert(!prefab.empty());

				// 부모를 등록하면 자식계층도 같이 등록합니다
				mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene
					, prefab[0]);
			}
			else if (path->extension() == ".model") // 모델 생성
			{
				mEditorProcess->mModelSystem->BuildModel(*path);
			}
		}

		const ImGuiPayload* objectPayLoad = ImGui::AcceptDragDropPayload("GameObject");

		if (objectPayLoad)
		{
			fq::game_module::GameObject* dropObject
				= static_cast<fq::game_module::GameObject*>(objectPayLoad->Data);

			auto parent = dropObject->GetParent();

			if (parent != nullptr)
			{
				auto childSP = dropObject->shared_from_this();
				auto parentSP = parent->shared_from_this();

				auto excute = [childSP]()
					{
						childSP->GetComponent<fq::game_module::Transform>()->SetParent(nullptr);
					};
				auto undo = [childSP, parentSP]()
					{
						childSP->GetComponent<fq::game_module::Transform>()
							->SetParent(parentSP->GetComponent<fq::game_module::Transform>());
					};

				mEditorProcess->mCommandSystem->Push<BindFunctionCommand>(BindFunctionCommand{ excute,undo });

			}
		}

		ImGui::EndDragDropTarget();
	}
}

void fq::game_engine::Hierarchy::Finalize()
{
	mEventManager->RemoveHandle(mSelectObjectHandle);
}

void fq::game_engine::Hierarchy::ExcuteShortcut()
{
	using namespace fq::game_module;

	const auto& input = mEditorProcess->mInputManager;

	bool isFoused = mbIsFocused || mEditorProcess->mGamePlayWindow->IsFocusedWindow();

	if (input->IsKeyState(EKey::Ctrl, EKeyState::Hold) && isFoused)
	{
		// Ctrl + C (복사)
		if (input->IsKeyState(EKey::C, EKeyState::Tap) && mSelectObject)
		{
			mCopyObject = mSelectObject;
		}
		// Ctrl + V (붙여넣기)
		else if (input->IsKeyState(EKey::V, EKeyState::Tap) && mCopyObject)
		{
			std::shared_ptr<GameObject> root = nullptr;
			std::map<std::string, std::shared_ptr<GameObject>> matchParent;
			std::queue<GameObject*> objectQueue;
			objectQueue.push(mCopyObject.get());

			while (!objectQueue.empty())
			{
				auto orginObject = objectQueue.front();
				objectQueue.pop();
				auto clone = std::make_shared<fq::game_module::GameObject>(*orginObject);
				if (root == nullptr)
				{
					root = clone;
				}
				auto parentT = orginObject->GetComponent<Transform>()->GetParentTransform();
				if (parentT != nullptr)
				{
					auto iter = matchParent.find(parentT->GetGameObject()->GetName());
					if (iter == matchParent.end())
					{
						clone->GetComponent<Transform>()->SetParent(parentT);
					}
					else
					{
						clone->GetComponent<Transform>()->SetParent(iter->second->GetComponent<Transform>());
					}
				}

				matchParent.insert({ clone->GetName(), clone });
				for (auto child : orginObject->GetChildren())
				{
					objectQueue.push(child);
				}
			}

			mEditorProcess->mCommandSystem->Push<AddObjectCommand>(mScene, root);
		}
	}
	else if (mSelectObject && input->IsKeyState(EKey::Del, EKeyState::Tap) 
		&& isFoused)
	{
		// Delete
		mEditorProcess->mCommandSystem->Push<DestroyObjectCommand>(mScene
			, mSelectObject);
	}
}
