#include "GamePlayWindow.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>
#include <algorithm>

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
	, mbIsOpen(true)
	, mCameraObject(nullptr)
	, mCameraMoveSpeed(20.f)
	, mCameraRotateSpeed(0.0065f)
	, mOperation(ImGuizmo::OPERATION::TRANSLATE)
	, mSelectObjectHandler{}
	, mSelectObject(nullptr)
	, mbIsUsingGizumo(false)
	, mViewportSize{ 1.f,1.f }
{}

fq::game_engine::GamePlayWindow::~GamePlayWindow()
{}

void fq::game_engine::GamePlayWindow::Render()
{
	if (ImGui::Begin("GamePlay", 0, ImGuiWindowFlags_MenuBar))
	{
		if (mWindowSize.x != ImGui::GetWindowSize().x
			|| mWindowSize.y != ImGui::GetWindowSize().y)
		{
			resizeWindow(ImGui::GetWindowSize());
		}

		pickObject();
		beginMenuBar_Control();
		beginImage_GameScreen();
		beginGizumo();
		drawSelectObjectDebugInfomation();
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
	mCameraObject->GetComponent<fq::game_module::Camera>()->SetFarPlain(10000000);

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
		beginButton_SwapCamera();
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
		}
		ImGui::PopStyleColor(2);
	}
	else
	{
		if (ImGui::Button("Stop"))
		{
			mbIsPauseGame = true;
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

		mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Game);
		mGameProcess->mEventManager
			->FireEvent<fq::event::RequestChangeScene>({ currentSceneName, true });
	}
	// Play -> Edit
	else if (mode == EditorMode::Edit)
	{
		mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Editor);
		mGameProcess->mEventManager
			->FireEvent<fq::event::RequestChangeScene>({ currentSceneName, false });
	}

	mGameProcess->mEventManager->FireEvent<editor_event::SelectObject>({ nullptr });

	mMode = mode;
}

void fq::game_engine::GamePlayWindow::ExcutShortcut()
{
	const auto& input = mEditorProcess->mInputManager;

	if (input->IsKeyState(EKey::Ctrl, EKeyState::Hold)
		&& input->IsKeyState(EKey::P, EKeyState::Tap))
	{
		// [Ctrl + LShift + P] 게임 정지
		if (input->IsKeyState(EKey::LShift, EKeyState::Hold))
		{
			mbIsPauseGame = !mbIsPauseGame;
		}
		else
		{
			// [Ctrl + P] 게임플레이 
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

	if (input->IsKeyState(EKey::RMouse, EKeyState::None))
	{
		if (input->IsKeyState(EKey::W, EKeyState::Tap))
		{
			mOperation = ImGuizmo::TRANSLATE;
		}
		if (input->IsKeyState(EKey::E, EKeyState::Tap))
		{
			mOperation = ImGuizmo::ROTATE;
		}
		if (input->IsKeyState(EKey::R, EKeyState::Tap))
		{
			mOperation = ImGuizmo::SCALE;
		}
		if (input->IsKeyState(EKey::Q, EKeyState::Tap))
		{
			mOperation = ImGuizmo::BOUNDS;
		}
	}

	if (input->IsKeyState(EKey::Ctrl, EKeyState::Hold))
	{
		if (input->IsKeyState(EKey::Num1, EKeyState::Tap))
		{
			mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Editor);
		}
		if (input->IsKeyState(EKey::Num2, EKeyState::Tap))
		{
			mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Game);
		}
	}
}

void fq::game_engine::GamePlayWindow::beginImage_GameScreen()
{
	auto windowPos = ImGui::GetWindowPos();
	auto cursorPos = ImGui::GetCursorPos();
	mImagePos.x = windowPos.x + cursorPos.x;
	mImagePos.y = windowPos.y + cursorPos.y;

	ImGui::Image(mGameProcess->mGraphics->GetSRV(), mViewportSize);
}

