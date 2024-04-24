#include "GamePlayWindow.h"

#include <imgui.h>
#include <imgui_internal.h>

#include "../FQGameModule/InputManager.h"
#include "../FQGraphics/IFQGraphics.h"

#include "EditorProcess.h"
#include "EditorEvent.h"
#include "GameProcess.h"
#include "EditorEvent.h"
#include "CameraSystem.h"
#include "WindowSystem.h"
#include "CommandSystem.h"
#include "Command.h"

fq::game_engine::GamePlayWindow::GamePlayWindow()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mMode(EditorMode::Edit)
	, mbIsPauseGame(false)
	, mCameraObject(nullptr)
	, mCameraMoveSpeed(20.f)
	, mCameraRotateSpeed(0.0065f)
	, mOperation(ImGuizmo::OPERATION::TRANSLATE)
	, mSelectObjectHandler{}
	, mSelectObject(nullptr)
	, mViewTM{}
	, mProjTM{}
	, mbIsUsingGizumo(false)
{}

fq::game_engine::GamePlayWindow::~GamePlayWindow()
{}

void fq::game_engine::GamePlayWindow::Render()
{
	if (ImGui::Begin("GamePlay", 0, ImGuiWindowFlags_MenuBar))
	{
		beginMenuBar_Control();
		beginImage_GameScreen();

		beginGizumo();
	}
	ImGui::End();
}

void fq::game_engine::GamePlayWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

	// 카메라 생성
	mCameraObject = std::make_shared<fq::game_module::GameObject>();;
	mCameraObject->AddComponent<fq::game_module::Camera>();

	mGameProcess->mCameraSystem->SetEditorCamera(mCameraObject->GetComponent<fq::game_module::Camera>());
	mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Editor);

	mSelectObjectHandler = mGameProcess->mEventManager->RegisterHandle<editor_event::SelectObject>(
		[this](editor_event::SelectObject event)
		{
			this->mSelectObject = event.object;
		});
}

void fq::game_engine::GamePlayWindow::Finalize()
{
}

void fq::game_engine::GamePlayWindow::beginMenuBar_Control()
{
	if (ImGui::BeginMenuBar())
	{
		beginButton_Play();
		beginButton_Stop();

		ImGui::EndMenuBar();
	}
}

void fq::game_engine::GamePlayWindow::beginButton_Stop()
{
	if (mbIsPauseGame)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.f, 0.f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		if (ImGui::Button("Stop"))
		{
			mbIsPauseGame = false;
			mGameProcess->mTimeManager->SetTimeScale(1.);
		}
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Stop"))
		{
			mbIsPauseGame = true;
			mGameProcess->mTimeManager->SetTimeScale(0.);
		}
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Ctrl + Shift + P");
	}
}

void fq::game_engine::GamePlayWindow::beginButton_Play()
{
	float currsorX = (ImGui::GetWindowSize().x / 2.f) - ImGui::CalcTextSize("Play").x;

	ImGui::SetCursorPosX(currsorX);

	if (mMode == EditorMode::Play)
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.8f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

		if (ImGui::Button("Play"))
		{
			SetMode(EditorMode::Edit);
		}

		ImGui::PopStyleColor(2);
	}
	else if (mMode == EditorMode::Edit)
	{
		if (ImGui::Button("Play"))
		{
			SetMode(EditorMode::Play);
		}
	}

	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Ctrl + P");
	}
}

void fq::game_engine::GamePlayWindow::SetMode(EditorMode mode)
{
	if (mMode == mode)
	{
		return;
	}

	auto currentSceneName = mGameProcess->mSceneManager->
		GetCurrentScene()->GetSceneName();

	// Edit -> Play
	if (mode == EditorMode::Play)
	{
		// 현재 씬을 저장합니다 
		mEditorProcess->mMainMenuBar->SaveScene();

		mGameProcess->mEventManager
			->FireEvent<fq::event::RequestChangeScene>({ currentSceneName, true });
	}
	// Play -> Edit
	else if (mode == EditorMode::Edit)
	{
		mGameProcess->mEventManager
			->FireEvent<fq::event::RequestChangeScene>({ currentSceneName, false });
	}

	mGameProcess->mEventManager->FireEvent<editor_event::SelectObject>({ nullptr });

	mMode = mode;
}

