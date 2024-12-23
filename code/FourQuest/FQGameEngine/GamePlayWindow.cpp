#include "GamePlayWindow.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <queue>

#include "../FQGameModule/SkinnedMeshRenderer.h"
#include "../FQGameModule/StaticMeshRenderer.h"
#include "../FQGameModule/InputManager.h"
#include "../FQGameModule/Light.h"
#include "../FQGameModule/Particle.h"
#include "../FQGameModule/Camera.h"
#include "../FQGameModule/Transform.h"
#include "../FQGameModule/Terrain.h"
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
	, mbIsMouseHoveredWindow(false)
	, mCameraObject(nullptr)
	, mCameraMoveSpeed(5.f)
	, mCameraRotateSpeed(0.0065f)
	, mOperation(ImGuizmo::OPERATION::TRANSLATE)
	, mSelectObjectHandler{}
	, mSelectObject(nullptr)
	, mbIsUsingGizumo(false)
	, mViewportSize{ 1.f,1.f }
	, mViewPortOffset{ 0.f,0.f }
	, mbAlreadyDrawGizumo(false)
	, mbIsFocused(false)
{}

fq::game_engine::GamePlayWindow::~GamePlayWindow()
{}

void fq::game_engine::GamePlayWindow::Render()
{
	if (ImGui::Begin("GamePlay", 0, ImGuiWindowFlags_MenuBar))
	{
		mbIsFocused = ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows);

		if (mWindowSize.x != ImGui::GetWindowSize().x
			|| mWindowSize.y != ImGui::GetWindowSize().y)
		{
			resizeWindow(ImGui::GetWindowSize());
		}

		checkMouse();
		pickObject();
		beginMenuBar_Control();
		beginImage_GameScreen();
		beginGizumo();
		drawSelectObjectDebugInfomation();
	}
	ImGui::End();

	// 기즈모 그리기 세팅을 초기화합니다
	mbAlreadyDrawGizumo = false;
}

void fq::game_engine::GamePlayWindow::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

	// 카메라 생성
	mCameraObject = std::make_shared<fq::game_module::GameObject>();;
	mCameraObject->AddComponent<fq::game_module::Camera>();
	mCameraObject->GetComponent<fq::game_module::Camera>()->SetFarPlain(100);

	mGameProcess->mCameraSystem->SetEditorCamera(mCameraObject->GetComponent<fq::game_module::Camera>());
	mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Editor);

	mSelectObjectHandler = mGameProcess->mEventManager->RegisterHandle<editor_event::SelectObject>(
		[this](editor_event::SelectObject event)
		{
			this->mSelectObject = event.object;
		});

	mSetScreenSizeHandler = mGameProcess->mEventManager->RegisterHandle<event::SetScreenSize>(
		[this](event::SetScreenSize event)
		{
			float aspectRatio = static_cast<float>(event.width) / event.height;
			mCameraObject->GetComponent<game_module::Camera>()->SetAspectRatio(aspectRatio);
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
	if (mMode == mode) return;

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

		mGameProcess->mEventManager
			->FireEvent<fq::event::SetViewportSize>(fq::event::SetViewportSize
		{ static_cast<unsigned short>(mViewportSize.x - mViewPortOffset.x)
				, static_cast<unsigned short>(mViewportSize.y - mViewPortOffset.y) });
	}
	// Play -> Edit
	else if (mode == EditorMode::Edit)
	{
		mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Editor);
		mGameProcess->mEventManager
			->FireEvent<fq::event::RequestChangeScene>({ currentSceneName, false });

		mGameProcess->mEventManager
			->FireEvent<fq::event::SetViewportSize>(fq::event::SetViewportSize
		{ static_cast<unsigned short>(mViewportSize.x), static_cast<unsigned short>(mViewportSize.y) });
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

			mGameProcess->mEventManager
				->FireEvent<fq::event::SetViewportSize>(fq::event::SetViewportSize
			{ static_cast<unsigned short>(mViewportSize.x), static_cast<unsigned short>(mViewportSize.y) });
		}
		if (input->IsKeyState(EKey::Num2, EKeyState::Tap))
		{
			mGameProcess->mCameraSystem->SetBindCamera(CameraSystem::CameraType::Game);
			mGameProcess->mEventManager
				->FireEvent<fq::event::SetViewportSize>(fq::event::SetViewportSize
			{ static_cast<unsigned short>(mViewportSize.x - mViewPortOffset.x)
					, static_cast<unsigned short>(mViewportSize.y - mViewPortOffset.y) });
		}
	}
}

