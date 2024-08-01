#pragma once
#include "../FQGameModule/Component.h"

namespace fq::game_module
{
	class ScreenManager;
}

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
		entt::meta_handle GetHandle() override { return *this; }
		std::shared_ptr<Component> Clone(std::shared_ptr<Component> clone /* = nullptr */)const override;

	private:
		int mSelectButtonID; // 0: ����ϱ�, 1: ȯ�漳��, 2: �̼��ߴ�, 3: ���� ����

		//std::vector<float> mButtonPosY; // 0~3 ��ư Y ��ġ ���� UI ��ư ��ġ �ű�� ����
		game_module::GameObject* mSelectBackground;
		std::vector<game_module::GameObject*> mButtons; 

		game_module::ScreenManager* mScreenManager;
	};
}

