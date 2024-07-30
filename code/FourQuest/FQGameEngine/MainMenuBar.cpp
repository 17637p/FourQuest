#include "MainMenuBar.h"

#include <string>
#include <spdlog/spdlog.h>
#include "spdlog/stopwatch.h"
#include <imgui.h>
#include "imgui_stdlib.h"

#include "../FQCommon/FQPath.h"
#include "../FQGameModule/GameModule.h"
#include "GameProcess.h"
#include "EditorProcess.h"
#include "PhysicsSystem.h"

fq::game_engine::MainMenuBar::MainMenuBar()
	:mGameProcess(nullptr)
	, mEditorProcess(nullptr)
	, mbIsOpenCreateWindowPopup(false)
	, mCreateSceneName{ "4Quset" }
	, mCurrentSceneName{}
	, mSceneList{}
	, mOnLoadSceneHandler{}
{}

fq::game_engine::MainMenuBar::~MainMenuBar()
{
}

void fq::game_engine::MainMenuBar::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;

	// 이벤트 핸들 등록
	mOnLoadSceneHandler = mGameProcess->mEventManager
		->RegisterHandle<fq::event::OnLoadScene>([this](const fq::event::OnLoadScene& event)
			{
				this->SetCurrentSceneName(event.sceneName);
			});
}

void fq::game_engine::MainMenuBar::Render()
{
	if (ImGui::BeginMainMenuBar())
	{
		beginMenu_File();
		beginMenu_Window();
		beginMenu_DebugOption();

		beginText_SceneName();
		beginText_FPS();
	}
	ImGui::EndMainMenuBar();

	beginOpenPopup_CreateScene();
}



void fq::game_engine::MainMenuBar::beginMenuItem_CreateScene()
{
	if (ImGui::MenuItem("Create Scene"))
	{
		mbIsOpenCreateWindowPopup = true;
	}
}

void fq::game_engine::MainMenuBar::beginMenuItem_LoadScene()
{
	if (ImGui::BeginMenu("Load Scene"))
	{
		updateSceneList();

		for (const auto& path : mSceneList)
		{
			std::string sceneName = path.filename().string();

			if (ImGui::MenuItem(sceneName.c_str()))
			{
				SaveScene();
				// Scene 변경 요청 
				mGameProcess->mEventManager
					->FireEvent<fq::event::RequestChangeScene>({ sceneName, false });
			}
		}

		ImGui::EndMenu();
	}
}

void fq::game_engine::MainMenuBar::beginMenuItem_SaveScene()
{
	if (ImGui::MenuItem("Save Scene"))
	{
		SaveScene();
	}
}

void fq::game_engine::MainMenuBar::beginMenu_File()
{
	if (ImGui::BeginMenu("File"))
	{
		beginMenuItem_CreateScene();
		beginMenuItem_LoadScene();
		beginMenuItem_SaveScene();

		ImGui::EndMenu();
	}
}