void fq::game_engine::GamePlayWindow::beginImage_GameScreen()
{
	auto windowPos = ImGui::GetWindowPos();
	auto cursorPos = ImGui::GetCursorPos();
	auto viewportSize = mViewportSize;

	if (mGameProcess->mCameraSystem->GetCameraType() == CameraSystem::CameraType::Game)
	{
		cursorPos.x += mViewPortOffset.x * 0.5f;
		cursorPos.y += mViewPortOffset.y * 0.5f;

		ImGui::SetCursorPos(cursorPos);
		viewportSize.x -= mViewPortOffset.x;
		viewportSize.y -= mViewPortOffset.y;
	}

	mImagePos.x = windowPos.x + cursorPos.x;
	mImagePos.y = windowPos.y + cursorPos.y;
 
	ImGui::Image(mGameProcess->mGraphics->GetSRV(), viewportSize);
}

void fq::game_engine::GamePlayWindow::UpdateCamera(float dt)
{
	auto& input = mGameProcess->mInputManager;

	if (!input->IsKeyState(EKey::RMouse, EKeyState::Hold)
		|| !input->IsKeyState(EKey::LMouse, EKeyState::None)
		|| !mbIsMouseHoveredWindow)
	{
		return;
	}

	auto cameraT = mCameraObject->GetComponent<fq::game_module::Transform>();
	auto matrix = cameraT->GetLocalMatrix();
	auto position = cameraT->GetLocalPosition();
	auto rotation = cameraT->GetLocalRotation();
	float distance = mCameraMoveSpeed * dt;

	if (input->IsKeyState(EKey::LShift, EKeyState::Hold))
	{
		distance *= 5.f;
	}

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
	using namespace DirectX::SimpleMath;

	// 기즈모 창관련 설정 
	auto x = ImGui::GetWindowPos().x;
	auto y = ImGui::GetWindowPos().y;
	auto width = ImGui::GetWindowSize().x;
	auto height = ImGui::GetWindowSize().y;
	ImGuizmo::SetRect(x, y, width, height);

	// 이미 다른곳에서 기즈모 그린경우 그리지 않습니다.
	if (mbAlreadyDrawGizumo)
	{
		return;
	}

	if (mSelectObject == nullptr || mOperation == ImGuizmo::BOUNDS
		|| mGameProcess->mCameraSystem->GetCameraType() == CameraSystem::CameraType::Game)
	{
		//ImGuizmo::Enable(false);
		return;
	}

	ImGuizmo::Enable(true);

	auto objectT = mSelectObject->GetComponent<fq::game_module::Transform>();
	auto objectMatrix = objectT->GetWorldMatrix();
	auto beforeMatrix = objectT->GetWorldMatrix();

	auto& input = mEditorProcess->mInputManager;

	auto camera = mCameraObject->GetComponent<fq::game_module::Camera>();
	auto view = camera->GetView();
	auto proj = camera->GetProjection(mViewportSize.x / mViewportSize.y);

	bool useSnap = mEditorProcess->mSettingWindow->UseSnap();
	float* snap = mEditorProcess->mSettingWindow->GetSnap();
	auto mode = mEditorProcess->mSettingWindow->GetMode();

	if (ImGuizmo::Manipulate(&view._11, &proj._11
		, mOperation, mode, &objectMatrix._11, nullptr, useSnap ? &snap[0] : nullptr))
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

		if (mGameProcess->mCameraSystem->GetCameraType() == CameraSystem::CameraType::Editor)
		{
			mGameProcess->mEventManager
				->FireEvent<fq::event::SetViewportSize>(fq::event::SetViewportSize
			{ static_cast<unsigned short>(mViewportSize.x), static_cast<unsigned short>(mViewportSize.y) });
		}
		else
		{
			mGameProcess->mEventManager
				->FireEvent<fq::event::SetViewportSize>(fq::event::SetViewportSize
			{ static_cast<unsigned short>(mViewportSize.x - mViewPortOffset.x)
					, static_cast<unsigned short>(mViewportSize.y - mViewportSize.y) });
		}
	}


}