void fq::game_engine::GamePlayWindow::ExcutShortcut()
{
	const auto& input = mEditorProcess->mInputManager;

	if (input->IsKeyState(Key::Ctrl, KeyState::Hold)
		&& input->IsKeyState(Key::P, KeyState::Tap))
	{
		if (input->IsKeyState(Key::LShift, KeyState::Hold))
		{
			if (mbIsPauseGame)
			{
				mGameProcess->mTimeManager->SetTimeScale(1.);
			}
			else
			{
				mGameProcess->mTimeManager->SetTimeScale(0.);
			}

			mbIsPauseGame = !mbIsPauseGame;
		}
		else
		{
			if (mMode == EditorMode::Edit)
			{
				SetMode(EditorMode::Play);
			}
			else
			{
				SetMode(EditorMode::Edit);
			}
		}
	}

	if (input->IsKeyState(Key::W, KeyState::Tap))
	{
		mOperation = ImGuizmo::TRANSLATE;
	}
	if (input->IsKeyState(Key::E, KeyState::Tap))
	{
		mOperation = ImGuizmo::ROTATE;
	}
	if (input->IsKeyState(Key::R, KeyState::Tap))
	{
		mOperation = ImGuizmo::SCALE;
	}
	if (input->IsKeyState(Key::Q, KeyState::Tap))
	{
		mOperation = ImGuizmo::BOUNDS;
	}
}

void fq::game_engine::GamePlayWindow::beginImage_GameScreen()
{
	//auto size = ImGui::GetWindowSize();
	//mGameProcess->mGraphics->SetViewportSize(size.x, size.y);

	auto current = ImGui::GetCurrentContext();
	auto pos = current->CurrentWindow->Pos;
	auto size = current->CurrentWindow->Size;
	auto idealSize = ImVec2(size.x, size.y - 3);
	auto maxpos = ImVec2(pos.x + idealSize.x, pos.y + idealSize.y);
	auto borderSize = current->CurrentWindow->WindowBorderSize;

	ImGui::GetWindowDrawList()->AddImage(
		mGameProcess->mGraphics->GetSRV(),
		ImVec2(pos.x, pos.y + 22),
		ImVec2(maxpos.x, maxpos.y));
}

void fq::game_engine::GamePlayWindow::UpdateCamera(float dt)
{
	auto& input = mEditorProcess->mInputManager;
	if (!input->IsKeyState(Key::RMouse, KeyState::Hold))
	{
		return;
	}

	auto cameraT = mCameraObject->GetComponent<fq::game_module::Transform>();
	auto matrix = cameraT->GetLocalMatrix();
	auto position = cameraT->GetLocalPosition();
	auto rotation = cameraT->GetLocalRotation();
	float distance = mCameraMoveSpeed * dt;

	if (input->IsKeyState(Key::W, KeyState::Hold))
	{
		position.x += matrix._31 * distance;
		position.y += matrix._32 * distance;
		position.z += matrix._33 * distance;
	}
	if (input->IsKeyState(Key::S, KeyState::Hold))
	{
		position.x -= matrix._31 * distance;
		position.y -= matrix._32 * distance;
		position.z -= matrix._33 * distance;
	}
	if (input->IsKeyState(Key::A, KeyState::Hold))
	{
		position.x -= matrix._11 * distance;
		position.y -= matrix._12 * distance;
		position.z -= matrix._13 * distance;
	}
	if (input->IsKeyState(Key::D, KeyState::Hold))
	{
		position.x += matrix._11 * distance;
		position.y += matrix._12 * distance;
		position.z += matrix._13 * distance;
	}
	if (input->IsKeyState(Key::Q, KeyState::Hold))
	{
		position.x -= matrix._21 * distance;
		position.y -= matrix._22 * distance;
		position.z -= matrix._23 * distance;
	}
	if (input->IsKeyState(Key::E, KeyState::Hold))
	{
		position.x += matrix._21 * distance;
		position.y += matrix._22 * distance;
		position.z += matrix._23 * distance;
	}

	
	float dx = mCameraRotateSpeed * static_cast<float>(input->GetDeltaMousePosition().x);
	auto x = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 0,1,0 }, dx);

	float dy = mCameraRotateSpeed * static_cast<float>(input->GetDeltaMousePosition().y);
	auto y = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle({ 1,0,0 }, dy);

	rotation = y * rotation * x;

	cameraT->SetLocalRotation(rotation);
	cameraT->SetLocalPosition(position);
}

