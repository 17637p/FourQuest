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
		int mSelectButtonID; // 0: ����ϱ�, 1: ȯ�漳��, 2: �̼��ߴ�, 3: ���� ����

		//std::vector<float> mButtonPosY; // 0~3 ��ư Y ��ġ ���� UI ��ư ��ġ �ű�� ����
		std::vector<game_module::GameObject*> mButtons; 
	};
}