void fq::game_engine::GamePlayWindow::resizeWindow(ImVec2 size)
{
	mWindowSize = size;

	constexpr float offsetY = 70.f;
	auto camera = mCameraObject->GetComponent<fq::game_module::Camera>();

	constexpr float FixedAspectRatio = 16.f / 9.f;

	// 16 : 9 화면 비율 고정 
	float cameraAspectRatio = FixedAspectRatio;
	// Window 사이즈 비율 
	//float cameraAspectRatio = camera->GetAspectRatio();

	mViewportSize.x = size.x;
	mViewportSize.y = size.y - offsetY;

	// 게임 카메라 화면 비율 조절 
	float windowAspectRatio = mViewportSize.x / mViewportSize.y;

	// 가로가 길이 조절
	if (cameraAspectRatio < windowAspectRatio)
	{
		float resizeX = mViewportSize.y * cameraAspectRatio;
		mViewPortOffset.x = mViewportSize.x - resizeX;
		mViewPortOffset.y = 0.f;
	}
	// 세로 길이 조절
	else
	{
		float resizeY = mViewportSize.x / cameraAspectRatio;
		mViewPortOffset.y = mViewportSize.y - resizeY;
		mViewPortOffset.x = 0.f;
	}

	if (mGameProcess->mCameraSystem->GetCameraType() == CameraSystem::CameraType::Editor)
		mGameProcess->mGraphics->SetViewportSize(mViewportSize.x, mViewportSize.y);
	else
		mGameProcess->mGraphics->SetViewportSize(mViewportSize.x - mViewPortOffset.x, mViewportSize.y - mViewPortOffset.y);

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
		frustum.Color = { 1.f, 1.f, 0.f, 1.f };

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
	auto cameraPosition = target + backward;

	cameraT->SetLocalPosition(cameraPosition);
}

#pragma optimize( "", off )
void fq::game_engine::GamePlayWindow::pickObject()
{
	if (mEditorProcess->mInputManager->IsKeyState(EKey::LMouse, EKeyState::Tap) && mOperation == ImGuizmo::BOUNDS)
	{
		// 창내부에 마우스가 있는지 확인
		auto mousePos = ImGui::GetMousePos();
		auto currentContext = ImGui::GetCurrentContext();
		auto currentPos = ImVec2{ currentContext->CurrentWindow->Pos.x, currentContext->CurrentWindow->Pos.y };
		auto currentSize = ImVec2{ currentContext->CurrentWindow->Size.x, currentContext->CurrentWindow->Size.y };

		// 정규화 좌표 구하기
		DirectX::SimpleMath::Vector2 posInViewPort{ mousePos.x - mImagePos.x ,mousePos.y - mImagePos.y };
		DirectX::SimpleMath::Vector2 posInNormalized{ posInViewPort.x / currentSize.x , posInViewPort.y / currentSize.y };

		// 정규화 좌표로 버퍼 좌표 구하기
		const auto bufferWidth = mGameProcess->mGraphics->GetWindowWidth();
		const auto bufferHeight = mGameProcess->mGraphics->GetWindowHeight();
		DirectX::SimpleMath::Vector2 posInImage{ posInNormalized.x * bufferWidth, posInNormalized.y * bufferHeight };

		if (posInImage.x < 0
			|| posInImage.y < 0
			|| posInImage.x > bufferWidth
			|| posInImage.y > bufferHeight)
		{
			return;
		}

		void* meshPtr = mGameProcess->mGraphics->GetPickingObject(posInImage.x, posInImage.y);

		// 빈공간을 클릭한경우 
		if (meshPtr == nullptr)
		{
			if (mSelectObject)
			{
				mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
				mGameProcess->mEventManager.get(), nullptr, mSelectObject });
			}
			return;
		}

		// 하이어라키 펼치기 전달
		mGameProcess->mEventManager->FireEvent<fq::editor_event::FlipHierachy>({});

		// 피킹한 오브젝트 탐색
		auto scene = mGameProcess->mSceneManager->GetCurrentScene();
		scene->ViewComponents<fq::game_module::StaticMeshRenderer>(
			[this, meshPtr](fq::game_module::GameObject& object, game_module::StaticMeshRenderer& mesh)
			{
				if (mesh.GetStaticMeshObject() == meshPtr)
				{
					if (mEditorProcess->mSettingWindow->UseRootPicking())
					{
						mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
						mGameProcess->mEventManager.get(), object.GetRootObject()->shared_from_this(), mSelectObject });
					}
					else
					{
						mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
						mGameProcess->mEventManager.get(), object.shared_from_this(), mSelectObject });
					}
				}

			});

		scene->ViewComponents<fq::game_module::SkinnedMeshRenderer>(
			[this, meshPtr](fq::game_module::GameObject& object, game_module::SkinnedMeshRenderer& mesh)
			{
				if (mesh.GetSkinnedMeshObject() == meshPtr)
				{
					if (mEditorProcess->mSettingWindow->UseRootPicking())
					{
						mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
						mGameProcess->mEventManager.get(), object.GetRootObject()->shared_from_this(), mSelectObject });
					}
					else
					{
						mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
						mGameProcess->mEventManager.get(), object.shared_from_this(), mSelectObject });
					}
				}
			});

		scene->ViewComponents<game_module::Terrain>([this, meshPtr](fq::game_module::GameObject& object, game_module::Terrain& terrain)
			{
				if (terrain.GetTerrainMeshObject() == meshPtr)
				{
					if (mEditorProcess->mSettingWindow->UseRootPicking())
					{
						mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
						mGameProcess->mEventManager.get(), object.GetRootObject()->shared_from_this(), mSelectObject });
					}
					else
					{
						mEditorProcess->mCommandSystem->Push<SelectObjectCommand>(SelectObjectCommand{
						mGameProcess->mEventManager.get(), object.shared_from_this(), mSelectObject });
					}
				}
			});

		mEditorProcess->mLightProbeWindow->PickObject(meshPtr);
	}
}
#pragma optimize("", on)

