#include "ToolEngine.h"

#include "../FQGameModule/GameModule.h"

#include "GameProcess.h"
#include "Editor.h"

#include "WindowSystem.h"


fq::game_engine::ToolEngine::ToolEngine()
	:mGameProcess(std::make_unique<GameProcess>())
	, mEditor(std::make_unique<Editor>())
{}

fq::game_engine::ToolEngine::~ToolEngine()
{}

void fq::game_engine::ToolEngine::Initialize()
{
	// ������ â �ʱ�ȭ 
	mGameProcess->mWindowSystem->Initialize();

	// GameProcess �ʱ�ȭ
	mGameProcess->mInputManager->
		Initialize(mGameProcess->mWindowSystem->GetHWND());

	mGameProcess->mSceneManager->Initialize("example"
		, mGameProcess->mEventManager.get()
		, mGameProcess->mInputManager.get());

	// Editor �ʱ�ȭ
	mEditor->Initialize(mGameProcess.get());
}

void fq::game_engine::ToolEngine::Process()
{
	MSG msg;

	bool bIsDone = false;
	while (!bIsDone)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				bIsDone = true;
			}
		}
		else
		{
			// ȭ�� ũ�� ����
			if (mGameProcess->mWindowSystem->IsResizedWindow())
			{
				mGameProcess->mWindowSystem->OnResize();
			}
			
			// �ð�,�Է� ó��
			float deltaTime = mGameProcess->mTimeManager->Update();
			mGameProcess->mInputManager->Update();

			// ����ó��
			mGameProcess->mSceneManager->FixedUpdate(0.f);
			
			mGameProcess->mSceneManager->Update(deltaTime);
			mGameProcess->mSceneManager->LateUpdate(deltaTime);

			mEditor->Update();
			mEditor->NewFrame();
			mEditor->Render();

			mGameProcess->mSceneManager->GetCurrentScene()->CleanUp();
		}
	}
}

void fq::game_engine::ToolEngine::Finalize()
{
	mGameProcess->mSceneManager->Finalize();
}