void fq::game_engine::GamePlayWindow::UpdateCamera(float dt)
{
	auto& input = mEditorProcess->mInputManager;
	if (!input->IsKeyState(EKey::RMouse, EKeyState::Hold)
		|| !input->IsKeyState(EKey::LMouse, EKeyState::None))
	{
		return;
	}

	auto cameraT = mCameraObject->GetComponent<fq::game_module::Transform>();
	auto matrix = cameraT->GetLocalMatrix();
	auto position = cameraT->GetLocalPosition();
	auto rotation = cameraT->GetLocalRotation();
	float distance = mCameraMoveSpeed * dt;

	if (input->IsKeyState(EKey::W, EKeyState::Hold))
	{
		position.x += matrix._31 * distance;
		position.y += matrix._32 * distance;
		position.z += matrix._33 * distance;
	}
	if (input->IsKeyState(EKey::S, EKeyState::Hold))
	{
		position.x -= matrix._31 * distance;
		position.y -= matrix._32 * distance;
		position.z -= matrix._33 * distance;
	}
	if (input->IsKeyState(EKey::A, EKeyState::Hold))
	{
		position.x -= matrix._11 * distance;
		position.y -= matrix._12 * distance;
		position.z -= matrix._13 * distance;
	}
	if (input->IsKeyState(EKey::D, EKeyState::Hold))
	{
		position.x += matrix._11 * distance;
		position.y += matrix._12 * distance;
		position.z += matrix._13 * distance;
	}
	if (input->IsKeyState(EKey::Q, EKeyState::Hold))
	{
		position.x -= matrix._21 * distance;
		position.y -= matrix._22 * distance;
		position.z -= matrix._23 * distance;
	}
	if (input->IsKeyState(EKey::E, EKeyState::Hold))
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
	if (mSelectObject == nullptr || mOperation == ImGuizmo::BOUNDS
		|| mGameProcess->mCameraSystem->GetCameraType() == CameraSystem::CameraType::Game)
	{
		ImGuizmo::Enable(false);
		return;
	}

	ImGuizmo::Enable(true);

	using namespace DirectX::SimpleMath;

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

	auto camera = mCameraObject->GetComponent<fq::game_module::Camera>();
	auto view = camera->GetView();
	auto proj = camera->GetProjection(mViewportSize.x / mViewportSize.y);

	if (ImGuizmo::Manipulate(&view._11, &proj._11
		, mOperation, ImGuizmo::WORLD, &objectMatrix._11))
	{
		if (objectT->HasParent())
		{
			objectT->SetWorldMatrix(objectMatrix);
		}
		else
		{
			objectT->SetLocalMatrix(objectMatrix);
		}
	}

	if (ImGuizmo::IsOver()
		&& input->IsKeyState(EKey::LMouse, EKeyState::Tap))
	{
		mStart = beforeMatrix;
		mbIsUsingGizumo = true;
	}

	if (mbIsUsingGizumo && input->IsKeyState(EKey::LMouse, EKeyState::Away))
	{
		mEditorProcess->mCommandSystem->Push<SetValueCommand<DirectX::SimpleMath::Matrix>>
			(
				[objectT, selectObject = mSelectObject](DirectX::SimpleMath::Matrix mat)
				{
					if (objectT->HasParent())
					{
						objectT->SetWorldMatrix(mat);
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

void fq::game_engine::GamePlayWindow::beginButton_SwapCamera()
{
	auto& cameraSystem = mGameProcess->mCameraSystem;

	auto type = cameraSystem->GetCameraType();

	std::string cameraType{};

	if (type == CameraSystem::CameraType::Editor)
	{
		cameraType = "Editor";
	}
	else if (type == CameraSystem::CameraType::Game)
	{
		cameraType = "Game";
	}

	if (ImGui::Button(cameraType.c_str()))
	{
		if (type == CameraSystem::CameraType::Editor)
		{
			cameraSystem->SetBindCamera(CameraSystem::CameraType::Game);
		}
		else
		{
			cameraSystem->SetBindCamera(CameraSystem::CameraType::Editor);
		}
	}
}

void fq::game_engine::GamePlayWindow::resizeWindow(ImVec2 size)
{
	mWindowSize = size;

	constexpr float offsetY = 70.f;

	mViewportSize.x = size.x;
	mViewportSize.y = size.y - offsetY;

	auto camera = mCameraObject->GetComponent<fq::game_module::Camera>();
	mGameProcess->mGraphics->SetViewportSize(mViewportSize.x, mViewportSize.y);
	mGameProcess->mGraphics->SetCamera(camera->GetCameraInfomation());
}

fq::game_engine::EditorMode fq::game_engine::GamePlayWindow::GetMode() const
{
	if (mbIsPauseGame && mMode == EditorMode::Play)
	{
		return EditorMode::Pause;
	}

	return mMode;
}

void fq::game_engine::GamePlayWindow::drawSelectObjectDebugInfomation()
{
	if (!mSelectObject || mGameProcess->mCameraSystem->GetCameraType() != CameraSystem::CameraType::Editor)  return;

	using namespace fq::game_module;

	// 카메라 정보 표시
	if (mSelectObject->HasComponent<Camera>())
	{
		auto camera = mSelectObject->GetComponent<Camera>();
		auto cameraT = mSelectObject->GetComponent<Transform>();

		fq::graphics::debug::FrustumInfo frustum;
		frustum.Color = { 0.f,0.f,0.f,1.f };

		float ratio = ImGui::GetWindowSize().x / ImGui::GetWindowSize().y;

		DirectX::BoundingFrustum::CreateFromMatrix(frustum.Frustum, camera->GetProjection(ratio));
		frustum.Frustum.Origin = cameraT->GetWorldPosition();
		frustum.Frustum.Orientation = cameraT->GetWorldRotation();

		mGameProcess->mGraphics->DrawFrustum(frustum);
	}

	// 라이트 정보 표시
	if (mSelectObject->HasComponent<Light>())
	{
		auto light = mSelectObject->GetComponent<Light>();

		if (light->GetLightType() == fq::graphics::ELightType::Directional)
		{
			mEditorProcess->mDebugSystem->RenderDirLight(*light);
		}
		else if (light->GetLightType() == fq::graphics::ELightType::Point)
		{
			mEditorProcess->mDebugSystem->RenderPointLight(*light);
		}
		else if (light->GetLightType() == fq::graphics::ELightType::Spot)
		{
			mEditorProcess->mDebugSystem->RenderSpotLight(*light);
		}

	}

}

void fq::game_engine::GamePlayWindow::LookAtTarget(DirectX::SimpleMath::Vector3 target)
{
	auto cameraT = mCameraObject->GetComponent<fq::game_module::Transform>();
	auto backward = cameraT->GetLocalMatrix().Forward();
	auto cameraPosition = target + backward * 100.f;

	cameraT->SetLocalPosition(cameraPosition);
}


void fq::game_engine::GamePlayWindow::pickObject()
{
	if (mEditorProcess->mInputManager->IsKeyState(EKey::LMouse, EKeyState::Tap) && mOperation == ImGuizmo::BOUNDS)
	{
		// 창내부에 마우스가 있는지 확인
		auto mousePos = ImGui::GetMousePos();
		auto windowPos = ImGui::GetWindowPos();
		auto windowSize = ImGui::GetWindowSize();

		if (mousePos.x < windowPos.x || mousePos.y < windowPos.y ||
			mousePos.x > windowPos.x + windowSize.x || mousePos.y > windowPos.y + windowSize.y)
		{
			return;
		}

		// 스크린 좌표 -> 이미지 좌표변환 
		mousePos.x = std::clamp(mousePos.x - mImagePos.x, 0.f, mViewportSize.x - 1);
		mousePos.y = std::clamp(mousePos.y - mImagePos.y, 0.f, mViewportSize.y - 1);

		UINT screenWidth = mGameProcess->mWindowSystem->GetScreenWidth();
		UINT screenHeight = mGameProcess->mWindowSystem->GetScreenHeight();

		// 이미지 좌표 -> 뷰포트 좌표 변환
		mousePos.x = mousePos.x * static_cast<float>(screenWidth) / mViewportSize.x;
		mousePos.y = mousePos.y * static_cast<float>(screenHeight) / mViewportSize.y;

		UINT mousePosX = std::clamp(static_cast<UINT>(mousePos.x), 0u, screenWidth);
		UINT mousePosY = std::clamp(static_cast<UINT>(mousePos.y), 0u, screenHeight);

		void* meshPtr = mGameProcess->mGraphics->GetPickingObject(mousePosX, mousePosY);

		// 빈공간을 클릭한경우 
		if (meshPtr == nullptr)
		{
			if (mSelectObject)
			{
				mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
				mGameProcess->mEventManager.get(), nullptr, mSelectObject });
			}
			spdlog::trace("nullptr");

			return;
		}

		// 피킹한 오브젝트 탐색
		auto scene = mGameProcess->mSceneManager->GetCurrentScene();
		scene->ViewComponents<fq::game_module::StaticMeshRenderer>(
			[this, meshPtr](fq::game_module::GameObject& object, game_module::StaticMeshRenderer& mesh)
			{
				if (mesh.GetStaticMeshObject() == meshPtr)
				{
					mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
					mGameProcess->mEventManager.get(), object.shared_from_this(), mSelectObject });
				}
			});

		scene->ViewComponents<fq::game_module::SkinnedMeshRenderer>(
			[this, meshPtr](fq::game_module::GameObject& object, game_module::SkinnedMeshRenderer& mesh)
			{
				if (mesh.GetSkinnedMeshObject() == meshPtr)
				{
					mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
					mGameProcess->mEventManager.get(), object.shared_from_this(), mSelectObject });
				}
			});
	}
}