void fq::game_engine::GamePlayWindow::checkMouse()
{

	if (mGameProcess->mInputManager->IsKeyState(EKey::RMouse, EKeyState::Away))
	{
		mbIsMouseHoveredWindow = false;
	}

	if (mGameProcess->mInputManager->IsKeyState(EKey::RMouse, EKeyState::Tap))
	{
		auto pos = ImGui::GetWindowPos();
		auto size = ImGui::GetWindowSize();
		auto mousePos = ImGui::GetMousePos();

		if (mousePos.x >= pos.x && mousePos.y >= pos.y
			&& mousePos.x <= pos.x + size.x && mousePos.y <= pos.y + size.y)
			mbIsMouseHoveredWindow = true;
		else
			mbIsMouseHoveredWindow = false;
	}
}

void fq::game_engine::GamePlayWindow::UpdateParticle(float dt)
{
	if (!mSelectObject || mSelectObject->IsDestroyed())
		return;

	std::queue<game_module::GameObject*> q;
	q.push(mSelectObject.get());

	while (!q.empty())
	{
		auto tmp = q.front();

		if (tmp && tmp->HasComponent<game_module::Particle>())
		{
			auto particle = tmp->GetComponent<game_module::Particle>();
			auto particleObject = particle->GetParticleObject();

			if (particleObject)
			{
				auto worldM = tmp->GetComponent<game_module::Transform>()->GetWorldMatrix();

				particleObject->SetTransform(worldM);
				particleObject->SetFrameTime(dt);
			}
		}

		for (auto child : tmp->GetChildren())
		{
			q.push(child);
		}
		q.pop();
	}
}

void fq::game_engine::GamePlayWindow::DrawGizumo(DirectX::SimpleMath::Matrix& transform)
{
	if (mbAlreadyDrawGizumo)
	{
		return;
	}

	mbAlreadyDrawGizumo = true;
	ImGuizmo::Enable(true);

	//auto objectT = mSelectObject->GetComponent<fq::game_module::Transform>();
	auto objectMatrix = transform;

	auto& input = mEditorProcess->mInputManager;

	auto camera = mCameraObject->GetComponent<fq::game_module::Camera>();
	auto view = camera->GetView();
	auto proj = camera->GetProjection(mViewportSize.x / mViewportSize.y);

	bool useSnap = mEditorProcess->mSettingWindow->UseSnap();
	float* snap = mEditorProcess->mSettingWindow->GetSnap();
	auto mode = mEditorProcess->mSettingWindow->GetMode();

	if (ImGuizmo::Manipulate(&view._11, &proj._11
		, mOperation, mode, &objectMatrix._11, nullptr, useSnap ? &snap[0] : nullptr))
	{
		transform = objectMatrix;
	}
}
