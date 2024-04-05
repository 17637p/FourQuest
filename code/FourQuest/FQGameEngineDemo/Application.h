#pragma once

#include <windows.h>
#include "../FQGameEngine/IEngine.h"

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
	void Initialize(HINSTANCE hInstance, Mode mode);

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

