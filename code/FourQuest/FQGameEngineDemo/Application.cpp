#include "Application.h"


Application::Application()
	:mEngine(nullptr)
{}

Application::~Application()
{

}

void Application::Initialize(Mode mode)
{
	if (mode == Mode::Game)
	{
		mEngine = fq::game_engine::Exporter::GetGameEngine();
	}
	else
	{
		mEngine = fq::game_engine::Exporter::GetEditorEngine();
	}

	mEngine->Initialize();
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
