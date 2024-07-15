#include "LightProbeWindow.h"

#include "LightProbeSystem.h"
#include "GameProcess.h"
#include "EditorProcess.h"

#include <imgui.h>
#include "imgui_stdlib.h"

#include <cctype> 

#include "../FQGameModule/InputManager.h"
#include "../FQCommon/IFQRenderObject.h"
#include "../FQGraphics/IFQGraphics.h"

#include "PathFindingSystem.h"

fq::game_engine::LightProbeWindow::LightProbeWindow()
	:mbIsOpen(false),
	mSaveFileName("LightProbe"),
	mWidth(256),
	mHeight(256),
	mScale(1),
	mIsDrawDebugLightProbe(true),
	mIntensity(1)
{
}

fq::game_engine::LightProbeWindow::~LightProbeWindow()
{
}

void fq::game_engine::LightProbeWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
	mLightProbeSystem = game->mLightProbeSystem.get();
	mInputManager = editor->mInputManager.get();
}

void fq::game_engine::LightProbeWindow::Render()
{
	if (!mbIsOpen) return;

	if (ImGui::Begin("LightProbe", &mbIsOpen))
	{
		beginButtons();
		beginProbeList();
		CheckCopyPaste();
		CancelSelect();
	}
	ImGui::End();
}

bool isNumber(const std::string& str)
{
	return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void fq::game_engine::LightProbeWindow::beginButtons()
{
	if (ImGui::Button("Add", ImVec2{ 133,25 }))
	{
		mLightProbeSystem->AddLightProbe({ 0, 0, 0 });
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete", ImVec2{ 133,25 }))
	{
		if (mSelectObject != nullptr)
		{
			mLightProbeSystem->DeleteLightProbe(mSelectObject);
			mSelectObject = nullptr;
			if (mSelectObject == mCopyObject)
			{
				mCopyObject = nullptr;
			}
		}
	}
	ImGui::Separator();
	{
		std::string widthStr = std::to_string(mWidth);
		std::string heightStr = std::to_string(mHeight);

		ImGui::InputText("width", &widthStr);
		ImGui::InputText("height", &heightStr);

		if (isNumber(widthStr))
		{
			mWidth = stoi(widthStr);
		}
		if (isNumber(heightStr))
		{
			mHeight = stoi(heightStr);
		}
	}

	if (ImGui::Button("Bake", ImVec2{ 133,25 }))
	{
		int numLightProbe = mLightProbeSystem->GetLightProbeObjects().size();
		if (numLightProbe >= 4)
		{
			mLightProbeSystem->SaveProbeTexture(mWidth, mHeight);
			mLightProbeSystem->BakeLightProbe();
		}
		else
		{
			spdlog::error("Baking is only possible with four or more");
		}
	}
	// Nav Mesh 
	if (ImGui::Button("Place Default", ImVec2{ 133,25 }))
	{
		std::vector<DirectX::SimpleMath::Vector3> navMeshVertices = mGameProcess->mPathFindgingSystem->GetNavMeshVertices();
		std::set<DirectX::SimpleMath::Vector3> navMeshVerticesOptimize(navMeshVertices.begin(), navMeshVertices.end());
		for (const auto& vertex : navMeshVerticesOptimize)
		{
			mLightProbeSystem->AddLightProbe(vertex);
		}
	}
	ImGui::Separator();
	ImGui::InputText(".Ltp", &mSaveFileName);
	if (ImGui::Button("Save", ImVec2{ 133,25 }))
	{
		mLightProbeSystem->SaveLightProbes(mSaveFileName);
	}
	if (ImGui::Button("Load", ImVec2{ 133,25 }))
	{
		mSelectObject = nullptr;
		mCopyObject = nullptr;
		mLightProbeSystem->LoadLightProbes(mSaveFileName);
	}
	ImGui::Separator();
	std::string intensityStr = std::to_string(mIntensity);
	if (ImGui::InputText("Intensity", &intensityStr)
		&& mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
	{
		try
		{
			mIntensity = std::stof(intensityStr);
			mGameProcess->mGraphics->SetLightProbeIntensity(mIntensity);
			//mLightProbeSystem->SetLightProbeScale(mIntensity);
		}
		catch (const std::invalid_argument& e)
		{

		}
	}
	ImGui::Separator();
	if (ImGui::Checkbox("Draw Debug", &mIsDrawDebugLightProbe))
	{
		mGameProcess->mGraphics->SetIsDrawDebugLightProbe(mIsDrawDebugLightProbe);
	}

	std::string scaleStr = std::to_string(mScale);

	if (ImGui::InputText("LightProbe Scale", &scaleStr)
		&& mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
	{
		try
		{
			mScale = std::stof(scaleStr);
			mLightProbeSystem->SetLightProbeScale(mScale);

			std::vector<fq::graphics::IProbeObject*> probeObjects = mLightProbeSystem->GetLightProbeObjects();
			for (int i = 0; i < probeObjects.size(); i++)
			{
				DirectX::SimpleMath::Matrix transform = probeObjects[i]->GetTransform();

				DirectX::SimpleMath::Vector3 pos;
				DirectX::SimpleMath::Quaternion rot;
				DirectX::SimpleMath::Vector3 scl;

				transform.Decompose(scl, rot, pos);

				scl = {
					mScale * mLightProbeSystem->GetLightProbeDefaultScale(),
					mScale * mLightProbeSystem->GetLightProbeDefaultScale(),
					mScale * mLightProbeSystem->GetLightProbeDefaultScale() };

				probeObjects[i]->SetTransform(DirectX::SimpleMath::Matrix::CreateScale(scl)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rot)
					* DirectX::SimpleMath::Matrix::CreateTranslation(pos));
			}
		}
		catch (const std::invalid_argument& e)
		{

		}
	}
	ImGui::Separator();
	if (ImGui::Button("Apply All Mesh", ImVec2{ 133,25 }))
	{
		// staticmeshrenderer 가져와서 
		auto componentView = mGameProcess->mSceneManager->GetCurrentScene()->GetComponentView<fq::game_module::StaticMeshRenderer>();

		float distMin = FLT_MAX;

		for (auto& staticMeshRenderer : componentView)
		{
			auto meshInfo = staticMeshRenderer.GetComponent<fq::game_module::StaticMeshRenderer>()->GetMeshObjectInfomation();
			meshInfo.bUseLightProbe = true;
			staticMeshRenderer.GetComponent<fq::game_module::StaticMeshRenderer>()->SetMeshObjectInfomation(meshInfo);
		}
	}
	if (ImGui::Button("Remove All Mesh", ImVec2{ 133,25 }))
	{
		// staticmeshrenderer 가져와서 
		auto componentView = mGameProcess->mSceneManager->GetCurrentScene()->GetComponentView<fq::game_module::StaticMeshRenderer>();

		float distMin = FLT_MAX;

		for (auto& staticMeshRenderer : componentView)
		{
			auto meshInfo = staticMeshRenderer.GetComponent<fq::game_module::StaticMeshRenderer>()->GetMeshObjectInfomation();
			meshInfo.bUseLightProbe = false;
			staticMeshRenderer.GetComponent<fq::game_module::StaticMeshRenderer>()->SetMeshObjectInfomation(meshInfo);
		}
	}
	ImGui::Separator();
}

void fq::game_engine::LightProbeWindow::beginProbeList()
{
	std::vector<fq::graphics::IProbeObject*> probeObjects = mLightProbeSystem->GetLightProbeObjects();
	for (int i = 0; i < probeObjects.size(); i++)
	{
		// 선택버튼
		std::string buttonName = " ##buttonName" + std::to_string(probeObjects[i]->GetIndex());

		bool isSelected = mSelectObject != nullptr
			&& mSelectObject->GetIndex() == probeObjects[i]->GetIndex();

		if (isSelected)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 1.0f, 0.f, 1.f));
			DirectX::SimpleMath::Matrix transform = mSelectObject->GetTransform();
			mEditorProcess->mGamePlayWindow->DrawGizumo(transform);

			DirectX::SimpleMath::Vector3 pos;
			DirectX::SimpleMath::Quaternion rot;
			DirectX::SimpleMath::Vector3 scl;

			transform.Decompose(scl, rot, pos);

			mSelectObject->SetTransform(DirectX::SimpleMath::Matrix::CreateScale(scl) * DirectX::SimpleMath::Matrix::CreateTranslation(pos));
			mGameProcess->mGraphics->SetLightProbe(mSelectObject->GetIndex(), pos);
		}

		if (ImGui::Button(buttonName.c_str()))
		{
			if (mSelectObject != probeObjects[i])
			{
				mSelectObject = probeObjects[i];
				mGameProcess->mEventManager->FireEvent<editor_event::SelectObject>({ nullptr });

				DirectX::SimpleMath::Matrix transform = probeObjects[i]->GetTransform();
				DirectX::SimpleMath::Vector3 pos = { transform._41, transform._42, transform._43 };
				mEditorProcess->mGamePlayWindow->LookAtTarget(pos);
			}
			else
			{
				mSelectObject = nullptr;
			}
			//mSelectObject = probeObjects[i];
			//if (mSelectObject == probeObjects[i])
			//{
			//	DirectX::SimpleMath::Matrix transform = probeObjects[i]->GetTransform();
			//	DirectX::SimpleMath::Vector3 pos = { transform._41, transform._42, transform._43 };
			//	mEditorProcess->mGamePlayWindow->LookAtTarget(pos);
			//}

			//mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
			//		mGameProcess->mEventManager.get(), object.shared_from_this(), mSelectObject
			//	});

		}

		if (isSelected)
		{
			ImGui::PopStyleColor();
		}

		ImGui::SameLine();

		// Name Bar
		std::string id = "LightProbe Index";
		std::string index = std::to_string(probeObjects[i]->GetIndex());

		ImGui::SetNextItemWidth(150.f);

		if (ImGui::InputText(id.c_str(), &index)
			&& mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
		{
			//std::shared_ptr<fq::game_module::GameObject> sObject
			//	= object.shared_from_this();

			//// 이름이 수정된 경우 Set 명령어 전달합니다
			//// 수명관리를 위해서 shared_ptr로 람다에 전달합니다
			//mEditorProcess->mCommandSystem->Push<SetValueCommand<std::string>>(
			//	SetValueCommand<std::string>{ [sObject](std::string name)
			//	{
			//		sObject->SetName(name);
			//	}, sObject->GetName(), objectName }
			//);
		}
	}
}

void fq::game_engine::LightProbeWindow::PickObject(void* iProbeObject)
{
	std::vector<fq::graphics::IProbeObject*> probeObjects = mLightProbeSystem->GetLightProbeObjects();

	auto probeObject = std::find(probeObjects.begin(), probeObjects.end(), iProbeObject);
	if (probeObject != probeObjects.end())
	{
		mSelectObject = *probeObject;
		mGameProcess->mEventManager->FireEvent<editor_event::SelectObject>({ nullptr });
	}
}

void fq::game_engine::LightProbeWindow::CheckCopyPaste()
{
	if (mInputManager->IsKeyState(EKey::Ctrl, EKeyState::Hold))
	{
		if (mInputManager->IsKeyState(EKey::C, EKeyState::Tap))
		{
			if (mSelectObject != nullptr)
			{
				mCopyObject = mSelectObject;
			}
		}
	}

	if (mInputManager->IsKeyState(EKey::Ctrl, EKeyState::Hold))
	{
		if (mInputManager->IsKeyState(EKey::V, EKeyState::Tap))
		{
			if (mCopyObject != nullptr)
			{
				DirectX::SimpleMath::Matrix transform = mCopyObject->GetTransform();
				mLightProbeSystem->AddLightProbe({ transform._41, transform._42, transform._43 });
			}
		}
	}
}

void fq::game_engine::LightProbeWindow::CancelSelect()
{
	if (mInputManager->IsKeyState(EKey::ESC, EKeyState::Tap))
	{
		mSelectObject = nullptr;
	}
}
