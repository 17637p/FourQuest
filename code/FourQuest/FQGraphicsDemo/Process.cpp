#include "Process.h"

//#include "GUIManager.h"
#include "InputManager.h"

//temp
#include "../FQGraphics/IFQGraphics.h"

#include "../FQLoader/ModelConverter.h"
#include "../FQLoader/ModelLoader.h"

Process::Process()
	:
	mWindowPosX(0),
	mWindowPosY(0),
	mScreenWidth(1920),
	mScreenHeight(1080),
	mResizing(false),
	mTestGraphics(nullptr)
{
	CreateHWND(L"FQGraphicsDemo", WS_OVERLAPPEDWINDOW, mWindowPosX, mWindowPosY, mScreenWidth, mScreenHeight);
	ShowWindow(mHwnd, SW_SHOWNORMAL);

	mEngineExporter = std::make_shared<fq::graphics::EngineExporter>();
}

Process::~Process()
{
	for (fq::graphics::IStaticMeshObject* iobj :mStaticMeshObjects)
	{
		mTestGraphics->DeleteStaticMeshObject(iobj);
	}

	mEngineExporter->DeleteEngine(mTestGraphics);
}

bool Process::Init(HINSTANCE hInstance)
{
	InputManager::GetInstance().Init(mHwnd);
	//m_timer = std::make_unique<GameTimer>();

	mTestGraphics = mEngineExporter->GetEngine();
	mTestGraphics->Initialize(mHwnd, mScreenWidth, mScreenHeight);

	std::string basePath = "../Temp/Model/";
	std::string filePath = basePath + "gun/gun";
	{
		fq::loader::ModelConverter converter;
		converter.ReadFBXFile(basePath + "gun.fbx");
		converter.WriteModel(filePath);
	}

	fq::common::Model model = fq::loader::ModelLoader::ReadModel(filePath);

	for (const auto& material : model.Materials)
	{
		if (!material.Name.empty())
		{
			mTestGraphics->CreateMaterial(filePath + material.Name, material, basePath);
		}
	}

	for (const auto& nodeMeshPair : model.Meshes)
	{
		const auto& node = nodeMeshPair.first;
		const auto& mesh = nodeMeshPair.second;

		if (!mesh.Name.empty())
		{
			fq::graphics::MeshObjectInfo info;

			for (const auto& subset : mesh.Subsets)
			{
				std::string materialKey = filePath + subset.MaterialName;

				info.MaterialKeys.push_back(materialKey);
				info.Transform = nodeMeshPair.first.ToParentMatrix;
			}

			std::string meshKey = filePath + mesh.Name;
			info.MeshKey = meshKey;

			if (mesh.BoneVertices.empty())
			{
				mTestGraphics->CreateStaticMesh(meshKey, mesh);
				fq::graphics::IStaticMeshObject* object = mTestGraphics->CreateStaticMeshObject(info);
				mStaticMeshObjects.push_back(object);
			}
			else
			{
				mTestGraphics->CreateSkinnedMesh(meshKey, mesh);
				fq::graphics::ISkinnedMeshObject* object = mTestGraphics->CreateSkinnedMeshObject(info);
			}
		}
	}

	return true;
}

void Process::Loop()
{
	MSG msg;

	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
		}
	}
}

void Process::Finalize()
{
	//_guiManager->Finalize();
}

//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Process::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	//if (ImGui_ImplWin32_WndProcHandler(m_hWnd, uMsg, wParam, lParam))
	//	return true;

	switch (uMsg)
	{
	case WM_SIZE:
	{
		//if (m_pRenderer == nullptr)
		//{
		//	return 0;
		//}
		//m_pRenderer->SetClientSize(LOWORD(lParam), HIWORD(lParam));
	}

	case WM_PAINT:
	{
		hdc = BeginPaint(mHwnd, &ps);
		EndPaint(mHwnd, &ps);
		break;
	}

	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	}

	return DefWindowProc(mHwnd, uMsg, wParam, lParam);
}

void Process::Update()
{
	// ESC ��ư ������ ���α׷� ����
	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	InputManager::GetInstance().Update();
}

void Process::Render()
{
	mTestGraphics->BeginRender();

	mTestGraphics->Render();

	/// �׸��⸦ �غ��Ѵ�.
	//m_pRenderer->BeginRender();
	//
	///// �������� �׸��⸦ �Ѵ�.
	//m_pRenderer->Render(IImpGraphicsEngine::RendererType::Forward);
	//
	//if (_has2ndCamera)
	//{
	//	m_pRenderer->UpdateRight(m_timer->DeltaTime());
	//
	//	m_pRenderer->BeginRenderRight();
	//	m_pRenderer->RenderRight(IImpGraphicsEngine::RendererType::Forward);
	//}
	//
	//m_pRenderer->RenderUI();
	//_guiManager->Render();
	///// �׸��⸦ ������.
	//m_pRenderer->EndRender();

	for (auto& obj : mStaticMeshObjects)
	{
		obj->UpdateTransform(obj->GetTransform() * DirectX::SimpleMath::Matrix::CreateRotationY(0.001f));
	}

	mTestGraphics->EndRender();
}
