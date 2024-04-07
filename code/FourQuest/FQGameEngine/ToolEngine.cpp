#include "FQGameEnginePCH.h"
#include "ToolEngine.h"

#include "GameProcess.h"
#include "Editor.h"

#include "WindowSystem.h"


fq::game_engine::ToolEngine::ToolEngine()
	:mGameProcess(std::make_unique<GameProcess>())
	,mEditor(std::make_unique<Editor>())
{}

fq::game_engine::ToolEngine::~ToolEngine()
{}

void fq::game_engine::ToolEngine::Initialize()
{
	// 윈도우 창 초기화 
	mGameProcess->mWindowSystem->Initialize();

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
			if (msg.message == WM_QUIT)
			{
				bIsDone = true;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			mEditor->NewFrame();

			ImGui::Begin("hie");
			ImGui::End();

			mEditor->Render();
		}
	}
}

void fq::game_engine::ToolEngine::Finalize()
{
}