void fq::game_engine::GamePlayWindow::beginGizumo()
{
	if (mSelectObject == nullptr || mOperation == ImGuizmo::BOUNDS)
	{
		ImGuizmo::Enable(false);
		return;
	}

	ImGuizmo::Enable(true);

	using namespace DirectX::SimpleMath;

	auto camera = mCameraObject->GetComponent<fq::game_module::Camera>();
	auto cameraT = mCameraObject->GetComponent<fq::game_module::Transform>();
	auto cameraInfo = camera->GetCameraInfomation();

	auto matrix = cameraT->GetLocalMatrix();
	auto fov = cameraInfo.filedOfView;
	auto aspectRatio = mGameProcess->mWindowSystem->GetScreenWidth()
		/ mGameProcess->mWindowSystem->GetScreenHeight();
	auto nearPlain = cameraInfo.nearPlain;
	auto farPlain = cameraInfo.farPlain;

	mViewTM = matrix.Invert();
	mProjTM = DirectX::XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlain, farPlain);

	auto x = ImGui::GetWindowPos().x;
	auto y = ImGui::GetWindowPos().y;
	auto width = ImGui::GetWindowSize().x;
	auto height = ImGui::GetWindowSize().y;

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(x, y, width, height);

	auto objectT = mSelectObject->GetComponent<fq::game_module::Transform>();
	auto objectMatrix = objectT->GetWorldMatrix();
	auto beforeMatrix = objectT->GetWorldMatrix();

	auto& input = mEditorProcess->mInputManager;

	if (ImGuizmo::Manipulate(&mViewTM._11, &mProjTM._11
		, mOperation, ImGuizmo::WORLD, &objectMatrix._11))
	{
		if (objectT->HasParent())
		{
			auto parentMatrix = objectT->GetParentTransform()->GetWorldMatrix();
			objectMatrix = objectMatrix * parentMatrix.Invert();

			objectT->SetLocalMatrix(objectMatrix);
		}
		else
		{
			objectT->SetLocalMatrix(objectMatrix);
		}
	}

	if (ImGuizmo::IsUsing()
		&& input->IsKeyState(Key::LMouse, KeyState::Tap))
	{
		mStart = beforeMatrix;
		mbIsUsingGizumo = true;
	}
	
	if (mbIsUsingGizumo && input->IsKeyState(Key::LMouse, KeyState::Away))
	{
		mEditorProcess->mCommandSystem->Push<SetValueCommand<DirectX::SimpleMath::Matrix>>
			(
				[ objectT, selectObject = mSelectObject](DirectX::SimpleMath::Matrix mat)
				{
					if (objectT->HasParent())
					{
						auto parentMatrix = objectT->GetParentTransform()->GetWorldMatrix();
						mat = mat * parentMatrix.Invert();

						objectT->SetLocalMatrix(mat);
					}
					else
					{
						objectT->SetLocalMatrix(mat);
					}
				}, mStart, objectMatrix
			);
		mbIsUsingGizumo = false;
	}

}
