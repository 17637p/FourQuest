
#include "IEngine.h"

#include "GameEngine.h"
#include "EditorEngine.h"
#include "WindowSystem.h"



fq::game_engine::IEngine* fq::game_engine::Exporter::mEngine = nullptr;

FQ_ENGNIE_API fq::game_engine::IEngine* fq::game_engine::Exporter::GetGameEngine()
{
	mEngine = new GameEngine();

	return mEngine;
}

FQ_ENGNIE_API fq::game_engine::IEngine* fq::game_engine::Exporter::GetEditorEngine()
{
	mEngine = new EditorEngine();

	return mEngine;
}

FQ_ENGNIE_API void fq::game_engine::Exporter::DeleteEngine()
{
	if (mEngine)
	{
		delete mEngine;
	}
}


