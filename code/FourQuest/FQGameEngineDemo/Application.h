#pragma once

#include "../FQGameEngine/FQGameEngine.h"

class Application
{
public:
	enum class Mode
	{
		Game, 
		Tool
	};

	Application();
	~Application();

public:
	/// <summary>
	/// 초기화
	/// </summary>
	void Initialize(Mode mode);

	/// <summary>
	/// 게임 루프
	/// </summary>
	void Process();

	/// <summary>
	/// 종료
	/// </summary>
	void Finalize();

private:
	fq::game_engine::IEngine* mEngine;
};

