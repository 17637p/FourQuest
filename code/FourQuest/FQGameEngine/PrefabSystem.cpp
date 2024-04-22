#include "PrefabSystem.h"

fq::game_engine::PrefabSystem::PrefabSystem()
	:mGameProcess(nullptr)
	,mEditorProcess(nullptr)
{}

fq::game_engine::PrefabSystem::~PrefabSystem()
{

}

void fq::game_engine::PrefabSystem::Initialize(GameProcess* game, EditorProcess* editor)
{
	mGameProcess = game;
	mEditorProcess = editor;
}

void fq::game_engine::PrefabSystem::Finalize()
{

}
