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
	/// �ʱ�ȭ
	/// </summary>
	void Initialize(Mode mode);

	/// <summary>
	/// ���� ����
	/// </summary>
	void Process();

	/// <summary>
	/// ����
	/// </summary>
	void Finalize();

private:
	fq::game_engine::IEngine* mEngine;
};