void fq::game_engine::MainMenuBar::beginOpenPopup_CreateScene()
{
	if (mbIsOpenCreateWindowPopup)
	{
		ImGui::OpenPopup("Create Scene");
		mbIsOpenCreateWindowPopup = false;
	}

	if (ImGui::BeginPopupModal("Create Scene"
		, nullptr
		, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::InputText("Scene Name ##create scene popup winodw"
			, &mCreateSceneName);

		// 생성 
		if (ImGui::Button("Create"))
		{
			// 씬 중복을 확인 
			updateSceneList();

			createScene(mCreateSceneName);
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		// 취소
		if (ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void fq::game_engine::MainMenuBar::updateSceneList()
{
	auto scenePath = fq::path::GetScenePath();
	mSceneList = fq::path::GetFileList(scenePath);
}

void fq::game_engine::MainMenuBar::createScene(std::string sceneName)
{
	namespace fs = std::filesystem;

	// scene 폴더 생성
	PathType scenePath = fq::path::GetScenePath();
	scenePath /= sceneName;
	fs::create_directory(scenePath);

	// prefab 폴더 생성
	PathType prefabPath = scenePath;
	prefabPath /= "prefab";
	fs::create_directory(prefabPath);

	// coliision_matrix 파일 생성
	fq::physics::CollisionMatrix().Save(scenePath);

	// skybox 파일 생성
	fq::game_module::SkyBox().Save(scenePath);

	// ... etc 

	// Scene 변경 요청 
	mGameProcess->mEventManager
		->FireEvent<fq::event::RequestChangeScene>({ sceneName });

	spdlog::trace("Create Scene");
}

void fq::game_engine::MainMenuBar::SetCurrentSceneName(std::string sceneName)
{
	mCurrentSceneName = std::move(sceneName);
}

void fq::game_engine::MainMenuBar::beginText_SceneName()
{
	std::string title = "[ " + mCreateSceneName + " ]";

	float cursorPosX = ImGui::GetWindowSize().x / 2
		- ImGui::CalcTextSize(title.c_str()).x / 2;

	ImGui::SetCursorPosX(cursorPosX);
	ImGui::Text(title.c_str());
}

void fq::game_engine::MainMenuBar::beginText_FPS()
{
	std::string fps = "FPS " + std::to_string(mGameProcess->mTimeManager->GetFPS());

	float cursorPosX = ImGui::GetWindowSize().x - ImGui::CalcTextSize(fps.c_str()).x;

	ImGui::SetCursorPosX(cursorPosX);
	ImGui::Text(fps.c_str());
}

void fq::game_engine::MainMenuBar::SaveScene()
{
	spdlog::stopwatch sw;

	auto scenePath = fq::path::GetScenePath();
	scenePath /= mCurrentSceneName;

	// 1. 프리팹 저장 
	auto scene = mGameProcess->mSceneManager->GetCurrentScene();
	auto& objectManager = mGameProcess->mPrefabManager;
	auto prefabPath = scenePath / "prefab";

	fq::path::ClearDirectory(prefabPath);

	for (auto& object : scene->GetObjectView(true))
	{
		// 부모계층을 기준으로 prefab을 생성합니다
		if (!object.HasParent())
		{
			objectManager->SavePrefab(&object, prefabPath);
		}
	}

	// 2. CollisionMatrix 저장
	mGameProcess->mPhysicsSystem->GetCollisionMatrix().Save(scenePath);

	// 3. AnimatorController 저장
	mEditorProcess->mAnimatorWindow->SaveAnimatorController();

	// 3.SkyBox 저장
	mEditorProcess->mSkyBoxWindow->SaveSkyBox(scenePath);

	mEditorProcess->mMaterialWindow->SaveMaterial();

	//  ... etc 
	spdlog::trace("[MainMenuBar] Save \"{}\" Scene [{}s]", mCurrentSceneName, sw);
}

void fq::game_engine::MainMenuBar::ExcuteShortcut()
{
	const auto& input = mEditorProcess->mInputManager;

	// 현재 씬이 플레이중이면 저장하지 않는다 
	if (mEditorProcess->mGamePlayWindow->GetMode() == EditorMode::Edit
		&& input->IsKeyState(EKey::Ctrl, EKeyState::Hold)
		&& input->IsKeyState(EKey::S, EKeyState::Tap))
	{
		SaveScene();
	}
}

void fq::game_engine::MainMenuBar::beginMenu_Window()
{
	if (ImGui::BeginMenu("Window"))
	{
		bool& hierarchy = mEditorProcess->mHierarchy->IsWindowOpen();
		ImGui::Checkbox("Hierarchy", &hierarchy);

		bool& inspector = mEditorProcess->mInspector->IsWindowOpen();
		ImGui::Checkbox("Inspector", &inspector);

		bool& fileDialog = mEditorProcess->mFileDialog->IsWindowOpen();
		ImGui::Checkbox("FileDialog", &fileDialog);

		bool& log = mEditorProcess->mLogWindow->IsWindowOpen();
		ImGui::Checkbox("Log", &log);

		ImGui::Separator();

		bool& animator = mEditorProcess->mAnimatorWindow->IsWindowOpen();
		ImGui::Checkbox("Animator", &animator);

		bool& collisionMatrix = mEditorProcess->mCollisionMatrixWindow->IsWindowOpen();
		ImGui::Checkbox("CollisionMatrix", &collisionMatrix);

		bool& physicsAnimator = mEditorProcess->mPhysicsAnimatorWindow->IsWindowOpen();
		ImGui::Checkbox("PhysicsAnimator", &physicsAnimator);

		bool& skyBox = mEditorProcess->mSkyBoxWindow->IsWindowOpen();
		ImGui::Checkbox("SkyBox", &skyBox);

		bool& onSetting = mEditorProcess->mSettingWindow->IsWindowOpen();
		ImGui::Checkbox("Setting", &onSetting);

		bool& onNavMesh = mEditorProcess->mNavMeshWindow->IsWindowOpen();
		ImGui::Checkbox("NavigationMesh", &onNavMesh);

		bool& articulationHierarchy = mEditorProcess->mArticulationHierarchy->IsWindowOpen();
		ImGui::Checkbox("ArticulationHierarchy", &articulationHierarchy);

		bool& articulationInspector = mEditorProcess->mArticulationInspector->IsWindowOpen();
		ImGui::Checkbox("ArticulationInspector", &articulationInspector);

		bool& onLightProbe = mEditorProcess->mLightProbeWindow->IsWindowOpen();
		ImGui::Checkbox("LightProbe", &onLightProbe);

		bool& onExpotWindow = mEditorProcess->mExportWindow->IsWindowOpen();
		ImGui::Checkbox("ExportWindow", &onExpotWindow);

		bool& onImportWindow = mEditorProcess->mImportWindow->IsWindowOpen();
		ImGui::Checkbox("ImportWindow", &onImportWindow);
		bool& onMaterialWindow = mEditorProcess->mMaterialWindow->IsWindowOpen();
		ImGui::Checkbox("MaterialWindow", &onMaterialWindow);

		ImGui::EndMenu();
	}
}

void fq::game_engine::MainMenuBar::beginMenu_DebugOption()
{
	if (ImGui::BeginMenu("Debug"))
	{
		auto& debug = mEditorProcess->mDebugSystem;

		bool& onGrid = debug->GetOnGrid();
		ImGui::Checkbox("Grid", &onGrid);

		bool& onBox = debug->GetOnBoxCollider();
		ImGui::Checkbox("BoxCollider", &onBox);

		bool& onCapsule = debug->GetOnCapsuleCollider();
		ImGui::Checkbox("CapsuleCollider", &onCapsule);

		bool& onSphere = debug->GetOnSphereCollider();
		ImGui::Checkbox("SphereCollider", &onSphere);

		bool& onConvex = debug->GetOnConvexMeshCollider();
		ImGui::Checkbox("ConvexMeshCollider", &onConvex);

		bool& onCharater = debug->GetOnCharaterController();
		ImGui::Checkbox("CharaterController", &onCharater);

		bool& onNavMesh = debug->GetOnNavigationMesh();
		ImGui::Checkbox("NavigationMesh", &onNavMesh);

		bool& onUseRenderDebug = debug->GetOnUseRenderDebug();
		ImGui::Checkbox("UseDebugRender", &onUseRenderDebug);

		ImGui::EndMenu();
	}
}
