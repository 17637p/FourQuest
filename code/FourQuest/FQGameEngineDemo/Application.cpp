#include "Application.h"


Application::Application()
	:mEngine(nullptr)
{}

Application::~Application()
{

}

bool Application::Initialize(Mode mode)
{
	if (mode == Mode::Game)
	{
		mEngine = fq::game_engine::Exporter::GetGameEngine();
	}
	else
	{
		mEngine = fq::game_engine::Exporter::GetEditorEngine();
	}

	return mEngine->Initialize();
}

void Application::Process()
{
	mEngine->Process();
}

void Application::Finalize()
{
	mEngine->Finalize();

	fq::game_engine::Exporter::DeleteEngine();
}
