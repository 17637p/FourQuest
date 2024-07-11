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

fq::game_engine::LightProbeWindow::LightProbeWindow()
	:mbIsOpen(false),
	mSaveFileName("LightProbe"),
	mWidth(256),
	mHeight(256),
	mScale(1)
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
		mLightProbeSystem->DeleteLightProbe(mSelectObject);
		mSelectObject = nullptr;
	}
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
		mLightProbeSystem->SaveProbeTexture(mWidth, mHeight);
		mLightProbeSystem->BakeLightProbe();
	}
	ImGui::InputText(".Ltp", &mSaveFileName);
	if (ImGui::Button("Save", ImVec2{ 133,25 }))
	{
		mLightProbeSystem->SaveLightProbes(mSaveFileName);
	}
	if (ImGui::Button("Load", ImVec2{ 133,25 }))
	{
		mSelectObject = nullptr;
		mLightProbeSystem->LoadLightProbes(mSaveFileName);
	}

	std::string scaleStr = std::to_string(mScale);

	if (ImGui::InputText("LightProbe Scale", &scaleStr)
		&& mInputManager->IsKeyState(EKey::Enter, EKeyState::Tap))
	{
		try
		{
			mScale = std::stof(scaleStr);

			std::vector<fq::graphics::IProbeObject*> probeObjects = mLightProbeSystem->GetLightProbeObjects();
			for (int i = 0; i < probeObjects.size(); i++)
			{
				DirectX::SimpleMath::Matrix transform = probeObjects[i]->GetTransform();

				DirectX::SimpleMath::Vector3 pos;
				DirectX::SimpleMath::Quaternion rot;
				DirectX::SimpleMath::Vector3 scl;

				transform.Decompose(scl, rot, pos);

				scl = { 
					mScale * mLightProbeSystem->GetLightProbeScale(), 
					mScale * mLightProbeSystem->GetLightProbeScale(),
					mScale * mLightProbeSystem->GetLightProbeScale() };

				probeObjects[i]->SetTransform(DirectX::SimpleMath::Matrix::CreateScale(scl)
					* DirectX::SimpleMath::Matrix::CreateFromQuaternion(rot)
					* DirectX::SimpleMath::Matrix::CreateTranslation(pos));
			}
		}
		catch (const std::invalid_argument& e)
		{

		}
	}
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
	}
}
