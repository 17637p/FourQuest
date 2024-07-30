#pragma once
#include "../FQGameModule/Component.h"

namespace fq::client
{
	class PauseUI : public game_module::Component
	{
	public:
		PauseUI();
		~PauseUI();

		virtual void OnStart() override;
		virtual void OnUpdate(float dt) override;

	private:
		int mSelectButtonID; // 0: 계속하기, 1: 환경설정, 2: 미션중단, 3: 게임 종료

		//std::vector<float> mButtonPosY; // 0~3 버튼 Y 위치 선택 UI 버튼 위치 옮기기 위해
		std::vector<game_module::GameObject*> mButtons; 
	};
}

